// bignumber-inl.h
#ifndef BIGNUMBER_INL_H_
#define BIGNUMBER_INL_H_

#include "bignumber.h"

#include <algorithm>

namespace bignumber {

template <typename ForwardIterator1, typename ForwardIterator2,
         typename DigitT, typename RandomAccessIterator>
RandomAccessIterator multiplies(
        ForwardIterator1 low_beg1, ForwardIterator1 high_end1,
        ForwardIterator2 low_beg2, ForwardIterator2 high_end2,
        const DigitT& round, RandomAccessIterator result) {
    typedef typename std::iterator_traits<
        RandomAccessIterator>::difference_type difference_type;
    typedef DigitT digit_type;
    digit_type carry=0, value=0;
    ForwardIterator1 iter1 = low_beg1;
    ForwardIterator2 iter2 = low_beg2;
    difference_type pos1(0), pos2(0), pos_end(0);
    for (iter1=low_beg1, pos1=0; iter1!=high_end1; ++iter1, ++pos1) {
        carry = 0;
        for (iter2=low_beg2, pos2=0; iter2!=high_end2; ++iter2, ++pos2) {
            if (pos1+pos2 >= pos_end) pos_end = pos1 + pos2 + 1;
            value = (*iter1) * (*iter2) + carry + result[pos1+pos2];
            carry = value / round;
            result[pos1+pos2] = value % round;
        }
        for (; carry>0; ++pos2) {
            if (pos1+pos2 >= pos_end) pos_end = pos1 + pos2 + 1;
            value = carry + result[pos1+pos2];
            carry = value / round;
            result[pos1+pos2] = value % round;
        }
    }
    return result + pos_end;
}

}  // namespace bignumber

#endif  // BIGNUMBER_INL_H_
