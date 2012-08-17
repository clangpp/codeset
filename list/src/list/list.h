// list.h
#ifndef LIST_H_
#define LIST_H_

namespace list {

/***************************************
concept NodeT {
    T value;
    NodeT * next;
};
***************************************/

template <typename NodeT>
NodeT* reverse(NodeT* head);

template <typename NodeT>
NodeT* reverse_recursive(NodeT* head);

}  // namespace list

#include "list-inl.h"

#endif  // LIST_H_
