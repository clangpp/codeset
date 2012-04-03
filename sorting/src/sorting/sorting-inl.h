// sorting-inl.h
#ifndef SORTING_INL_H_
#define SORTING_INL_H_

#include "sorting.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <vector>

#include "../tree/tree.h"

namespace sorting {

template <typename ForwardIterator, typename BinaryPredicate>
void bubble_sort(ForwardIterator first,
        ForwardIterator last, BinaryPredicate pred) {
    while (last!=first) {  // unsorted range narrow and narrow
        ForwardIterator left(first), right(first);
        for (++right; right!=last; ++left, ++right) {  // one loop bubble
            if (pred(*right, *left)) std::iter_swap(left, right);
        }
        last = left;  // --last
    }
}

template <typename ForwardIterator>
inline void bubble_sort(ForwardIterator first, ForwardIterator last) {
    typedef typename std::iterator_traits<
        ForwardIterator>::value_type value_type;
    bubble_sort(first, last, std::less<value_type>());
}

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
        if (best!=curr) std::iter_swap(curr, best);
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
    std::vector<difference_type> gaps(1, 1);  // first gap must be 1
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

template <typename RandomAccessIterator, typename BinaryPredicate>
void heap_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred) {
    tree::heap::make(first, last, pred);
    for (; last!=first; --last)
        tree::heap::pop(first, last, pred);
}

template <typename RandomAccessIterator>
void heap_sort(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::value_type value_type;
    heap_sort(first, last, std::less<value_type>());
}

template <typename InputIterator1, typename InputIterator2,
         typename OutputIterator, typename BinaryPredicate>
OutputIterator merge(
        InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
        InputIterator2 last2, OutputIterator result, BinaryPredicate pred) {
    while (first1!=last1 && first2!=last2) {  // main merge process
        if (pred(*first1, *first2)) {
            *(result++) = *(first1++);
        } else {
            *(result++) = *(first2++);
        }
    }
    while (first1!=last1) *(result++) = *(first1++);  // copy rest of left part
    while (first2!=last2) *(result++) = *(first2++);  // copy rest of right part
    return result;
}

template <typename InputIterator1,
         typename InputIterator2, typename OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    typedef typename std::iterator_traits<
        InputIterator1>::value_type value_type;
    return sorting::merge(first1, last1,
            first2, last2, result, std::less<value_type>());
}

template <typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void merge_sort(RandomAccessIterator1 first, RandomAccessIterator1 last,
        RandomAccessIterator2 buffer, BinaryPredicate pred) {
    std::copy(first, last, buffer);  // make mirror sequence
    internal::merge_sort(buffer, buffer+(last-first), first, pred);
}

template <typename RandomAccessIterator1, typename RandomAccessIterator2>
void merge_sort(RandomAccessIterator1 first,
        RandomAccessIterator1 last, RandomAccessIterator2 buffer) {
    typedef typename std::iterator_traits<
        RandomAccessIterator1>::value_type value_type;
    merge_sort(first, last, buffer, std::less<value_type>());
}

namespace internal {

// pre-condition: [first, last) and [result, result+(last-first))
//  contains same subsequence groups
template <typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void merge_sort(RandomAccessIterator1 first, RandomAccessIterator1 last,
        RandomAccessIterator2 result, BinaryPredicate pred) {
    if (last-first <= 1) return;
    typedef typename std::iterator_traits<
        RandomAccessIterator1>::difference_type difference_type;
    difference_type len = last-first, mid_pos = len/2;
    internal::merge_sort(result, result+mid_pos, first, pred);
    internal::merge_sort(result+mid_pos, result+len, first+mid_pos, pred);
    sorting::merge(first, first+mid_pos, first+mid_pos, last, result, pred);
}

}  // namespace internal

template <typename BidirectionalIterator, typename Predicate>
BidirectionalIterator partition(BidirectionalIterator first,
        BidirectionalIterator last, Predicate pred) {
    while(first!=last) {
        while (first!=last && pred(*first)) ++first;  // left-to-right scan
        if (first!=last) --last;  // right sentry move to test element
        while (first!=last && !pred(*last)) --last;  // right-to-left scan
        if (first!=last) {
            std::iter_swap(first, last);  // swap to right place
            ++first;  // left sentry move to test element
        }
    }
    return first;
}

template <typename RandomAccessIterator, typename BinaryPredicate>
void quick_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred) {
    if (last-first<=1) return;

    // small amount array use insertion_sort
    if (last-first <= internal::QUICK_SORT_CUTOFF_RANGE) {
        insertion_sort(first, last, pred);
        return;
    }

    // quick sort algorithm (pre-condition: last-first>=2)
    RandomAccessIterator center = internal::median3(first, last, pred);
    center = sorting::partition(first+1, last-1, std::bind2nd(pred,*center));
    sorting::quick_sort(first, center, pred);
    sorting::quick_sort(center, last, pred);
}

template <typename RandomAccessIterator>
void quick_sort(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::value_type value_type;
    quick_sort(first, last, std::less<value_type>());
}

namespace internal {

// median3 algorithm
// pre-condition: last-first>=2
// post-condition: !(*center,*first) && !(*(last-1),*center)
template <typename RandomAccessIterator, typename BinaryPredicate>
RandomAccessIterator median3(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred) {
    RandomAccessIterator center = first + (last-first)/2;
    if (pred(*center, *first)) std::iter_swap(first, center);
    if (pred(*(last-1), *first)) std::iter_swap(first, last-1);
    if (pred(*(last-1), *center)) std::iter_swap(center, last-1);
    return center;
}

}  // namespace internal

namespace internal {

template <typename BinaryPredicate, typename Iterator>
class IndirectCompare:
    public std::binary_function<Iterator, Iterator, bool> {
public:
    IndirectCompare(BinaryPredicate pred): pred_(pred) {}
    bool operator () (const Iterator& lhs, const Iterator& rhs) const {
        return pred_(*lhs, *rhs);
    }
private:
    BinaryPredicate pred_;
};

}  // namespace internal

template <typename RandomAccessIterator, typename BinaryPredicate>
void indirect_sort(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred) {

    // make iterator vector
    std::vector<RandomAccessIterator> iters;
    for (RandomAccessIterator iter=first; iter!=last; ++iter)
        iters.push_back(iter);

    // do quick sort on iterator vector
    quick_sort(iters.begin(), iters.end(), internal::
            IndirectCompare<BinaryPredicate, RandomAccessIterator>(pred));

    // shuffle items in place
    typedef std::iterator_traits<RandomAccessIterator> traits;
    typedef typename traits::value_type value_type;
    typedef typename traits::difference_type difference_type;
    for (difference_type i=0, len=iters.size(); i<len; ++i) {
        if (iters[i] == first+i) continue;  // matched, no need to move

        value_type value = first[i];
        difference_type j = 0, next_j = 0;
        for (j=i; iters[j]!=first+i; j=next_j) {  // circle move
            next_j = iters[j] - first;
            first[j] = *iters[j];
            iters[j] = first+j;
        }
        first[j] = value;
        iters[j] = first+j;
    }
}

template <typename RandomAccessIterator>
void indirect_sort(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::value_type value_type;
    indirect_sort(first, last, std::less<value_type>());
}

template <typename ForwardIterator>
ForwardIterator bucket_sort(
        ForwardIterator first, ForwardIterator last, bool ascending) {
    typedef std::vector<bool>::difference_type difference_type;
    typedef std::vector<bool>::size_type size_type;
    std::vector<bool> buckets;
    for (ForwardIterator curr=first; curr!=last; ++curr) {
        if (*curr<0 || difference_type(buckets.max_size()>>1)<=*curr)
            throw std::runtime_error("number negative or too large");
        if (difference_type(buckets.size()) <= *curr)
            buckets.resize(*curr+1);
        buckets[*curr] = true;
    }
    if (ascending) {
        for (difference_type i=0, len=buckets.size(); i<len; ++i) {
            if (buckets[i]) *(first++) = i;
        }
    } else {
        for (difference_type len=buckets.size(), i=len-1; i>=0; --i) {
            if (buckets[i]) *(first++) = i;
        }
    }
    return first;
}

template <typename RandomAccessIterator,
         typename Distance, typename BinaryPredicate>
RandomAccessIterator quick_select(RandomAccessIterator first,
        RandomAccessIterator last, Distance n, BinaryPredicate pred) {
    if (n<0 || last-first<=n) return last;  // illegal input
    if (last-first==1 && n==0) return first;  // return condition

    // small amount array use insertion_sort
    if (last-first<=internal::QUICK_SORT_CUTOFF_RANGE) {
        insertion_sort(first, last, pred);
        return first+n;
    }

    // recursive quick_select (pre-condition: last-first>=2)
    RandomAccessIterator center = internal::median3(first, last, pred);
    center = sorting::partition(first, last, std::bind2nd(pred,*center));
    if (n < center-first) {
        return quick_select(first, center, n, pred);
    } else {
        return quick_select(center, last, n-(center-first), pred);
    }
}

template <typename RandomAccessIterator, typename Distance>
RandomAccessIterator quick_select(
        RandomAccessIterator first, RandomAccessIterator last, Distance n) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::value_type value_type;
    return quick_select(first, last, n, std::less<value_type>());
}

template <typename RandomAccessIterator,
         typename Distance, typename BinaryPredicate>
RandomAccessIterator heap_select(RandomAccessIterator first,
        RandomAccessIterator last, Distance n, BinaryPredicate pred) {
    if (n<0 || last-first<=n) return last;  // illegal input
    if (last-first==1 && n==0) return first;  // return condition
    tree::heap::make(first, first+n+1, pred);  // make "max" heap of size n+1

    for (RandomAccessIterator curr=first+n+1; curr!=last; ++curr) {
        if (pred(*curr, *first)) {  // maintain fixed size "max" heap
            std::iter_swap(first, curr);
            tree::heap::percolate_down(first, first+n+1, 0, pred);
        }
    }
    return first;
}

template <typename RandomAccessIterator, typename Distance>
RandomAccessIterator heap_select(
        RandomAccessIterator first, RandomAccessIterator last, Distance n) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::value_type value_type;
    return heap_select(first, last, n, std::less<value_type>());
}

}  // namespace sorting

#endif  // SORTING_INL_H_
