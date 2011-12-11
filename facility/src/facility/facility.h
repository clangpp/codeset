// facility.h
#ifndef FACILITY_H_
#define FACILITY_H_

#include <string>

namespace facility {

// usage: int a = to<int>("356");  // a: 356
template <typename ToType, typename FromType>
ToType to(const FromType& from);

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
std::size_t array_length(T (&)[N]);

}  // namespace facility

// inline implemention
#include "facility-inl.h"

#endif  // FACILITY_H_
