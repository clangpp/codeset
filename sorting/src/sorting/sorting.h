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

}  // namespace sorting

#include "sorting-inl.h"

#endif  // SORTING_H_
