#include <ConsoleWriter.h>

#include <iostream>
#include <sstream>

void ConsoleWriter::write(uint8_t, const Bulk& bulk) {
  add_job([this, bulk](){
    os_ << bulk;
  });
}

