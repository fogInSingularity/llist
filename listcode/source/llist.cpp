#include "../include/llist.h"

//static-----------------------------------------------------------------------

static const size_t kLListMul = 2;
static const uint64_t kFreeTrashRef = __UINT64_MAX__;
static const elem_t kFreeTrashValue = INT_MIN;

//public-----------------------------------------------------------------------

LListError LList::Ctor(size_t initCap) {
  cap_ = initCap > kLListMinAlloc ?
         initCap : kLListMinAlloc;
  list_ = (LLNode*)calloc(initCap, sizeof(LLNode));
  if (list_ == nullptr) {
    cap_ = 0;
    return LListError::CTOR_CANT_ALLOC;
  }

  list_[0] = {INT_MAX, 0, 0}; // head and tail
  free_ = 0;

  size_ = 1;

  LListError error = LinkFree();
  if (error != LListError::SUCCESS) { return error; }

  return LListError::SUCCESS;
}

void LList::Dtor() {
  free(list_);
  list_ = nullptr;
  size_ = 0;
  cap_ = 0;
}

void LList::ThrowError(LListError error) {
  switch (error) {
    case LListError::SUCCESS:
      break;
    case LListError::CTOR_CANT_ALLOC:
      break;
    case LListError::RECALLOC_CANT_ALLOC:
      break;
    case LListError::REMOVE_INDEX_OOR:
      break;
    case LListError::INSERT_INDEX_OOR:
      break;
    default:
      ASSERT(0 && "UNKNOWN ERROR CODE");
      break;
  }
}

LListError LList::InsertAfter(uint64_t ind, elem_t elem) {
  LListError error = ResizeUp();
  if (error != LListError::SUCCESS) {
    return error;
  }

  uint64_t freeIndex = free_;

  free_ = list_[freeIndex].next; // change free

  list_[freeIndex].elem = elem; // set elem

  list_[list_[ind].next].prev = freeIndex; // []->
  list_[freeIndex].next = list_[ind].next;

  list_[freeIndex].prev = ind; // ->[]
  list_[ind].next = freeIndex;

  size_++;

  return LListError::SUCCESS;
}

LListError LList::RemoveAt(uint64_t ind) {
  if (ind > size_) {
    return LListError::REMOVE_INDEX_OOR;
  }

  LListError error = ResizeDown();
  if (error != LListError::SUCCESS) {
    return error;
  }

  list_[list_[ind].next].prev = list_[ind].prev; // ->[]
  list_[list_[ind].prev].next = list_[ind].next; // []->

  list_[ind].elem = kFreeTrashValue;

  list_[ind].next = free_;

  list_[ind].prev = kFreeTrashRef;
  free_ = ind;

  size_--;

  return LListError::SUCCESS;
}

void LList::DotDump() {
  FILE* dotfile = fopen("dotdump.dot", "w");
  fprintf(dotfile, "digraph {\n");
  fprintf(dotfile, "  rankdir=LR\n");
  fprintf(dotfile, "  node [shape = record];");

  uint64_t index = 0;
  do {
    fprintf(dotfile,
            "	node%lu [shape = Mrecord, style = filled, fillcolor = cyan,"
            " label = \" index: %lu  | elem: %d | next: %lu | prev: %lu\"];\n",
            index, index, list_[index].elem, list_[index].next, list_[index].prev);
    fprintf(dotfile, "  node%lu -> node%lu\n", index, list_[index].next);
    fprintf(dotfile, "  node%lu -> node%lu\n", list_[index].next, index);
    index = list_[index].next;
  } while (index != 0);

  fprintf(dotfile,
          "	list [shape = Mrecord, style = filled, fillcolor = green,"
          " label = \"size: %lu | cap: %lu\"];\n", size_, cap_);

  fprintf(dotfile,
          "	free [shape = Mrecord, style = filled, fillcolor = red,"
          " label = \"free | next: %lu\"];\n", free_);
  fprintf(dotfile, "  free -> free%lu\n", free_);
  index = free_;

  while (index != list_[index].next) {
    fprintf(dotfile,
            "	free%lu [shape = Mrecord, style = filled, fillcolor = orange,"
            " label = \" index: %lu  | elem: %d | next: %lu | prev: %lu\"];\n",
            index, index, list_[index].elem, list_[index].next, list_[index].prev);
    fprintf(dotfile, "  free%lu -> free%lu\n", index, list_[index].next);
    index = list_[index].next;
  }
  fprintf(dotfile,
          "	free%lu [shape = Mrecord, style = filled, fillcolor = orange,"
          " label = \" index: %lu  | elem: %d | next: %lu | prev: %lu\"];\n",
          index, index, list_[index].elem, list_[index].next, list_[index].prev);
  fprintf(dotfile, "  free%lu -> free%lu\n", index, list_[index].next);

  fprintf(dotfile, "}\n");
}

LListError LList::Linearize() {
  LLNode* hold = (LLNode*)calloc(size_ * kLListMul, sizeof(LLNode));
  cap_ = size_ * kLListMul;

  uint64_t index = 0;
  uint64_t storeIndex = 0;
  do {
    hold[storeIndex] = {list_[index].elem, storeIndex + 1, storeIndex - 1};
    index = list_[index].next;
    storeIndex++;
  } while (index != 0);

  hold[0].prev = size_ - 1;
  hold[size_ - 1].next = 0;

  free(list_);
  list_ = hold;

  return LinkFree();
}

//private----------------------------------------------------------------------

LListError LList::Recalloc() {
  LLNode* hold = list_;
  list_ = (LLNode*)realloc(list_, cap_*sizeof(LLNode));
  if (list_ == nullptr) {
    list_ = hold;
    hold = nullptr;

    return LListError::RECALLOC_CANT_ALLOC;
  }

  memset(list_ + size_, 0, (cap_ - size_)*sizeof(LLNode));

  return LListError::SUCCESS;
}

LListError LList::ResizeUp() {
  if (cap_ <= size_) {
    cap_ *= kLListMul;

    if (Recalloc() == LListError::RECALLOC_CANT_ALLOC) {
      cap_ /= kLListMul;

      return LListError::RECALLOC_CANT_ALLOC;
    }

    return LinkFree();
  }

  return LListError::SUCCESS;
}

LListError LList::ResizeDown() {
  if (cap_ >= size_ * kLListMul * kLListMul) {
    LListError error = Linearize();
    if (error != LListError::SUCCESS) { return error; }

    cap_ /= kLListMinAlloc;
    if (Recalloc() == LListError::RECALLOC_CANT_ALLOC) {
      cap_ *= kLListMul;

      return LListError::RECALLOC_CANT_ALLOC;
    }

    return LinkFree();
  }

  return LListError::SUCCESS;
}

LListError LList::LinkFree() {
  free_ = size_;
  for (size_t ind = size_; ind < cap_; ind++) {
    list_[ind].next = ind + 1;
    list_[ind].prev = kFreeTrashRef;
    list_[ind].elem = kFreeTrashValue;
  }
  list_[cap_-1].next = cap_ - 1;

  return LListError::SUCCESS;
}
