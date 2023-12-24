#include <iostream>
#include <async.h>

int main(int, char *[]) {
  std::size_t bulk = 5;
  auto h = connect(bulk);
  auto h2 = connect(bulk);
  receive(h, "1", 1);
  receive(h2, "1\n", 2);
  receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
  receive(h, "b\nc\nd\n}\n89\n", 11);
  disconnect(h);
  disconnect(h2);

  return 0;
}
