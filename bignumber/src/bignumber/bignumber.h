// bignumber.h
#ifndef BIGNUMBER_H_
#define BIGNUMBER_H_

#include <cstddef>
#include <vector>

namespace bignumber {

template <typename DigitContainer>
void multiplies(
        const DigitContainer& lhs, const DigitContainer& rhs,
        const typename DigitContainer::value_type& round,
        DigitContainer& result);

template <typename ForwardIterator1, typename ForwardIterator2,
         typename DigitT, typename RandomAccessIterator>
RandomAccessIterator multiplies(
        ForwardIterator1 low_beg1, ForwardIterator1 high_end1,
        ForwardIterator2 low_beg2, ForwardIterator2 high_end2,
        const DigitT& round, RandomAccessIterator result);

}  // bignumber

// template <typename DigitT, DigitT Round>
// class BigNumber {
// public:
//     typedef DigitT digit_type;
// 
// public:
//     BigNumber() {}
// 
//     template <typename InputIterator>
//     BigNumber(first, last);  // from high digit to low digit
// 
//     BigNumber& operator *= (const BigNumber& rhs);
// 
// public:
//     static bool valid_digit(const digit_type& value) {
//         return 0<=vaule && value<Round;
//     }
// 
// private:
//     std::vector<digit_type> digits_;
// };

#include "bignumber-inl.h"

#endif  // BIGNUMBER_H_
