// bignumber.h
#ifndef BIGNUMBER_H_
#define BIGNUMBER_H_

#include <cstddef>
#include <vector>

namespace bignumber {

// pre-condition: [low_beg1, high_end1) [low_beg2, high_end2) are
//  big numbers with digits from low to high
// pre-condition: result has enough space to contain the final result
// remarks: calculate product of big numbers [low_beg1, high_end1) and
//  [low_beg2, high_end2), add the product to big number result.
template <typename ForwardIterator1, typename ForwardIterator2,
         typename DigitT, typename RandomAccessIterator>
RandomAccessIterator multiplies(
        ForwardIterator1 low_beg1, ForwardIterator1 high_end1,
        ForwardIterator2 low_beg2, ForwardIterator2 high_end2,
        const DigitT& round, RandomAccessIterator result);

}  // bignumber

#include "bignumber-inl.h"

#endif  // BIGNUMBER_H_
