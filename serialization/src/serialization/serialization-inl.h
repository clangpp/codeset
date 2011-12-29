// serialization-inl.h
#ifndef SERIALIZATION_INL_H_
#define SERIALIZATION_INL_H_

#include "serialization.h"
#include <string>

namespace serialization {
	
// std::pair serialization
// " ( first second ) "
// =============================================================================
template <typename CharT, typename Traits, typename T1, typename T2>
std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, std::pair<T1, T2>& val) {
	std::string ignored;
	std::getline(is, ignored, '(');
	is >> val.first >> val.second;
	std::getline(is, ignored, ')');
	return is;
}

template <typename CharT, typename Traits, typename T1, typename T2>
inline std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, const std::pair<T1, T2>& val) {
	return os << " ( " << val.first << " " << val.second << " ) ";
}

	
// Triad serialization
// " ( first second third ) "
// =============================================================================
template <typename CharT, typename Traits, typename T1, typename T2, typename T3>
inline std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, Triad<T1, T2, T3>& val) {
	std::string ignored;
	std::getline(is, ignored, '(');
	is >> val.first >> val.second >> val.third;
	std::getline(is, ignored, ')');
	return is;
}

template <typename CharT, typename Traits, typename T1, typename T2, typename T3>
inline std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, Triad<T1, T2, T3>& val) {
	return os << " ( " << val.first << " " << val.second << " " << val.third << " ) ";
}

// namespace sparse_matrix
// element: " ( row column value ) "
// dimension: " [ row column ] "
// each line stores one row of matrix, last line stores dimension of matrix
// =============================================================================
namespace sparse_matrix {

template <typename CharT, typename Traits, typename T>
std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, Cell<T>& val) {
	std::string ignored;
	getline(is, ignored, '(');
	is >> val.row >> val.column >> val.value;
	getline(is, ignored, ')');
	return is;
}

template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, const Cell<T>& val) {
	return os << " ( " << val.row << " " << val.column << " " << val.value << " ) ";
}

template <typename CharT, typename Traits>
std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, Dimension& val) {
	std::string ignored;
	getline(is, ignored, '[');
	is >> val.row >> val.column;
	getline(is, ignored, ']');
	return is;
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, const Dimension& val) {
	return os << " [ " << val.row << " " << val.column << " ] ";
}

}  // namespace sparse_matrix

}  // namespace serialization

#endif  // SERIALIZATION_INL_H_