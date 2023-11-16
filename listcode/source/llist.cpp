#include "llist.h"

//static-----------------------------------------------------------------------

static const size_t kLListMul = 2;
static const uint64_t kFreeTrashRef = __UINT64_MAX__;
static const elem_t kFreeTrashValue = INT_MIN;
static const index_t kHeadTail = 0;

//public-----------------------------------------------------------------------

LListError LList::Ctor(size_t initCap) {
  cap_ = initCap > kLListMinAlloc ?
         initCap : kLListMinAlloc;

  list_ = (LLNode*)calloc(initCap, sizeof(LLNode));
  if (list_ == nullptr) {
    size_ = 0;
    cap_ = 0;

    return LListError::CTOR_CANT_ALLOC;
  }

  list_[kHeadTail] = {INT_MAX, 0, 0}; // head and tail

  size_ = 1;

  free_ = 0;
  LListError error = LinkFree();
  if (error != LListError::SUCCESS) { return error; }

  return LListError::SUCCESS;
}

void LList::Dtor() { // NOTE down up destr
  free(list_);
  list_ = nullptr;
  free_ = 0;

  size_ = 0;
  cap_ = 0;
}

void LList::ThrowError(LListError error) {
  switch (error) {
    case LListError::SUCCESS:
      //---//
      break;
    case LListError::CTOR_CANT_ALLOC:
      ERROR_M("constructor cant allocate memory");
      break;
    case LListError::RECALLOC_CANT_ALLOC:
      ERROR_M("recalloc cant allocate memory");
      break;
    case LListError::REMOVE_INDEX_OOR:
      ERROR_M("RemoveAt() function call out of range");
      break;
    case LListError::CANT_REMOVE_HEAD:
      ERROR_M("RemoveAt() function call on head");
      break;
    case LListError::INSERT_INDEX_OOR:
      ERROR_M("InsertAfter() function call out of range");
      break;
    case LListError::FRONT_ACCESS_ON_EMPTY_LIST:
      ERROR_M("Front() function call on empty list");
      break;
    case LListError::BACK_ACCESS_ON_EMPTY_LIST:
      ERROR_M("Back() function call on empty list");
      break;
    case LListError::SET_INDEX_OOR:
      ERROR_M("SetElemAt() function call out of range");
      break;
    case LListError::CANT_SET_HEAD:
      ERROR_M("SetElemAt() function call on head");
      break;
    case LListError::GET_ELEM_INDEX_OOR:
      ERROR_M("GetElemAt() function call out of range");
      break;
    case LListError::CANT_GET_ELEM_OF_HEAD:
      ERROR_M("GetElemAt() function call on head");
      break;
    case LListError::GET_NEXT_INDEX_OOR:
      ERROR_M("GetNextAr() function call out of range");
      break;
    case LListError::GET_PREV_INDEX_OOR:
      ERROR_M("GetPrevAt() function call out of range");
      break;
    case LListError::POP_ON_EMPTY_LIST:
      ERROR_M("Pop() function call on empty list");
      break;
    default:
      ASSERT(0 && "UNKNOWN ERROR CODE");
      break;
  }
}

bool LList::IsEmpty() {
  return !(Size());
}

size_t LList::Size() {
  return size_ - 1;
}

size_t LList::Capacity() {
  return cap_ - 1;
}

LListError LList::Front(index_t* indRet) {
  ASSERT(indRet != nullptr);

  if (IsEmpty()) { return LListError::FRONT_ACCESS_ON_EMPTY_LIST; }

  *indRet = list_[kHeadTail].next;

  return LListError::SUCCESS;
}

LListError LList::Back(index_t* indRet) {
  ASSERT(indRet != nullptr);

  if (IsEmpty()) { return LListError::BACK_ACCESS_ON_EMPTY_LIST; }

  *indRet = list_[kHeadTail].prev;

  return LListError::SUCCESS;
}

LListError LList::SetElemAt(index_t ind, elem_t elem) {
  LListError error = OutOfRangeCheck(ind, LListError::SET_INDEX_OOR);
  if (error != LListError::SUCCESS) { return error; }

  if (ind == kHeadTail) { return LListError::CANT_SET_HEAD; }

  list_[ind].elem = elem;

  return LListError::SUCCESS;
}

LListError LList::GetElemAt(index_t ind, elem_t* elemRet) {
  ASSERT(elemRet != nullptr);

  LListError error = OutOfRangeCheck(ind, LListError::GET_ELEM_INDEX_OOR);
  if (error != LListError::SUCCESS) { return error; }

  if (ind == kHeadTail) { return LListError::CANT_GET_ELEM_OF_HEAD; }

  *elemRet = list_[ind].elem;

  return LListError::SUCCESS;
}

LListError LList::GetNextAt(index_t ind, index_t* indRet) {
  ASSERT(indRet != nullptr);

  LListError error = OutOfRangeCheck(ind, LListError::GET_NEXT_INDEX_OOR);
  if (error != LListError::SUCCESS) { return error; }

  *indRet = list_[ind].next;

  return LListError::SUCCESS;
}

LListError LList::GetPrevAt(index_t ind, index_t* indRet) {
  ASSERT(indRet != nullptr);

  LListError error = OutOfRangeCheck(ind, LListError::GET_PREV_INDEX_OOR);
  if (error != LListError::SUCCESS) { return error; }

  *indRet = list_[ind].prev;

  return LListError::SUCCESS;
}

LListError LList::PushFront(elem_t elem) {
  return InsertAfter(kHeadTail, elem);
}

LListError LList::PushBack(elem_t elem) {
  return InsertAfter(list_[kHeadTail].prev, elem);
}

LListError LList::PopFront() {
  if (IsEmpty()) { return LListError::POP_ON_EMPTY_LIST; }

  return RemoveAt(list_[kHeadTail].next);
}

LListError LList::PopBack() {
  if (IsEmpty()) { return LListError::POP_ON_EMPTY_LIST; }

  return RemoveAt(list_[kHeadTail].prev);
}

LListError LList::InsertAfter(index_t ind, elem_t elem) {
  LListError error = OutOfRangeCheck(ind, LListError::INSERT_INDEX_OOR);
  if (error != LListError::SUCCESS) { return error; }

  error = ResizeUp();
  if (error != LListError::SUCCESS) { return error; }

  index_t freeIndex = free_;

  free_ = list_[freeIndex].next; // change free

  list_[freeIndex].elem = elem; // set elem

  list_[list_[ind].next].prev = freeIndex; // []->
  list_[freeIndex].next = list_[ind].next;

  list_[freeIndex].prev = ind; // ->[]
  list_[ind].next = freeIndex;

  size_++;

  return LListError::SUCCESS;
}

LListError LList::RemoveAt(index_t ind) {
  LListError error = OutOfRangeCheck(ind, LListError::REMOVE_INDEX_OOR);
  if (error != LListError::SUCCESS) { return error; }

  if (ind == kHeadTail) { return LListError::CANT_REMOVE_HEAD; }

  list_[list_[ind].next].prev = list_[ind].prev; // ->[]
  list_[list_[ind].prev].next = list_[ind].next; // []->

  list_[ind].elem = kFreeTrashValue;

  list_[ind].next = free_;
  free_ = ind;

  list_[ind].prev = kFreeTrashRef;

  size_--;

  error = ResizeDown();
  if (error != LListError::SUCCESS) { return error; }

  return LListError::SUCCESS;
}

//FIXME - holy shit
void LList::DotDump() {
  FILE* dotfile = fopen("dotdump.dot", "w");
  fprintf(dotfile, "digraph {\n");
  fprintf(dotfile, "  rankdir=LR\n");
  fprintf(dotfile, "  node [shape = record];");

  index_t index = kHeadTail;
  do {
    fprintf(dotfile,
            "	node%lu [shape = Mrecord, style = filled, fillcolor = cyan,"
            " label = \" index: %lu  | elem: %d | next: %lu | prev: %lu\"];\n",
            index, index, list_[index].elem, list_[index].next, list_[index].prev);
    fprintf(dotfile, "  node%lu -> node%lu\n", index, list_[index].next);
    fprintf(dotfile, "  node%lu -> node%lu\n", list_[index].next, index);
    index = list_[index].next;
  } while (index != kHeadTail);

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
  if (hold == nullptr) { return LListError::LINEARIZE_CANT_ALLOC; }

  cap_ = size_ * kLListMul;

  index_t index = kHeadTail;
  index_t storeIndex = 0;
  do {
    hold[storeIndex] = {list_[index].elem, storeIndex + 1, storeIndex - 1};
    index = list_[index].next;
    storeIndex++;
  } while (index != kHeadTail);

  hold[kHeadTail].prev = size_ - 1;
  hold[size_ - 1].next = kHeadTail;

  free(list_);
  list_ = hold;

  return LinkFree();
}

//private----------------------------------------------------------------------

LListError LList::OutOfRangeCheck(index_t ind, LListError error) {
  if (ind >= cap_) { return error; }
  if ((list_[ind].elem == kFreeTrashValue)
      && (list_[ind].prev == kFreeTrashRef)) {
    return error;
  }

  return LListError::SUCCESS;
}

LListError LList::Recalloc() {
  LLNode* hold = list_;
  list_ = (LLNode*)realloc(list_, cap_ * sizeof(LLNode));
  if (list_ == nullptr) {
    list_ = hold;
    hold = nullptr;

    return LListError::RECALLOC_CANT_ALLOC;
  }

  memset(list_ + size_, 0, (cap_ - size_) * sizeof(LLNode));

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
  list_[cap_ - 1].next = cap_ - 1;

  return LListError::SUCCESS;
}
