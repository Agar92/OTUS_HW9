#include <iostream>
#include <chrono>
#include <fstream>

#include <handler.hpp>

using namespace std::chrono_literals;

Handler::Handler(std::size_t n)
{
  m_default_size = n;
  m_threads[0] = std::thread(&Handler::consoleWriter, this);
  m_threads[1] = std::thread(&Handler::fileWriter, this, "A");
  m_threads[2] = std::thread(&Handler::fileWriter, this, "B");
}
 
Handler::~Handler()
{
  for(auto& th : m_threads) th.join();
}

void Handler::handle(std::string_view lines)
{
  std::istringstream input(lines.data());
  std::string line;
  while(std::getline(input, line))
  {
    if(line == "{")
    {
      if(!m_block_started)
      {
        m_block_started = true;
        write();
      }
      continue;
    }  
    if(line == "}")
    {
      if(m_block_started)
      {
        m_block_started = false;
        write();
      }
      continue;
    }
    m_lines << line << " ";
    m_counter++;  
    if(!m_block_started)
      if(m_counter == m_default_size) write();
  }
}
  
void Handler::stop()
{
  if(m_counter > 0) write();    
  std::unique_lock<std::mutex> lock(m_mutex);
  for(auto& th : m_threads)
  {
    m_threadStates[th.get_id()].m_type = StateType::Terminating;
    m_threadStates[th.get_id()].m_done = false;
  }  
  m_events.push(Event::Terminate);
  m_cv.wait(lock, [&]{
    return
       m_threadStates[m_threads[0].get_id()].m_done &&
       m_threadStates[m_threads[1].get_id()].m_done &&
       m_threadStates[m_threads[2].get_id()].m_done;
  });  
  for(auto& t : m_threads) m_threadStates[t.get_id()].m_done = false;
  m_events.pop();
}
  
void Handler::write()
{
  if(m_counter == 0) return;
  std::unique_lock<std::mutex> lock(m_mutex);
  for(auto& th : m_threads)
  {
    m_threadStates[th.get_id()].m_type = StateType::Writing;
    m_threadStates[th.get_id()].m_done = false;
  }  
  m_events.push(Event::Write);
  m_cv.wait(lock, [&]{
    return
       m_threadStates[m_threads[0].get_id()].m_done &&
       m_threadStates[m_threads[1].get_id()].m_done &&
       m_threadStates[m_threads[2].get_id()].m_done;
  });  
  for(auto& t : m_threads) m_threadStates[t.get_id()].m_done = false;
  m_events.pop();  
  m_counter = 0;
  m_lines.str("");
}

void Handler::consoleWriter()
{
  auto thread_id = std::this_thread::get_id();
  {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_threadStates[thread_id].m_type = StateType::Pending;
    m_threadStates[thread_id].m_done = false;
  }
  Event event;  
  for(;;)
  {
    {
      std::unique_lock<std::mutex> lock{m_mutex};
      if (m_events.empty()|| m_threadStates[thread_id].m_done)
      {
        lock.unlock();
        std::this_thread::sleep_for(5ms);
        continue;
      }
      event = m_events.front();
    }  
    std::unique_lock<std::mutex> lock(m_mutex);
    if(event == Event::Write)
    {
      std::cout << "bulk: " << m_lines.str() << "\n";
      m_threadStates[thread_id].m_type = StateType::Pending;
      m_threadStates[thread_id].m_done = true;
      lock.unlock();
      m_cv.notify_all();
      continue;
    }  
    if(event == Event::Terminate)
    {
      m_threadStates[thread_id].m_type = StateType::Stopped;
      m_threadStates[thread_id].m_done = true;
      lock.unlock();
      m_cv.notify_all();
      return;
    }
  }
}
  
void Handler::fileWriter(std::string_view string)
{
  auto thread_id = std::this_thread::get_id();
  {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_threadStates[thread_id].m_type = StateType::Pending;
    m_threadStates[thread_id].m_done = false;
  }
  std::stringstream filename;
  std::ofstream out;
  Event event;  
  for(;;)
  {
    {
      std::unique_lock<std::mutex> lock{m_mutex};
      if (m_events.empty()|| m_threadStates[thread_id].m_done)
      {
        lock.unlock();
        std::this_thread::sleep_for(5ms);
        continue;
      }
      event = m_events.front();
    }  
    std::unique_lock<std::mutex> lock(m_mutex);
    if(event == Event::Write)
    {
      filename << "bulk-" << thread_id << "-" 
               << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
               << string;
      out.open (filename.str(), std::ios::out);
      filename.str(""); 
      out << "bulk: " << m_lines.str() << "\n";  
      m_threadStates[thread_id].m_type = StateType::Pending;
      m_threadStates[thread_id].m_done = true;
      lock.unlock();
      m_cv.notify_all();
      out.close();
      continue;
    }  
    if(event == Event::Terminate)
    {
      m_threadStates[thread_id].m_type = StateType::Stopped;
      m_threadStates[thread_id].m_done = true;
      lock.unlock();
      m_cv.notify_all();
      return;
    }
  }
}
