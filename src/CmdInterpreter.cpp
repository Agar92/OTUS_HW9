#include <CmdInterpreter.h>

bool CmdInterpreter::interpret(const std::string& input, std::string& cmd) {
  if(input.empty()) {
    /// Завершение по EOF.
    if((tokens_ == 0) && (size_ > 0)) {
      size_ = 0;
      return true;
    }
  } else if(input == "{") {
    /// Начало блока.
    if((tokens_++ == 0) && (size_ > 0)) {
      size_ = 0;
      return true;
    }
  } else if(input == "}") {
    /// Конец блока.
    if(tokens_ > 0)
      if(--tokens_ == 0)
        return true;
  } else {
    /// Команда.
    cmd = input;
    if((tokens_ == 0) && (++size_ == bulk_size_)) {
      size_ = 0;
      return true;
    }
  }
  return false;
}

