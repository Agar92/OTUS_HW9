#include <Bulk.h>

void Bulk::push(const std::string& cmd) {
  if(pool_.empty())
    time_ = std::time(nullptr);
  if(!cmd.empty())
    pool_.emplace_back(cmd);
}

void Bulk::clear() {
  pool_.clear();
  time_ = {};
}

size_t Bulk::size() const {
  return pool_.size();
}

std::vector<std::string> Bulk::get_cmds() const {
  return pool_;
}

std::time_t Bulk::time() const {
  return time_;
}

std::ostream& operator <<(std::ostream& os, const Bulk& bulk) {
  os << "bulk: ";
  for(const auto &it: bulk.pool_) {
    os << it << (&it != &bulk.pool_.back() ? ", " : "\n");
  }
  return os;
}

