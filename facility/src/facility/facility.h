// facility.h
// last modified: 2011.12.26
#ifndef FACILITY_H_
#define FACILITY_H_

#include <cstddef>
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

// simple conversion between string and wstring, only matters blank bytes
// /wide/ denotes the character width used in wchar_t
std::string& copy_little_endian(
        const std::wstring& src, std::size_t wide, std::string& dest);

std::wstring& copy_little_endian(
        const std::string& src, std::size_t wide, std::wstring& dest);

std::string& copy_big_endian(
        const std::wstring& src, std::size_t wide, std::string& dest);

std::wstring& copy_big_endian(
        const std::string& src, std::size_t wide, std::wstring& dest);

template <typename InputIterator>
std::string& copy_little_endian(
        InputIterator wchar_beg, InputIterator wchar_end, 
        std::size_t wide, std::string& dest);

template <typename InputIterator>
std::wstring& copy_little_endian(
        InputIterator char_beg, InputIterator char_end, 
        std::size_t wide, std::wstring& dest);

template <typename InputIterator>
std::string& copy_big_endian(
        InputIterator wchar_beg, InputIterator wchar_end, 
        std::size_t wide, std::string& dest);

template <typename InputIterator>
std::wstring& copy_big_endian(
        InputIterator char_beg, InputIterator char_end, 
        std::size_t wide, std::wstring& dest);

}  // namespace facility

// inline implemention
#include "facility-inl.h"

#endif  // FACILITY_H_
