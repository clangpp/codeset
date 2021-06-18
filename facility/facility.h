#ifndef FACILITY_H_
#define FACILITY_H_

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace facility {
namespace internal {

// specialization for std::stringstream extractor (>>) to vector<T>
template <typename T>
std::stringstream& operator>>(std::stringstream& ss, std::vector<T>& vec) {
  vec.clear();
  T value = T();
  while (ss >> value) vec.push_back(value);
  return ss;
}

// generic conversion from std::stringstream to ToType
template <typename ToType>
ToType to(std::stringstream& ss) {
  ToType result = ToType();
  ss >> result;
  return result;
}

// specialization for conversion from std::stringstream to std::string
template <>
inline std::string to<std::string>(std::stringstream& ss) {
  return ss.str();
}

}  // namespace internal

// usage: int a = to<int>("356");  // a: 356
template <typename ToType, typename FromType>
ToType to(const FromType& from) {
  std::stringstream ss;
  ss << from;
  return internal::to<ToType>(ss);
}

// usage: string s = trim_left("  abc");  // s: "abc"
std::string& trim_left(std::string& str);

// usage: string s = trim_right("abc  ");  // s: "abc"
std::string& trim_right(std::string& str);

// usage: string s = trim("  abc  ");  // s: "abc"
std::string& trim(std::string& str);

// usage:
// int a[35];
// // many codes...
// size_t len = array_length(a);  // len: 35
template <typename T, std::size_t N>
std::size_t array_length(T (&)[N]) {
  return N;
}

// simple conversion between string and wstring, only matters blank bytes
// /wide/ denotes the character width used in wchar_t
std::string& copy_little_endian(const std::wstring& src, std::size_t wide,
                                std::string& dest);

std::wstring& copy_little_endian(const std::string& src, std::size_t wide,
                                 std::wstring& dest);

std::string& copy_big_endian(const std::wstring& src, std::size_t wide,
                             std::string& dest);

std::wstring& copy_big_endian(const std::string& src, std::size_t wide,
                              std::wstring& dest);

template <typename InputIterator>
std::string& copy_little_endian(InputIterator wchar_beg,
                                InputIterator wchar_end, std::size_t wide,
                                std::string& dest) {
  typedef typename std::iterator_traits<InputIterator>::value_type wchar_type;
  wide = std::min(std::max(wide, std::size_t(1)), sizeof(wchar_type));
  dest.clear();
  for (; wchar_beg != wchar_end; ++wchar_beg) {
    wchar_type wch = *wchar_beg;
    bool is_ascii = (wch ^ (wch & 0x7f)) == 0;
    if (is_ascii) {
      dest.push_back(static_cast<char>(wch));
    } else {
      for (std::size_t j = 0; j < wide; ++j) dest.push_back(wch >> (j << 3));
    }
  }
  return dest;
}

template <typename InputIterator>
std::wstring& copy_little_endian(InputIterator char_beg, InputIterator char_end,
                                 std::size_t wide, std::wstring& dest) {
  wide = std::min(std::max(wide, std::size_t(1)), sizeof(wchar_t));
  dest.clear();
  for (std::size_t j = 0; char_beg != char_end; ++char_beg) {
    unsigned char uch = static_cast<unsigned char>(*char_beg);
    if (0 != j) {  // in non-ascii code
      dest[dest.size() - 1] |= (uch << (j << 3));
      j = (j + 1) % wide;
    } else if ((uch & 0x80) == 0) {  // ascii code
      dest.push_back(uch);
    } else {  // (0==j) new non-ascii code
      dest.push_back(uch);
      j = (j + 1) % wide;
    }
  }
  return dest;
}

template <typename InputIterator>
std::string& copy_big_endian(InputIterator wchar_beg, InputIterator wchar_end,
                             std::size_t wide, std::string& dest) {
  typedef typename std::iterator_traits<InputIterator>::value_type wchar_type;
  wide = std::min(std::max(wide, std::size_t(1)), sizeof(wchar_type));
  dest.clear();
  for (; wchar_beg != wchar_end; ++wchar_beg) {
    wchar_type wch = *wchar_beg;
    bool is_ascii = (wch ^ (wch & 0x7f)) == 0;
    if (is_ascii) {
      dest.push_back(static_cast<char>(wch));
    } else {
      for (std::size_t j = 0; j < wide; ++j)
        dest.push_back(wch >> ((wide - 1 - j) << 3));
    }
  }
  return dest;
}

template <typename InputIterator>
std::wstring& copy_big_endian(InputIterator char_beg, InputIterator char_end,
                              std::size_t wide, std::wstring& dest) {
  wide = std::min(std::max(wide, std::size_t(1)), sizeof(wchar_t));
  dest.clear();
  for (std::size_t j = 0; char_beg != char_end; ++char_beg) {
    unsigned char uch = static_cast<unsigned char>(*char_beg);
    if (0 != j) {  // in non-ascii code
      dest[dest.size() - 1] |= (uch << ((wide - 1 - j) << 3));
      j = (j + 1) % wide;
    } else if ((uch & 0x80) == 0) {  // ascii code
      dest.push_back(uch);
    } else {  // (0==j) new non-ascii code
      dest.push_back(uch << ((wide - 1) << 3));
      j = (j + 1) % wide;
    }
  }
  return dest;
}

// split string by token
// usage: vector<string> vs = split("abc def  ghi ");  // vs: "abc" "def" ""
// "ghi" ""
std::vector<std::string> split(const std::string& str,
                               const std::string& token = " ");

}  // namespace facility

#endif  // FACILITY_H_
