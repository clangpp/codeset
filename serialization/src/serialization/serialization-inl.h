// serialization-inl.h
#ifndef SERIALIZATION_INL_H_
#define SERIALIZATION_INL_H_

#include "serialization.h"
#include <stdexcept>
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

// namespace sparsematrix
// element: " ( row column value ) "
// dimension: " [ row column ] "
// each line stores one row of matrix, last line stores dimension of matrix
// =============================================================================
namespace sparsematrix {

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

template <typename CharT, typename Traits, typename T>
bool next_cell(
        std::basic_istream<CharT, Traits>& is, Cell<T>& cell, Dimension& dim) {
    char beg_ch=0;
    is >> beg_ch;
    is.putback(beg_ch);
    bool is_cell = false;
    switch (beg_ch) {
    case '(':  // cell unit
        is >> cell;
        is_cell = true;
        break;
    case '[':  // dimension unit
        is >> dim;
        is_cell = false;
        break;
    default:
        is_cell = false;
        throw std::runtime_error("unknown pattern");
        break;
    }
    return is_cell;
}

}  // namespace sparsematrix

template <typename CharT, typename Traits, typename T>
std::basic_istream<CharT, Traits>& operator >> (
        std::basic_istream<CharT, Traits>& is, CrossList<T>& c) {
    typedef serialization::sparsematrix::Cell<T> cell_type;
    typedef serialization::sparsematrix::Dimension dimension_type;
	c.clear();
    cell_type cell;
    dimension_type dimension;
    while (is) {  // one unit per cycle
        try {
            if (sparsematrix::next_cell(is, cell, dimension)) {
                if (cell.row>=c.row_count())
                    c.row_reserve(cell.row+1);
                if (cell.column>=c.column_count())
                    c.column_reserve(cell.column+1);
                c.rinsert(cell.row, cell.column, cell.value);
            } else {
                c.reserve(dimension.row, dimension.column);
            }
        } catch (const std::exception& e) {
            break;
        }
    }
    return is;
}

template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits>& operator << (
        std::basic_ostream<CharT, Traits>& os, const CrossList<T>& c) {
    typedef serialization::sparsematrix::Cell<T> cell_type;
    typedef serialization::sparsematrix::Dimension dimension_type;
    typedef typename CrossList<T>::const_row_iterator const_row_iterator;
	for (typename CrossList<T>::size_type r=0; r<c.row_count(); ++r)
	{
		if (c.row_size(r)==0) continue;
		const_row_iterator row_iter=c.row_begin(r), row_end=c.row_end(r);
		for (; row_iter!=row_end; ++row_iter)	// write every node
            os << cell_type(row_iter.row(),row_iter.column(),*row_iter);
		os << std::endl;
	}
	os << dimension_type(c.row_count(), c.column_count()) << std::flush;
	return os;
}

}  // namespace serialization

#endif  // SERIALIZATION_INL_H_

