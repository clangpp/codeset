// streammatrix-inl.h
#ifndef STREAMMATRIX_INL_H_
#define STREAMMATRIX_INL_H_

#include "streammatrix.h"

template <typename T>
StreamMatrix<T>::StreamMatrix(stream_type& smat):
    is_(&smat), entire_size_(0) {
    build_index();
}

template <typename T>
void StreamMatrix<T>::set_stream(stream_type& smat) {
    clear();
    is_ = &smat;
    build_index();
}

template <typename T>
void StreamMatrix<T>::clear() {
    is_ = NULL;
    row_begins_.clear();
    row_sizes_.clear();
    column_sizes_.clear();
    entire_size_ = 0;
    dimension_.row = dimension_.column = 0;
}

template <typename T>
typename StreamMatrix<T>::row_iterator
    StreamMatrix<T>::row_begin(size_type row_index) {
    if (!is_valid_row(row_index))
        throw std::out_of_range("invalid row input");
    return row_iterator(this, row_index);
}

template <typename T>
typename StreamMatrix<T>::row_iterator
    StreamMatrix<T>::row_end(size_type row_index) {
    if (!is_valid_row(row_index))
        throw std::out_of_range("invalid row input");
	return row_iterator(this, row_index+1);
}

template <typename T>
typename StreamMatrix<T>::size_type
	StreamMatrix<T>::row_size(size_type row_index) const {
    if (!is_valid_row(row_index))
        throw std::out_of_range("invalid row input");
    return row_sizes_[row_index];
}

template <typename T>
typename StreamMatrix<T>::size_type
	StreamMatrix<T>::column_size(size_type col_index) const {
    if (!is_valid_column(col_index))
        throw std::out_of_range("invalid column input");
    return column_sizes_[col_index];
}

template <typename T>
bool StreamMatrix<T>::build_index() {
    using namespace serialization::sparsematrix;
    Cell cell;
    Dimension dimension;
    pos_type position=0;
    while (*is_) {
        try {
            position = is_->tellg();
            if (next_cell(*is_, cell, dimension)) {
				if (cell.row >= row_begins_.size())
					row_begins_.resize(cell.row+1, position);
				if (cell.row >= row_sizes_.size())
					row_sizes_.resize(cell.row+1, 0);
				if (cell.column >= column_sizes_.size())
					column_sizes_.resize(cell.column+1, 0);
                ++ row_sizes_[cell.row];
                ++ column_sizes_[cell.column];
                ++ entire_size_;
            } else {
                // 1 more position for last row's end
                row_begins_.resize(dimension.row+1, position);
                dimension_ = dimension;
            }
        } catch (const std::exception&) {
            return false;
        }
    }
    return true;
}

#endif  // STREAMMATRIX_INL_H_
