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
        //std::this_thread::sleep_for(200ms);
        std::unique_lock<std::mutex> lock(finished_mutex);
        cv_finished.wait(lock, [this]{return finished.load();});
        
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
        std::lock_guard<std::mutex> lock(connection_mutex);
        connection_pool.erase(id);
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
        block_t block;
        while (queue_log.wait_and_pop(block)) {
            std::cout << block.cmd << '\n';
            queue_file.push(block);
        }
    }

    void to_file_queue(size_t id) {
        block_t block;
        while (queue_file.wait_and_pop(block)) {
            std::lock_guard<std::mutex> lock(finished_mutex);
            std::ofstream file("log/" + block.t_stamp + std::to_string(id) + ".log");
            file << block.cmd;
            file.close();
            finished = connection_pool.empty() && queue_log.empty() && queue_file.empty();
            if(finished){
                cv_finished.notify_all();
            }
            else;
        }
    }

    std::string curdir;
    std::atomic<bool> finished{false};
    std::map<size_t, conn_t> connection_pool;
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

Bulk bulk;

size_t connect(size_t N) {
    return bulk.connect(N);
}

void receive(const char *buff, size_t buff_size, const size_t &id) {
    bulk.receive(buff, buff_size, id);
}

void disconnect(const size_t &id) {
    bulk.disconnect(id);
}

