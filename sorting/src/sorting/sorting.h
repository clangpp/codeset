// sorting.h
#ifndef SORTING_H_
#define SORTING_H_

namespace sorting {

template <typename BidirectionalIterator, typename BinaryPredicate>
void insertion_sort(BidirectionalIterator first,
        BidirectionalIterator last, BinaryPredicate pred);

template <typename BidirectionalIterator>
inline void insertion_sort(
        BidirectionalIterator first, BidirectionalIterator last);

template <typename ForwardIterator, typename BinaryPredicate>
void selection_sort(ForwardIterator first,
        ForwardIterator last, BinaryPredicate pred);

template <typename ForwardIterator>
inline void selection_sort(ForwardIterator first, ForwardIterator last);

template <typename RandomAccessIterator, typename BinaryPredicate>
void shell_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

template <typename RandomAccessIterator>
inline void shell_sort(RandomAccessIterator first, RandomAccessIterator last);

namespace internal {

template <typename RandomAccessIterator,
         typename BidirectionalIterator, typename BinaryPredicate>
void shell_sort(RandomAccessIterator first, RandomAccessIterator last,
        BidirectionalIterator gap_first, BidirectionalIterator gap_last,
        BinaryPredicate pred);

}  // namespace internal

template <typename RandomAccessIterator, typename BinaryPredicate>
void heap_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

template <typename RandomAccessIterator>
inline void heap_sort(RandomAccessIterator first, RandomAccessIterator last);

// work just like std::merge(), I write it to practice merge algorithm
// pre-condition: both [first, center) and [center, last) are sorted
template <typename InputIterator1, typename InputIterator2,
         typename OutputIterator, typename BinaryPredicate>
OutputIterator merge(
        InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
        InputIterator2 last2, OutputIterator result, BinaryPredicate pred);

template <typename InputIterator1,
         typename InputIterator2, typename OutputIterator>
inline OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2, OutputIterator result);

template <typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void merge_sort(RandomAccessIterator1 first, RandomAccessIterator1 last,
        RandomAccessIterator2 buffer, BinaryPredicate pred);

template <typename RandomAccessIterator1, typename RandomAccessIterator2>
inline void merge_sort(RandomAccessIterator1 first,
        RandomAccessIterator1 last, RandomAccessIterator2 buffer);

namespace internal {

template <typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void merge_sort(RandomAccessIterator1 first, RandomAccessIterator1 last,
        RandomAccessIterator2 result, BinaryPredicate pred);

}  // namespace internal

template <typename BidirectionalIterator, typename Predicate>
BidirectionalIterator partition(BidirectionalIterator first,
        BidirectionalIterator last, Predicate pred);

template <typename RandomAccessIterator, typename BinaryPredicate>
void quick_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

template <typename RandomAccessIterator>
inline void quick_sort(RandomAccessIterator first, RandomAccessIterator last);

namespace internal {

enum { QUICK_SORT_CUTOFF_RANGE = 10 };

template <typename RandomAccessIterator, typename BinaryPredicate>
inline RandomAccessIterator median3(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

}  // namespace internal

template <typename RandomAccessIterator, typename BinaryPredicate>
void indirect_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

template <typename RandomAccessIterator>
inline void indirect_sort(
        RandomAccessIterator first, RandomAccessIterator last);

// parameter: n is 0 based, first element has n 0
// pre-condition: 0<=n && n<last-first  (if not fit, return last)
// returns: iterator at index n in sorted( [first, last) )
template <typename RandomAccessIterator,
         typename Distance, typename BinaryPredicate>
RandomAccessIterator quick_select(RandomAccessIterator first,
        RandomAccessIterator last, Distance n, BinaryPredicate pred);

template <typename RandomAccessIterator, typename Distance>
inline RandomAccessIterator quick_select(
        RandomAccessIterator first, RandomAccessIterator last, Distance n);

template <typename RandomAccessIterator,
         typename Distance, typename BinaryPredicate>
RandomAccessIterator heap_select(RandomAccessIterator first,
        RandomAccessIterator last, Distance n, BinaryPredicate pred);

template <typename RandomAccessIterator, typename Distance>
inline RandomAccessIterator heap_select(
        RandomAccessIterator first, RandomAccessIterator last, Distance n);

}  // namespace sorting

#include "sorting-inl.h"

#endif  // SORTING_H_
