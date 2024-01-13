#include <iostream>
#include <fstream>
#include <unordered_map>
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
        std::unique_lock<std::mutex> lock(finished_mutex);
        cv_finished.wait(lock, [this]{
            //we don't need  checkong the condition
            //queue_log.empty() && queue_file.empty()
            //Sometimes it will be true, sometimes - not.
            //But the below lines
            //queue_log.wake_and_done();
            //queue_file.wake_and_done();
            //will force it to be true.
            return connection_pool.empty();
        });
        
        queue_log.wake_and_done();
        queue_file.wake_and_done();

        log.join();
        file1.join();
        file2.join();
    }

    size_t connect(const size_t id) {
        std::lock_guard<std::mutex> lock(connection_mutex);
        connection_pool.insert({ID, {id, {}}});
        return ID++;
    }

    void receive(const char *buff, size_t buff_size, const size_t &id) {
        std::string line(buff, buff_size);
        if (input(std::move(line), id)) {
            finished = false;
            queue_log.push(std::move(connection_pool[id].block_cmd));
        }
    }

    void disconnect(const size_t &id) {
        std::unique_lock<std::mutex> lk{connection_mutex};
        connection_pool.erase(id);
        lk.unlock();
        cv_finished.notify_all();
    }

private:
    bool input(std::string &&line, const size_t &id) {
        if (line == "{")
            return connection_pool[id].cnt_brace++ == 0 && (connection_pool[id].cnt_cmd = 0, !connection_pool[id].empty());

        if (line == "}")
            return --connection_pool[id].cnt_brace == 0;

        if (connection_pool[id].cnt_cmd++ == 0) {
            connection_pool[id].block_cmd = block_new(line);
        }
        else connection_pool[id].block_cmd.cmd += ", " + line;

        if (connection_pool[id].cnt_brace == 0) {
            return connection_pool[id].cnt_cmd == connection_pool[id].limit_cmd && (connection_pool[id].cnt_cmd = 0, true);
        }
        return false;
    }

    block_t block_new(const std::string &line) {
        auto t = std::chrono::system_clock::now().time_since_epoch();
        auto utc = std::chrono::duration_cast<std::chrono::microseconds>(t).count();
        return {std::to_string(utc) + "_", "bulk: " + line};
    }

    void to_log_queue() {
        try {
            block_t block;
            while (queue_log.wait_and_pop(block)) {
                std::cout << block.cmd << '\n';
                queue_file.push(block);
            }
        }
        catch (std::exception & e) {
            std::cout<<"***Exception to_log_queue: "<<e.what()<<std::endl;
        }
    }

    void to_file_queue(size_t id) {
        try {
            ///throw std::bad_alloc{};
            block_t block;
            while (queue_file.wait_and_pop(block)) {
                //line 33:
                //std::unique_lock<std::mutex> lock(finished_mutex);
                //That is why the line below leads to a deadlock:
                //std::lock_guard<std::mutex> lock(finished_mutex);
                std::ofstream file("log/" + block.t_stamp + std::to_string(id) + ".log");
                file << block.cmd;
                file.close();
                //This is not necessary here.
                //We need just check that the connection_pool is empty.
                /////finished = connection_pool.empty() && queue_log.empty() && queue_file.empty();
                /////if(finished) cv_finished.notify_all();
            }
        }
        catch (std::exception & e) {
            std::cout<<"***Exception to_file_queue: "<<e.what()<<std::endl;
        }
    }

    std::string curdir;
    std::atomic<bool> finished{false};
    std::unordered_map<size_t, conn_t> connection_pool;
    ts_queue<block_t> queue_log{};
    ts_queue<block_t> queue_file{};
    std::thread log{&Bulk::to_log_queue, this};
    std::thread file1{&Bulk::to_file_queue, this, 2};
    std::thread file2{&Bulk::to_file_queue, this, 3};
    //
    std::mutex finished_mutex;
    std::condition_variable cv_finished;
    size_t ID=0;
    std::mutex connection_mutex;
};

namespace {
    Bulk bulk;
}

size_t connect(size_t N) {
    return bulk.connect(N);
}

void receive(const char *buff, size_t buff_size, const size_t &id) {
    bulk.receive(buff, buff_size, id);
}

void disconnect(const size_t &id) {
    bulk.disconnect(id);
}

