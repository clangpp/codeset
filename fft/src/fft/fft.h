// fft.h
#ifndef FFT_H_
#define FFT_H_

#include <complex>
#include <vector>

namespace fft {

// class Filter: establish fft machine
template <typename FloatT>
class Filter {
public:
    typedef FloatT float_type;
    typedef std::size_t size_type;
    typedef std::complex<float_type> value_type;

public:
    void establish(size_type seq_len);

    size_type point_count() const { return omega_.size() << 1; }

    // RandomAccessContainer can be vector or deque
    template <template <class T,
             class A=std::allocator<T> > class RandomAccessContainer>
    RandomAccessContainer<value_type>& filter(
            RandomAccessContainer<value_type>& seq) const;

private:
    std::vector<value_type> omega_;
    std::vector<size_type> index_;
};

}  // namespace fft

#include "fft-inl.h"

#endif  // FFT_H_
