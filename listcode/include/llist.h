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

typedef int elem_t;

enum class LListError {
  SUCCESS,
  CTOR_CANT_ALLOC,
  RECALLOC_CANT_ALLOC,
  REMOVE_INDEX_OOR,
  INSERT_INDEX_OOR,
};

struct LLNode {
  elem_t elem;
  uint64_t next;
  uint64_t prev;
};

struct LList {
 public:
  LLNode* list_;
  size_t size_;
  size_t cap_;

  uint64_t free_;

  LListError Ctor(size_t initCap = kLListMinAlloc);
  void Dtor();
  void ThrowError(LListError error);
  void DotDump();

  LListError InsertAfter(uint64_t ind, elem_t elem);
  LListError RemoveAt(uint64_t ind);
  LListError Linearize();
 private:
  LListError Recalloc();
  LListError ResizeUp();
  LListError ResizeDown();
  LListError LinkFree();
};

#endif // LLIST_H
