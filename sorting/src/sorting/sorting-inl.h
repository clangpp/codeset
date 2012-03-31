// sorting-inl.h
#ifndef SORTING_INL_H_
#define SORTING_INL_H_

#include "sorting.h"

#include <algorithm>
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

template <typename ForwardIterator, typename BinaryPredicate>
void selection_sort(ForwardIterator first,
        ForwardIterator last, BinaryPredicate pred) {
    for (ForwardIterator curr=first; curr!=last; ++curr) {
        ForwardIterator best=curr, cursor=best;
        for (++cursor; cursor!=last; ++cursor) {
            if (pred(*cursor, *best)) best = cursor;
        }
        if (best!=curr)
            std::iter_swap(curr, best);
    }
}

template <typename ForwardIterator>
void selection_sort(ForwardIterator first, ForwardIterator last) {
    typedef typename std::iterator_traits<
        ForwardIterator>::value_type value_type;
    selection_sort(first, last, std::less<value_type>());
}

}  // namespace sorting

#endif  // SORTING_INL_H_
