#include <CmdProcessContext.h>

#include <algorithm>
#include <istream>
#include <iostream>
#include <thread>

void CmdProcessContext::subscribe(std::unique_ptr<IStreamWriter> observer) {
  auto it = std::find(observers_.begin(), observers_.end(), observer);
  if(it == observers_.end()) observers_.emplace_back(std::move(observer));
}

void CmdProcessContext::process(const char* data, std::size_t size, bool finish_bulk) {
  if(nullptr != data) data_.append(data, size);
  size_t rows{};
  while(true) {
    auto end_line = finish_bulk ? data_.size() : data_.find('\n');
    if(end_line != std::string::npos) {
      std::string input = data_.substr(0, end_line);
      std::string cmd;
      bool is_bulk_end = interpreter_.interpret(input, cmd);
      bulk_.push(cmd);
      if(is_bulk_end)
      {
        publish(bulk_);
        bulk_.clear();
        rows = 0;
        if(finish_bulk) break;
      }
      data_ = data_.substr(++end_line);
    }
    else break;
  }
}

void CmdProcessContext::publish(const Bulk& bulk) {
  for(auto& it: observers_) it->write(id_, bulk);
}

