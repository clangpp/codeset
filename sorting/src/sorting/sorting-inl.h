// sorting-inl.h
#ifndef SORTING_INL_H_
#define SORTING_INL_H_

#include "sorting.h"

#include <functional>
#include <iterator>

namespace sorting {

template <typename BidirectionalIterator, typename BinaryPredicate>
void insertion_sort(BidirectionalIterator first,
        BidirectionalIterator last, BinaryPredicate pred) {
    if (first==last) return;
    typedef typename std::iterator_traits<
        BidirectionalIterator>::value_type value_type;
    for (BidirectionalIterator curr=first; ++curr!=last;) {
        value_type value = *curr;
        BidirectionalIterator right=curr, left=curr;
        for (--left; pred(value,*left); --left) {
            *(right--) = *left;
            if (left==first) break;  // reach first element
        }
        *right = value;
    }
}

template <typename BidirectionalIterator>
void insertion_sort(
        BidirectionalIterator first, BidirectionalIterator last) {
    typedef typename std::iterator_traits<
        BidirectionalIterator>::value_type value_type;
    insertion_sort(first, last, std::less<value_type>());
}

}  // namespace sorting

#endif  // SORTING_INL_H_
