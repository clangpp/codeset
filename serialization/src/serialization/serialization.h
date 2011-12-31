// serialization.h
#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <cstddef>
#include <istream>
#include <ostream>
#include <utility>


namespace serialization {

// std::pair serialization
// =============================================================================
template <typename CharT, typename Traits, typename T1, typename T2>
inline std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, std::pair<T1, T2>& val);

template <typename CharT, typename Traits, typename T1, typename T2>
inline std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, const std::pair<T1, T2>& val);

// class Triad
template <typename T1, typename T2, typename T3>
struct Triad: public std::pair<T1, T2> {
	typedef typename std::pair<T1, T2> pair_type;
	typedef typename pair_type::first_type first_type;
	typedef typename pair_type::second_type second_type;
	typedef T3 third_type;

	third_type third;

	Triad(): pair_type(), third(third_type()) {}
	Triad(const first_type& f, const second_type& s, const third_type& t):
			pair_type(f,s), third(t) {}
};

template <typename CharT, typename Traits, typename T1, typename T2, typename T3>
inline std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, Triad<T1, T2, T3>& val);

template <typename CharT, typename Traits, typename T1, typename T2, typename T3>
inline std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, Triad<T1, T2, T3>& val);

// namespace sparse_matrix
// =============================================================================
namespace sparse_matrix {

// element of sparse matrix, (row, column, value) tuple
template <typename T>
struct Cell {
	// std::size_t is large enough, won't support larger size
	typedef std::size_t size_type;
	typedef T value_type;
	size_type row;  // 0 based
	size_type column;  // 0 based
	value_type value;
	Cell(): row(0), column(0), value() {};
	Cell(size_type r, size_type c, const value_type& v):
		row(r), column(c), value(v) {}
};

template <typename CharT, typename Traits, typename T>
inline std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, Cell<T>& val);

template <typename CharT, typename Traits, typename T>
inline std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, const Cell<T>& val);

// Dimension of sparse matrix, (row, column) pair
struct Dimension {
	typedef std::size_t size_type;
	size_type row;
	size_type column;
	Dimension(): row(0), column(0) {}
	Dimension(size_type r, size_type c): row(r), column(c) {}
};

template <typename CharT, typename Traits>
inline std::basic_istream<CharT, Traits>& operator >> (
		std::basic_istream<CharT, Traits>& is, Dimension& val);

template <typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits>& operator << (
		std::basic_ostream<CharT, Traits>& os, const Dimension& val);

}  // namespace sparse_matrix

}  // namespace serialization

// nest into namespace std  // TBD: don't know allow or not
namespace std {
	using serialization::operator>>;
	using serialization::operator<<;
}  // namespace std

#include "serialization-inl.h"

#endif  // SERIALIZATION_H_
