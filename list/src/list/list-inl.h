// list-inl.h
#ifndef LIST_INL_H_
#define LIST_INL_H_

#include "list.h"

#include <cstddef>

namespace list {

template <typename NodeT>
NodeT* reverse(NodeT* head) {
    NodeT* prev = NULL;
    NodeT* curr = head;
    while (NULL!=curr) {
        NodeT* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

template <typename NodeT>
NodeT* reverse_recursive(NodeT* head) {
    if (NULL==head || NULL==head->next) return head;  // stop condition
    NodeT* new_head = reverse_recursive(head->next);  // reverse sub-list
    NodeT* new_tail = head->next;  // new tail of sub-list
    new_tail->next = head;  // reverse link
    head->next = NULL;  // clear field
    return new_head;
}

}  // namespace list

#endif  // LIST_INL_H_
