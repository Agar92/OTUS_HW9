#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <atomic>
#include <filesystem>
#include <chrono>

#include <ts_queue.h>

using namespace std::chrono_literals;

struct block_t {
    std::string t_stamp;
    std::string cmd;
};

struct conn_t {
    size_t limit_cmd{}, cnt_cmd{}, cnt_brace{};
    block_t block_cmd;
    bool empty() {
        return block_cmd.t_stamp.empty();
    }
};

struct Bulk {
    Bulk(){
        curdir=std::filesystem::current_path();
        std::filesystem::create_directory("log");
        //std::cout<<"curdir="<<curdir<<std::endl;
    }
    ~Bulk() {
        std::this_thread::sleep_for(200ms);
        std::unique_lock<std::mutex> lock(mut_finished);
        while(!STOP){
            std::cout<<"WAIT"<<" finished="<<finished
                     <<" STOP="<<STOP
                     <<std::endl;
            cv_finished.wait(lock);
        }
        if(false)
        {
        cv_finished.wait(lock,[this]{
            /*
            std::cout<<"@@@finished="<<finished
                     <<" conn_pool.size()="<<conn_pool.size()
                     <<" q_log.empty()="<<std::boolalpha<<q_log.empty()
                     <<" q_file.empty()="<<std::boolalpha<<q_file.empty()
                     <<std::endl;
            */
            return finished.load();
            /*
            return conn_pool.empty() &&
                   q_log.empty()     &&
                   q_file.empty();
            */
        });
        }
        //while (!finished);
        q_log.wake_and_done();
        q_file.wake_and_done();

        log.join();
        file1.join();
        file2.join();
    }

    void connect(const size_t id) {
        conn_pool.insert({++ID, {id, {}}});
    }

    void receive(const char *buff, size_t buff_size, const size_t &id) {
        std::string line(buff, buff_size);
        if (input(std::move(line), id)) {
            finished = false;
            q_log.push(std::move(conn_pool[id].block_cmd));
        }
        //finished=true;
        //cv_finished.notify_all();
    }

    void disconnect(const size_t &id) {
        conn_pool.erase(id);
    }

private:
    bool input(std::string &&line, const size_t &id) {
        if (line == "{")
            return conn_pool[id].cnt_brace++ == 0 && (conn_pool[id].cnt_cmd = 0, !conn_pool[id].empty());

        if (line == "}")
            return --conn_pool[id].cnt_brace == 0;

        if (conn_pool[id].cnt_cmd++ == 0) {
            conn_pool[id].block_cmd = block_new(line);
        }
        else conn_pool[id].block_cmd.cmd += ", " + line;

        if (conn_pool[id].cnt_brace == 0) {
            return conn_pool[id].cnt_cmd == conn_pool[id].limit_cmd && (conn_pool[id].cnt_cmd = 0, true);
        }
        return false;
    }

    block_t block_new(const std::string &line) {
        auto t = std::chrono::system_clock::now().time_since_epoch();
        auto utc = std::chrono::duration_cast<std::chrono::microseconds>(t).count();
        return {std::to_string(utc) + "_", "bulk: " + line};
    }

    void to_log_q() {
        block_t block;
        while (q_log.wait_and_pop(block)) {
            std::cout << block.cmd << '\n';
            q_file.push(block);
        }
    }

    void to_file_q(size_t id) {
        block_t block;
        while (q_file.wait_and_pop(block)) {
            std::lock_guard<std::mutex> lock(mut_finished);
            std::ofstream file("log/" + block.t_stamp + std::to_string(id) + ".log");
            file << block.cmd;
            file.close();
            finished = conn_pool.empty() && q_log.empty() && q_file.empty();
            if(finished){
                STOP=true;
                std::cout<<"!!!finished="<<finished<<" STOP="<<STOP<<std::endl;
                cv_finished.notify_all();
                std::cout<<"ooooooooooEND"<<std::endl;
            }
            else
                std::cout<<"#########END"<<std::endl;
        }
    }

    std::string curdir;
    std::atomic<bool> STOP{false};
    std::atomic<bool> finished{false};
    std::map<size_t, conn_t> conn_pool;
    ts_queue<block_t> q_log{};
    ts_queue<block_t> q_file{};
    std::thread log{&Bulk::to_log_q, this};
    std::thread file1{&Bulk::to_file_q, this, 2};
    std::thread file2{&Bulk::to_file_q, this, 3};
    //
    std::mutex mut_finished;
    std::condition_variable cv_finished;
    std::atomic<size_t> ID;
};

Bulk bulk;

size_t connect(size_t N) {
    bulk.connect(N);
    return N;
}

void receive(const char *buff, size_t buff_size, const size_t &id) {
    std::cout<<"ENTER receive: "<<buff<<" "<<buff_size<<" "<<id<<std::endl;
    bulk.receive(buff, buff_size, id);
    std::cout<<"EXIT receive: "<<buff<<" "<<buff_size<<" "<<id<<std::endl;
}

void disconnect(const size_t &id) {
    std::cout<<"ENTER disconnect "<<id<<std::endl;
    bulk.disconnect(id);
    std::cout<<"EXIT disconnect "<<id<<std::endl;
}

