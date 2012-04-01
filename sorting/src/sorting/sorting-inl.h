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

template <typename RandomAccessIterator, typename BinaryPredicate>
void shell_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::difference_type difference_type;
    vector<difference_type> gaps(1, 1);  // first gap must be 1
    while (gaps.back() < last-first) {
        difference_type gap = gaps.back();
        // gap = 2 * gap;  // Shell's increments
        gap = 2*(gap+1) - 1;  // Hibbard's increments
        if (gap < gaps.back()) break;  // new gap overflow
        gaps.push_back(gap);
    }
    gaps.pop_back();  // last gap must be less than (last-first)
    internal::shell_sort(first, last, gaps.begin(), gaps.end(), pred);
}

template <typename RandomAccessIterator>
void shell_sort(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::value_type value_type;
    shell_sort(first, last, std::less<value_type>());
}

namespace internal {

// pre-condition: (gap_first==gap_last) || (*gap_first==1)
template <typename RandomAccessIterator,
         typename BidirectionalIterator, typename BinaryPredicate>
void shell_sort(RandomAccessIterator first, RandomAccessIterator last,
        BidirectionalIterator gap_first, BidirectionalIterator gap_last,
        BinaryPredicate pred) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::value_type value_type;
    typedef typename std::iterator_traits<
        BidirectionalIterator>::value_type gap_type;
    typedef std::reverse_iterator<BidirectionalIterator> reverse_iterator;

    // iteration for each gap in increment sequence [gap_first, gap_last)
    reverse_iterator gap_rbegin(gap_last), gap_rend(gap_first), gap_riter;
    for (gap_riter=gap_rbegin; gap_riter!=gap_rend; ++gap_riter) {
        const gap_type& gap = *gap_riter;  // current gap

        // insertion sort with gap 'gap'
        for (RandomAccessIterator iter=first+gap; iter<last; ++iter) {
            value_type value = *iter;
            RandomAccessIterator curr = iter;
            for (; curr>=first+gap && pred(value,*(curr-gap)); curr-=gap)
                *curr = *(curr-gap);
            *curr = value;
        }
    }
}

}  // namespace internal

}  // namespace sorting

#endif  // SORTING_INL_H_
