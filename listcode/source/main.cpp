#include "llist.h"

int main() {
  LList list = {};
  list.Ctor();

  list.InsertAfter(0, 123);
  list.DotDump();
  list.PushFront(4);
  list.DotDump();
  list.PushFront(2);
  list.DotDump();

  list.Dtor();

  return 0;
}
