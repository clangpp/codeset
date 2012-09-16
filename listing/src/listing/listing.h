// listing.h
#ifndef LISTING_H_
#define LISTING_H_

namespace listing {

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

}  // namespace listing

#include "listing-inl.h"

#endif  // LISTING_H_
