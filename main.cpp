#include "async.hpp"

int main()
{ 
  auto handler = connect(3);
  
  receive(handler, "cmd1\ncmd2");
  receive(handler, "cmd3\ncmd4\ncmd5");
  receive(handler, "cmd6\ncmd7");
  
  disconnect(handler);
  delete handler;
  
  return 0;
}
