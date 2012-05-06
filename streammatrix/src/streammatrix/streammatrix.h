// streammatrix.h
#ifndef STREAMMATRIX_H_
#define STREAMMATRIX_H_

#include <iostream>
#include <iterator>
#include <vector>

#include "../crosslist/crosslist.h"
#include "../serialization/serialization.h"

template <typename T>
class StreamMatrix {
private:
    typedef serialization::sparsematrix::Cell<T> Cell;
    typedef serialization::sparsematrix::Dimension Dimension;

public:  // type definition
	typedef T value_type;
	typedef crosslist::size_type size_type;
    typedef std::istream stream_type;
    typedef stream_type::pos_type pos_type;
    typedef stream_type::off_type off_type;

public:  // iterator types
    class iterator;
    typedef iterator const_iterator;
    class row_iterator;
    typedef row_iterator const_row_iterator;
    friend class basic_iterator;
    friend class row_iterator;

    class basic_iterator:
        public std::iterator<std::forward_iterator_tag, value_type> {
    public:
        explicit basic_iterator(StreamMatrix* mat=NULL, pos_type pos=0):
            mat_(mat), pos_(pos) {
            if (mat_!=NULL) step();
        }
    public:
        value_type operator * () const { return cell_.value; }
        value_type* operator -> () const { return &*(*this); }
        size_type row() const { return cell_.row; }
        size_type column() const { return cell_.column; }
        bool operator == (const basic_iterator& rhs) const {
            return mat_==rhs.mat_ && pos_==rhs.pos_;
        }
        bool operator != (const basic_iterator& rhs) const {
			return !(*this==rhs);
		}
    protected:
        void step() {
			mat_->is_->clear();
            mat_->is_->seekg(pos_);
            *(mat_->is_) >> cell_;
			pos_ = mat_->is_->tellg();
        }
    protected:
        StreamMatrix* mat_;
        pos_type pos_;
        Cell cell_;
    };

    class iterator: public basic_iterator {
		typedef basic_iterator base;
    public:
        explicit iterator(StreamMatrix* mat=NULL, pos_type pos=0):
            base(mat, pos) {}
        iterator& operator ++ () { base::step(); return *this; }
        iterator operator ++ (int) {
            iterator iter(*this); ++(*this); return iter;
        }
    };

    class row_iterator: public basic_iterator {
        typedef basic_iterator base;
    public:
        // pre-condition: row is valid row_index in mat
        explicit row_iterator(StreamMatrix* mat=NULL, size_type row=0):
            base(mat, 0), row_(0) {
            if (base::mat_!=NULL) {
                locate_row(row);
            }
        }
        row_iterator& operator ++ () {
            base::step(); return *this;
        }
        row_iterator operator ++ (int) {
            row_iterator iter(*this); ++(*this); return iter;
        }
    protected:
        void locate_row(size_type row) {
            row_ = row;
            base::pos_ = base::mat_->row_begins_[row_];
			base::step();
        }
        bool reach_row_end() const {
            return base::pos_ >= base::mat_->row_begins_[row_+1];
        }
    protected:
        size_type row_;
    };

public:  // iterator observer
    iterator begin() { return iterator(this, row_begins_.at(0)); }
    iterator end() { return iterator(this, row_begins_.back()); }
    row_iterator row_begin(size_type row_index);
    row_iterator row_end(size_type row_index);

public:  // member functions
    StreamMatrix(stream_type& smat);
    StreamMatrix(): is_(NULL), entire_size_(0) {}
    void set_stream(stream_type& smat);
    void clear();
	size_type row_size(size_type row_index) const;
	size_type column_size(size_type col_index) const;
    size_type size() const { return entire_size_; }
    size_type empty() const { return size()==0; }
	size_type row_count() const { return dimension_.row; }
	size_type column_count() const { return dimension_.column; }

private:
    // pre-condition: is_ is a valid stream
    // pre-condition: matrix is stord row first order and row increment
    bool build_index();
    bool is_valid_row(size_type row_index) const {
        return row_index < dimension_.row;
    }
    bool is_valid_column(size_type col_index) const {
        return col_index < dimension_.column;
    }

private:
    stream_type* is_;
    std::vector<pos_type> row_begins_;
    std::vector<size_type> row_sizes_;
    std::vector<size_type> column_sizes_;
    size_type entire_size_;
    Dimension dimension_;
};

#include "streammatrix-inl.h"

#endif  // STREAMMATRIX_H_
