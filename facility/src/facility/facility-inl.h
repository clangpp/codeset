// facility-inl.h
#ifndef FACILITY_INL_H_
#define FACILITY_INL_H_

#include "facility.h"
#include <sstream>
#include <string>
#include <vector>

namespace facility {
namespace internal {

// specialization for std::stringstream extractor (>>) to vector<T> 
template <typename T>
inline std::stringstream& operator >> (
		std::stringstream& ss, std::vector<T>& vec) {
	vec.clear();
	T value = T();
	while (ss >> value)
		vec.push_back(value);
	return ss;
}

// generic conversion from std::stringstream to ToType
template <typename ToType>
inline ToType to(std::stringstream& ss) {
	ToType result = ToType();
	ss >> result;
	return result;
}

// specialization for conversion from std::stringstream to std::string
template <>
inline std::string to<std::string>(std::stringstream& ss) {
	return ss.str();
}

}  // namespace facility::internal


// generic conversion API
template <typename ToType, typename FromType>
inline ToType to(const FromType& from) {
	std::stringstream ss;
	ss << from;
	return internal::to<ToType>(ss);
}

inline std::string& trim(std::string& str) {
	return trim_left(trim_right(str));
}

template <typename T, std::size_t N>
inline std::size_t array_length(T (&)[N]) { return N; }

inline std::string& copy_little_endian(
        const std::wstring& src, std::size_t wide, std::string& dest) {
    return copy_little_endian(src.begin(), src.end(), wide, dest);
}

inline std::wstring& copy_little_endian(
        const std::string& src, std::size_t wide, std::wstring& dest) {
    return copy_little_endian(src.begin(), src.end(), wide, dest);
}

inline std::string& copy_big_endian(
        const std::wstring& src, std::size_t wide, std::string& dest) {
    return copy_big_endian(src.begin(), src.end(), wide, dest);
}

inline std::wstring& copy_big_endian(
        const std::string& src, std::size_t wide, std::wstring& dest) {
    return copy_big_endian(src.begin(), src.end(), wide, dest);
}

template <typename InputIterator>
std::string& copy_little_endian(
        InputIterator wchar_beg, InputIterator wchar_end, 
        std::size_t wide, std::string& dest) {
    typedef typename
        std::iterator_traits<InputIterator>::value_type wchar_type;
    wide = std::min(std::max(wide,std::size_t(1)), sizeof(wchar_type));
    dest.clear();
    for (; wchar_beg!=wchar_end; ++wchar_beg) {
        wchar_type wch = *wchar_beg;
        bool is_ascii = (wch^(wch&0x7f))==0;
        if (is_ascii) {
            dest.push_back(static_cast<char>(wch));
        } else {
            for (std::size_t j=0; j<wide; ++j)
                dest.push_back(wch >> (j<<3));
        }
    }
    return dest;
}

template <typename InputIterator>
std::wstring& copy_little_endian(
        InputIterator char_beg, InputIterator char_end, 
        std::size_t wide, std::wstring& dest) {
    wide = std::min(std::max(wide,std::size_t(1)), sizeof(wchar_t));
    dest.clear();
    for (std::size_t j=0; char_beg!=char_end; ++char_beg) {
        unsigned char uch = static_cast<unsigned char>(*char_beg);
        if (0 != j) {  // in non-ascii code
            dest[dest.size()-1] |= (uch << (j<<3));
            j = (j+1) % wide;
        } else if ((uch & 0x80) == 0) {  // ascii code
            dest.push_back(uch);
        } else {  // (0==j) new non-ascii code
            dest.push_back(uch);
            j = (j+1) % wide;
        }
    }
    return dest;
}

template <typename InputIterator>
std::string& copy_big_endian(
        InputIterator wchar_beg, InputIterator wchar_end, 
        std::size_t wide, std::string& dest) {
    typedef typename
        std::iterator_traits<InputIterator>::value_type wchar_type;
    wide = std::min(std::max(wide,std::size_t(1)), sizeof(wchar_type));
    dest.clear();
    for (; wchar_beg!=wchar_end; ++wchar_beg) {
        wchar_type wch = *wchar_beg;
        bool is_ascii = (wch^(wch&0x7f))==0;
        if (is_ascii) {
            dest.push_back(static_cast<char>(wch));
        } else {
            for (std::size_t j=0; j<wide; ++j)
                dest.push_back(wch >> ((wide-1-j)<<3));
        }
    }
    return dest;
}

template <typename InputIterator>
std::wstring& copy_big_endian(
        InputIterator char_beg, InputIterator char_end, 
        std::size_t wide, std::wstring& dest) {
    wide = std::min(std::max(wide,std::size_t(1)), sizeof(wchar_t));
    dest.clear();
    for (std::size_t j=0; char_beg!=char_end; ++char_beg) {
        unsigned char uch = static_cast<unsigned char>(*char_beg);
        if (0 != j) {  // in non-ascii code
            dest[dest.size()-1] |= (uch << ((wide-1-j)<<3));
            j = (j+1) % wide;
        } else if ((uch & 0x80) == 0) {  // ascii code
            dest.push_back(uch);
        } else {  // (0==j) new non-ascii code
            dest.push_back(uch << ((wide-1)<<3));
            j = (j+1) % wide;
        }
    }
    return dest;
}

}  // namespace facility

#endif  // FACILITY_INL_H_

