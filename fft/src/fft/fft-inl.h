// fft-inl.h
#ifndef FFT_INL_H_
#define FFT_INL_H_

#include "fft-inl.h"

#include <cmath>
#include <stdexcept>

namespace fft {

template <typename FloatT>
void Filter<FloatT>::establish(size_type seq_len) {

    // get minimum 2 power not less than seq_len
    size_type len_pow2 = 2, pow = 1;
    for (; len_pow2<seq_len && len_pow2!=0; len_pow2<<=1, ++pow) {}
    if (0==len_pow2) {
        throw std::runtime_error(
                "Filter::establish(size_type): seq_len too large");
    }

	// create omega used in fft (half is enough)
    float_type pi = acos((float_type)(-1));
    float_type delta = -2 * pi / len_pow2;
    omega_.resize(len_pow2 >> 1);  // half size is enough
    for (size_type i=0; i<omega_.size(); ++i)
        omega_[i] = value_type(cos(i*delta), sin(i*delta));

    // re-order indices of sequence
    index_.assign(len_pow2, 0);
    for (size_type i=0; i<pow; ++i) {
        size_type add= 1<<i;
        size_type check = 1<<(pow-1-i);  // 2^(pow-1-i) == (len_pow2/2)*(1/2)^i
        for (size_type j=0; j<len_pow2; ++j) {
            if ((j/check) & 0x01)  // odd number
                index_[j] += add;
        }
    }
}

template <typename FloatT>
template <template <class T,
         class A=std::allocator<T> > class RandomAccessContainer>
RandomAccessContainer<typename Filter<FloatT>::value_type>&
Filter<FloatT>::filter(RandomAccessContainer<value_type>& seq) const {
    size_type len = point_count();
    seq.resize(len);  // fill extra zeros to base 2 sequence

	// butterfly operation
    RandomAccessContainer<value_type> work(len);
    for (size_type i=0; i<len; ++i)
        work[i] = seq[index_[i]];
    for (size_type group_dist=2; group_dist<=len; group_dist<<=1) {  // group distance
        size_type group_count = len/group_dist;  // butterfly count of current circle
        size_type half_dist = group_dist>>1;
        for (size_type group_index=0; group_index<group_count; ++group_index) {  // for each group
            size_type base_index = group_index * group_dist;
            for (size_type i=0; i<half_dist; ++i)  // right half sequence multiplied by omega
                work[base_index+half_dist+i] *= omega_[group_count*i];
            for (size_type i=0; i<half_dist; ++i)  // left half result: seq[k]=work[k]+work[k+len/2]
                seq[base_index+i] = work[base_index+i] + work[base_index+half_dist+i];
            for (size_type i=half_dist; i<group_dist; ++i)  // right half result: seq[k+len/2]=work[k]-work[k+len/2]
                seq[base_index+i] = work[base_index-half_dist+i] - work[base_index+i];
        }
        if (group_dist < len) work.swap(seq);  // refill work vector for next circle
    }

    return seq;
}

}  // namespace fft

#endif  // FFT_INL_H_
