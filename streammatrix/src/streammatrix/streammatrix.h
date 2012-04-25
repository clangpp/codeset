// streammatrix.h
#ifndef STREAMMATRIX_H_
#define STREAMMATRIX_H_

#include <iostream>

#include "../crosslist/crosslist.h"
#include "../serialization/serialization.h"

template <typename T>
class StreamMatrix {
public:
	typedef T value_type;
	typedef crosslist::size_type size_type;

public:
	size_type row_size(size_type row_index);
	size_type column_size(size_type col_index);
	size_type row_count() const;
	size_type column_count() const;
};

#include "streammatrix-inl.h"

#endif  // STREAMMATRIX_H_
