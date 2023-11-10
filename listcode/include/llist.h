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

/// @brief errors that list can throw
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

/// @brief list node
struct LLNode {
  elem_t elem;  ///< node data
  index_t next; ///< index of next node
  index_t prev; ///< index of previous node
};

/// @brief double linked list
struct LList {
 public:

  /**
   * @brief list constructor
   *
   * @param initCap (optional) init capacity
   *
   * @return list error
  */
  LListError Ctor(size_t initCap = kLListMinAlloc);

  /**
   * @brief list destructor
  */
  void Dtor();

  /**
   * @brief throw error to stdout
   *
   * @param error error to throw
  */
  void ThrowError(LListError error);

  /**
   * @brief generates dump in .dot file format
  */
  void DotDump();

  /**
   * @brief checks if the list has no nodes
   *
   * @return true if the list is empty, false otherwise
  */
  bool IsEmpty();

  /**
   * @brief returns number of nodes in list
   *
   * @return number of nodes in list
  */
  size_t Size();

  /**
   * @brief returns
   *
   * @return maximum number of nodes in list
  */
  size_t Capacity();

  /**
   * @brief returns the index of the first node in the list
   *
   * @param indRet return index
   *
   * @return list error
  */
  LListError Front(index_t* indRet);

  /**
   * @brief returns the index of the last node in the list
   *
   * @param indRet return index
   *
   * @return list error
  */
  LListError Back(index_t* indRet);

  /**
   * @brief prepends the given node to the beginning of the list
   *
   * @param elem node to prepand
   *
   * @return list error
  */
  LListError PushFront(elem_t elem);

  /**
   * @brief appends the given node to the end of the list
   *
   * @param elem node to append
   *
   * @return list error
  */
  LListError PushBack(elem_t elem);

  /**
   * @brief removes the first node of the list
   *
   * @return list error
  */
  LListError PopFront();

  /**
   * @brief removes the last node of the list
   *
   * @return list error
  */
  LListError PopBack();

  /**
   * @brief insers element after given index
   *
   * @param ind index of the node before which the element will be inserted
   * @param elem element of the node to insert
   *
   * @return list error
  */
  LListError InsertAfter(index_t ind, elem_t elem);

  /**
   * @brief removes the node at the given index
   *
   * @param ind index of the node to remove
   *
   * @return list error
  */
  LListError RemoveAt(index_t ind);

  /**
   * @brief linearize the list in the memory
   *
   * @return list error
  */
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
