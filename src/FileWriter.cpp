#include <FileWriter.h>

#include <fstream>

void FileWriter::write(uint8_t context_id, const Bulk& bulk) {
  add_job([this, context_id, bulk](){
    std::string file_name = "bulk" + std::to_string(bulk.time()) + "_" +
                            std::to_string(context_id) +  "_" +
                            std::to_string(get_job_id()) + ".log";
    std::fstream fs{std::string("log/") + file_name, std::ios::app};

    if(fs.is_open()) {
      fs << bulk;
      fs.close();
    }
  });
}

