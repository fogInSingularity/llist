#ifndef LLIST_H
#define LLIST_H

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <limits.h>

#include "debug.h"

//static-----------------------------------------------------------------------

static const size_t kLListMinAlloc = 8;

//public-----------------------------------------------------------------------

#define ERROR_M(errorM)                                                       \
  fputs(RED BOLD "error: " RESET errorM "\n", stdout);

typedef int elem_t;
typedef size_t index_t;

enum class LListError {
  SUCCESS                    = 0,
  CTOR_CANT_ALLOC            = 1,
  RECALLOC_CANT_ALLOC        = 2,
  REMOVE_INDEX_OOR           = 3,
  CANT_REMOVE_HEAD           = 4,
  INSERT_INDEX_OOR           = 5,
  FRONT_ACCESS_ON_EMPTY_LIST = 6,
  BACK_ACCESS_ON_EMPTY_LIST  = 7,
  POP_ON_EMPTY_LIST          = 8,
};

struct LLNode {
  elem_t elem;
  index_t next;
  index_t prev;
};

struct LList {
 public:
  LListError Ctor(size_t initCap = kLListMinAlloc);
  void Dtor();
  void ThrowError(LListError error);
  void DotDump();

  bool IsEmpty();
  size_t Size();
  size_t Capacity();

  LListError Front(index_t* indRet);
  LListError Back(index_t* indRet);

  LListError PushFront(elem_t elem);
  LListError PushBack(elem_t elem);
  LListError PopFront();
  LListError PopBack();

  LListError InsertAfter(index_t ind, elem_t elem);
  LListError RemoveAt(index_t ind);
  LListError Linearize();
 private:
  LLNode* list_;
  size_t size_;
  size_t cap_;

  index_t free_;

  LListError Recalloc();
  LListError ResizeUp();
  LListError ResizeDown();
  LListError LinkFree();
};

#endif // LLIST_H
