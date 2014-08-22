#ifndef MATRIX_H_
#define MATRIX_H_

// Author: clangpp@gmail.com
//
// Intention:
// C++11 r-value reference is already released and well supported, it's time to
// write a matrix class to take advantage of that, to perform space-cheaper
// matrix arithmetics.

#include <algorithm>
#include <cstddef>  // size_t
#include <iterator>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <utility>  // move(), make_pair
#include <vector>

#include <iostream>  // debug

template <typename T> class Matrix;

namespace matrix {

template <typename T>
void CheckDimensionMatches(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  if (lhs.row_size() != rhs.row_size() ||
      lhs.column_size() != rhs.column_size()) {
    std::stringstream ss;
    ss << "Matrices dimensions don't match: "
      << "(" << lhs.row_size() << ", " << lhs.column_size() << ") vs "
      << "(" << rhs.row_size() << ", " << rhs.column_size() << ")";
    throw std::runtime_error(ss.str());
  }
}

template <typename T>
void CheckDimensionMultipliable(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  if (lhs.column_size() != rhs.row_size()) {
    std::stringstream ss;
    ss << "Matrices dimensions not multipliable: "
      << "(" << lhs.row_size() << ", " << lhs.column_size() << ") vs "
      << "(" << rhs.row_size() << ", " << rhs.column_size() << ")";
    throw std::runtime_error(ss.str());
  }
}

template <typename T>
void CheckIndicesRange(const Matrix<T>& mat,
                       typename Matrix<T>::size_type row,
                       typename Matrix<T>::size_type column) {
  if (row >= mat.row_size() || column >= mat.column_size()) {
    std::stringstream ss;
    ss << "Matrix indices (" << row << ", " << column << ") out of range "
       << "(" << mat.row_size() << ", " << mat.column_size() << ")";
    throw std::out_of_range(ss.str());
  }
}

template <typename T>
void CheckRowRange(const Matrix<T>& mat, typename Matrix<T>::size_type row) {
  if (row >= mat.row_size()) {
    std::stringstream ss;
    ss << "Matrix row " << row << ") out of range "
       << "(" << mat.row_size() << ", " << mat.column_size() << ")";
    throw std::out_of_range(ss.str());
  }
}

template <typename T>
void CheckColumnRange(const Matrix<T>& mat,
                      typename Matrix<T>::size_type column) {
  if (column >= mat.column_size()) {
    std::stringstream ss;
    ss << "Matrix column " << column << ") out of range "
       << "(" << mat.row_size() << ", " << mat.column_size() << ")";
    throw std::out_of_range(ss.str());
  }
}

}  // namespace matrix

template <typename T>
class Matrix {
 public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  // ==== Iterators ====

  typedef typename std::vector<value_type>::iterator row_iterator;
  typedef typename std::vector<value_type>::const_iterator const_row_iterator;

  template <typename Container, typename ValueType>
  class basic_column_iterator:
      public std::iterator<std::random_access_iterator_tag, ValueType> {
   public:
    typedef basic_column_iterator self;
    typedef std::iterator<std::random_access_iterator_tag, ValueType> base;
    typedef typename base::difference_type difference_type;
    typedef typename base::pointer pointer;
    typedef typename base::reference reference;

    // ==== InputIterator requirements ====

    bool operator == (const self& other) const {
      return (data_ == other.data_ && row_ == other.row_ &&
              column_ == other.column_);
    }
    bool operator != (const self& other) const { return !(*this == other); }
    reference operator * () const { return (*data_)[row_][column_]; }
    pointer operator -> () const { return &*(*this); }

    // ==== ForwardIterator requirements ====

    self& operator ++ () {
      ++row_;
      return *this;
    }
    self operator ++ (int) {
      self result(*this);
      ++*this;
      return result;
    }

    // ==== BidirectionalIterator requirements ====

    self& operator -- () {
      --row_;
      return *this;
    }
    self operator --(int) {
      self result(*this);
      --*this;
      return result;
    }

    // ==== RandomAccessIterator requirements ====

    self& operator += (difference_type n) {
      row_ += n;
      return *this;
    }
    self operator + (difference_type n) const {
      self result(*this);
      return result += n;
    }
    friend self operator + (difference_type n, const self& iter) {
      return iter + n;
    }
    self& operator -= (difference_type n) { return *this += (-n); }
    self operator - (difference_type n) const {
      self result(*this);
      return result -= n;
    }
    difference_type operator - (const self& other) const {
      return row_ - other.row_;
    }
    reference operator [] (difference_type n) const { return *(*this + n); }
    bool operator < (const self& other) const { return row_ < other.row_; }
    bool operator > (const self& other) const { return other < *this; }
    bool operator >= (const self& other) const { return !(*this < other); }
    bool operator <= (const self& other) const { return !(*this > other); }
   private:
    friend class Matrix;
    basic_column_iterator(Container* data, size_type row, size_type column)
      : data_(data), row_(row), column_(column) {
    }
    Container* data_;
    size_type row_;
    size_type column_;
  };
  typedef basic_column_iterator<
      std::vector<std::vector<value_type>>, value_type> column_iterator;
  typedef basic_column_iterator<
      const std::vector<std::vector<value_type>>, const value_type>
      const_column_iterator;

 private:
  // inner class, used for implementing `mat[i][j]` access pattern.
  class BracketBridge {
   public:
    reference operator [] (size_type index) {
      return (*vec_)[index];
    }
   private:
    friend class Matrix;
    BracketBridge(std::vector<value_type>* vec): vec_(vec) {}
    std::vector<value_type>* vec_;
  };
  class ConstBracketBridge {
   public:
    const_reference operator [] (size_type index) const {
      return (*vec_)[index];
    }
   private:
    friend class Matrix;
    ConstBracketBridge(const std::vector<value_type>* vec): vec_(vec) {}
    const std::vector<value_type>* vec_;
  };

 public:
  // ==== Constructors and (default) Destructor ====

  Matrix(size_type row_size, size_type column_size,
                  const value_type& filled = value_type())
    : data_(row_size, std::vector<value_type>(column_size, filled)),
      column_size_(column_size) {
    std::clog << "Matrix::Matrix(size_type, size_type, const value_type&)"
        << std::endl;
  }

  Matrix(): column_size_(0) {
    std::clog << "Matrix::Matrix()" << std::endl;
  }

  Matrix(const Matrix& other)
    : data_(other.data_), column_size_(other.column_size_) {
    std::clog << "Matrix::Matrix(const Matrix&)" << std::endl;
  }

  Matrix(Matrix&& other)
    : data_(std::move(other.data_)), column_size_(other.column_size_) {
    other.column_size_ = 0;
    std::clog << "Matrix::Matrix(Matrix&&)" << std::endl;
  }

  // ==== Assignment operator ====

  Matrix& operator = (const Matrix& other) {
    std::clog << "Matrix::operator = (const Matrix&)" << std::endl;
    data_ = other.data_;
    column_size_ = other.column_size_;
    return *this;
  }

  Matrix& operator = (Matrix&& other) {
    std::clog << "Matrix::operator = (Matrix&&)" << std::endl;
    if (this != &other) {
      data_ = std::move(other.data_);
      column_size_ = other.column_size_;
    }
    return *this;
  }

  // ==== Observers ====

  size_type row_size() const { return data_.size(); }

  size_type column_size() const { return column_size_; }

  size_type size() const {
    return row_size() * column_size();
  }

  std::pair<size_type, size_type> dimension() const {
    return std::make_pair(row_size(), column_size());
  }

  bool empty() const { return row_size() == 0 || column_size() == 0; }

  reference operator () (size_type row, size_type column) {
    matrix::CheckIndicesRange(*this, row, column);
    return data_[row][column];
  }

  const_reference operator () (size_type row, size_type column) const {
    matrix::CheckIndicesRange(*this, row, column);
    return data_[row][column];
  }

  // Supports access pattern `mat[row][column]`.
  // NOTE(clangpp): No index check is performed in this way.
  BracketBridge operator [] (size_type row) {
    return BracketBridge(&data_[row]);
  }

  // Supports access pattern `const_mat[row][column]`.
  // NOTE(clangpp): No index check is performed in this way.
  ConstBracketBridge operator [] (size_type row) const {
    return ConstBracketBridge(&data_[row]);
  }

  // ==== Iterators ====

  row_iterator row_begin(size_type row) {
    matrix::CheckRowRange(*this, row);
    return data_[row].begin();
  }

  row_iterator row_end(size_type row) {
    matrix::CheckRowRange(*this, row);
    return data_[row].end();
  }

  const_row_iterator row_begin(size_type row) const {
    matrix::CheckRowRange(*this, row);
    return data_[row].begin();
  }

  const_row_iterator row_end(size_type row) const {
    matrix::CheckRowRange(*this, row);
    return data_[row].end();
  }

  column_iterator column_begin(size_type column) {
    matrix::CheckColumnRange(*this, column);
    return column_iterator(&data_, 0, column);
  }

  column_iterator column_end(size_type column) {
    matrix::CheckColumnRange(*this, column);
    return column_iterator(&data_, data_.size(), column);
  }

  const_column_iterator column_begin(size_type column) const {
    matrix::CheckColumnRange(*this, column);
    return const_column_iterator(&data_, 0, column);
  }

  const_column_iterator column_end(size_type column) const {
    matrix::CheckColumnRange(*this, column);
    return const_column_iterator(&data_, data_.size(), column);
  }

  // ==== Manipulators ====

  void row_resize(size_type new_row_size,
                  const value_type& filled = value_type()) {
    data_.resize(new_row_size, std::vector<value_type>(column_size_, filled));
  }

  void column_resize(size_type new_column_size,
                     const value_type& filled = value_type()) {
    for (auto& row_vec : data_) {
      row_vec.resize(new_column_size, filled);
    }
    column_size_ = new_column_size;
  }

  // ==== Arithmetric operators ====

  Matrix& operator += (const Matrix& other) {
    matrix::CheckDimensionMatches(*this, other);
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] += other.data_[i][j];
      }
    }
    return *this;
  }

  Matrix& operator -= (const Matrix& other) {
    matrix::CheckDimensionMatches(*this, other);
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] -= other.data_[i][j];
      }
    }
    return *this;
  }

  Matrix& operator *= (const value_type& factor) {
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] *= factor;
      }
    }
    return *this;
  }

  Matrix& operator /= (const value_type& factor) {
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] /= factor;
      }
    }
    return *this;
  }

  Matrix& operator %= (const value_type& factor) {
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] %= factor;
      }
    }
    return *this;
  }

  Matrix& operator *= (const Matrix& other) {
    matrix::CheckDimensionMultipliable(*this, other);
    std::vector<value_type> row_result(other.column_size());
    for (size_type row = 0; row < row_size(); ++row) {
      row_result.resize(other.column_size());
      for (size_type column = 0; column < other.column_size(); ++column) {
        row_result[column] = std::inner_product(
            row_begin(row), row_end(row), other.column_begin(column),
            value_type());
      }
      data_[row].swap(row_result);
    }
    column_size_ = other.column_size();
    return *this;
  }

  friend Matrix operator * (const Matrix& lhs, Matrix&& rhs) {
    matrix::CheckDimensionMultipliable(lhs, rhs);
    size_type rhs_row_size(rhs.row_size()), rhs_column_size(rhs.column_size());
    Matrix result(std::move(rhs));
    result.row_resize(std::max(rhs_row_size, lhs.row_size()));
    std::vector<value_type> column_result(lhs.row_size());
    for (size_type column = 0; column < rhs_column_size; ++column) {
      for (size_type row = 0; row < lhs.row_size(); ++row) {
        column_result[row] = std::inner_product(
            lhs.row_begin(row), lhs.row_end(row), result.column_begin(column),
            value_type());
      }
      std::copy(column_result.begin(), column_result.end(),
                result.column_begin(column));
    }
    result.row_resize(lhs.row_size());
    return result;
  }

 private:
  std::vector<std::vector<value_type>> data_;
  size_type column_size_;
};

// ==== Arithmetric operators ====

template <typename T>
Matrix<T> operator + (const Matrix<T>& lhs, const Matrix<T>& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(lhs);
  return result += rhs;
}

template <typename T>
Matrix<T> operator + (const Matrix<T>& lhs, Matrix<T>&& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(std::move(rhs));
  // TODO(clangpp): Dont' know why, but if merge the following two lines to
  // `return result += lhs;`, there will be an additional
  // Matrix::Matrix(const Matrix&) call.
  result += lhs;
  return result;
}

template <typename T>
Matrix<T> operator + (Matrix<T>&& lhs, const Matrix<T>& rhs) {
  return rhs + std::move(lhs);
}

template <typename T>
Matrix<T> operator - (const Matrix<T>& lhs, const Matrix<T>& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(lhs);
  return result -= rhs;
}

template <typename T>
Matrix<T> operator - (const Matrix<T>& lhs, Matrix<T>&& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(std::move(rhs));
  typedef typename Matrix<T>::size_type size_type;
  for (size_type i=0; i<result.row_size(); ++i) {
    for (size_type j=0; j<result.column_size(); ++j) {
      result[i][j] = lhs[i][j] - result[i][j];
    }
  }
  return result;
}

template <typename T>
Matrix<T> operator - (Matrix<T>&& lhs, const Matrix<T>& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(std::move(lhs));
  result -= rhs;
  return result;
}

template <typename T>
Matrix<T> operator * (const Matrix<T>& mat, const T& factor) {
  Matrix<T> result(mat);
  return result *= factor;
}

template <typename T>
Matrix<T> operator * (const T& factor, const Matrix<T>& mat) {
  return mat * factor;
}

template <typename T>
Matrix<T> operator * (Matrix<T>&& mat, const T& factor) {
  Matrix<T> result(std::move(mat));
  result *= factor;
  return result;
}

template <typename T>
Matrix<T> operator * (const T& factor, Matrix<T>&& mat) {
  return std::move(mat) * factor;
}

template <typename T>
Matrix<T> operator / (const Matrix<T>& mat, const T& factor) {
  Matrix<T> result(mat);
  return result /= factor;
}

template <typename T>
Matrix<T> operator / (Matrix<T>&& mat, const T& factor) {
  Matrix<T> result(std::move(mat));
  result /= factor;
  return result;
}

template <typename T>
Matrix<T> operator % (const Matrix<T>& mat, const T& factor) {
  Matrix<T> result(mat);
  return result %= factor;
}

template <typename T>
Matrix<T> operator % (Matrix<T>&& mat, const T& factor) {
  Matrix<T> result(std::move(mat));
  result %= factor;
  return result;
}

template <typename T>
Matrix<T> operator * (const Matrix<T>& lhs, const Matrix<T>& rhs) {
  Matrix<T> result(lhs);
  return result *= rhs;
}

template <typename T>
Matrix<T> operator * (Matrix<T>&& lhs, const Matrix<T>& rhs) {
  Matrix<T> result(std::move(lhs));
  result *= rhs;
  return result;
}

#endif  // MATRIX_H_
