#ifndef STREAMMATRIX_H_
#define STREAMMATRIX_H_

#include <iostream>
#include <iterator>
#include <vector>

#include "crosslist/crosslist.h"
#include "serialization/serialization.h"

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

  class basic_iterator
      : public std::iterator<std::forward_iterator_tag, value_type> {
   public:
    explicit basic_iterator(StreamMatrix* mat = nullptr, pos_type pos = 0)
        : mat_(mat), pos_(pos) {
      if (mat_ != nullptr) {
        step();
      }
    }

   public:
    value_type operator*() const { return cell_.value; }
    value_type* operator->() const { return &*(*this); }
    size_type row() const { return cell_.row; }
    size_type column() const { return cell_.column; }
    bool operator==(const basic_iterator& rhs) const {
      return mat_ == rhs.mat_ && pos_ == rhs.pos_;
    }
    bool operator!=(const basic_iterator& rhs) const { return !(*this == rhs); }

   protected:
    void step() {
      mat_->is_->clear();
      mat_->is_->seekg(pos_);
      *(mat_->is_) >> cell_;
      pos_ = mat_->is_->tellg();
    }

   protected:
    StreamMatrix* mat_ = nullptr;
    pos_type pos_ = 0;
    Cell cell_;
  };

  class iterator : public basic_iterator {
    typedef basic_iterator base;

   public:
    explicit iterator(StreamMatrix* mat = nullptr, pos_type pos = 0)
        : base(mat, pos) {}
    iterator& operator++() {
      base::step();
      return *this;
    }
    iterator operator++(int) {
      iterator iter(*this);
      ++(*this);
      return iter;
    }
  };

  class row_iterator : public basic_iterator {
    typedef basic_iterator base;

   public:
    // pre-condition: row is valid row_index in mat
    explicit row_iterator(StreamMatrix* mat = nullptr, size_type row = 0)
        : base(mat, 0), row_(0) {
      if (base::mat_ != nullptr) {
        locate_row(row);
      }
    }
    row_iterator& operator++() {
      base::step();
      return *this;
    }
    row_iterator operator++(int) {
      row_iterator iter(*this);
      ++(*this);
      return iter;
    }

   protected:
    void locate_row(size_type row) {
      row_ = row;
      base::pos_ = base::mat_->row_begins_[row_];
      base::step();
    }
    bool reach_row_end() const {
      return base::pos_ >= base::mat_->row_begins_[row_ + 1];
    }

   protected:
    size_type row_;
  };

 public:  // iterator observer
  iterator begin() { return iterator(this, row_begins_.at(0)); }
  iterator end() { return iterator(this, row_begins_.back()); }
  row_iterator row_begin(size_type row_index) {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range("invalid row input");
    }
    return row_iterator(this, row_index);
  }
  row_iterator row_end(size_type row_index) {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range("invalid row input");
    }
    return row_iterator(this, row_index + 1);
  }

 public:  // member functions
  StreamMatrix(stream_type& smat) : is_(&smat) { build_index(); }
  StreamMatrix() : is_(nullptr) {}
  void set_stream(stream_type& smat) {
    clear();
    is_ = &smat;
    build_index();
  }
  void clear() {
    is_ = nullptr;
    row_begins_.clear();
    row_sizes_.clear();
    column_sizes_.clear();
    entire_size_ = 0;
    dimension_.row = dimension_.column = 0;
  }
  size_type row_size(size_type row_index) const {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range("invalid row input");
    }
    return row_sizes_[row_index];
  }
  size_type column_size(size_type col_index) const {
    if (!is_valid_column(col_index)) {
      throw std::out_of_range("invalid column input");
    }
    return column_sizes_[col_index];
  }
  size_type size() const { return entire_size_; }
  size_type empty() const { return size() == 0; }
  size_type row_count() const { return dimension_.row; }
  size_type column_count() const { return dimension_.column; }

 private:
  // pre-condition: is_ is a valid stream
  // pre-condition: matrix is stord row first order and row increment
  bool build_index() {
    using namespace serialization::sparsematrix;
    Cell cell;
    Dimension dimension;
    pos_type position = 0;
    while (*is_) {
      try {
        position = is_->tellg();
        if (next_cell(*is_, cell, dimension)) {
          if (cell.row >= row_begins_.size()) {
            row_begins_.resize(cell.row + 1, position);
          }
          if (cell.row >= row_sizes_.size()) {
            row_sizes_.resize(cell.row + 1, 0);
          }
          if (cell.column >= column_sizes_.size()) {
            column_sizes_.resize(cell.column + 1, 0);
          }
          ++row_sizes_[cell.row];
          ++column_sizes_[cell.column];
          ++entire_size_;
        } else {
          // 1 more position for last row's end
          row_begins_.resize(dimension.row + 1, position);
          dimension_ = dimension;
        }
      } catch (const std::exception&) {
        return false;
      }
    }
    return true;
  }
  bool is_valid_row(size_type row_index) const {
    return row_index < dimension_.row;
  }
  bool is_valid_column(size_type col_index) const {
    return col_index < dimension_.column;
  }

 private:
  stream_type* is_ = nullptr;
  std::vector<pos_type> row_begins_;
  std::vector<size_type> row_sizes_;
  std::vector<size_type> column_sizes_;
  size_type entire_size_ = 0;
  Dimension dimension_;
};

#endif  // STREAMMATRIX_H_
