#ifndef MATRIX_H_
#define MATRIX_H_

// Author: clangpp@gmail.com
//
// Intention:
// 1. C++11 r-value reference is already released and well supported, it's time
// to write a matrix class to take advantage of that, to perform space-cheaper
// matrix arithmetics.
// 2. C++11 std::async() is a powerful tool to perform concurrent operations,
// it can be used to make full use of multi-core CPU's power, to perform
// time-faster matrix arithmetrics.

#include <algorithm>
#include <cstddef>  // size_t, ptrdiff_t
#include <functional>
#include <future>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>  // move(), make_pair(), swap()
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

template <typename T>
struct TrivialIsZero {
  bool operator()(const T& value) {
    return value == 0;
  }
};

// T: StreamOutputable
template <typename T, typename IsZero = TrivialIsZero<T>>
void CheckValueNotZero(const T& value, IsZero is_zero = IsZero()) {
  if (is_zero(value)) {
    std::stringstream ss;
    ss << "Non-zero value expected, actual " << value;
    throw std::invalid_argument(ss.str());
  }
}

void CheckIndicesNotEqual(size_t index1, size_t index2) {
  if (index1 == index2) {
    std::stringstream ss;
    ss << "Indices should'nt be equal, actual " << index1 << " and " << index2;
    throw std::runtime_error(ss.str());
  }
}

template <typename T>
void CheckColumnSizesEqual(
    std::initializer_list<std::initializer_list<T>> il) {
  auto iter = std::adjacent_find(
      il.begin(), il.end(),
      [](std::initializer_list<T> lhs, std::initializer_list<T> rhs) {
        return lhs.size() != rhs.size();
      });
  if (iter != il.end()) {
    auto diff = iter + 1;  // std::adjacent_find() tells us *iter != *(iter+1)
    std::stringstream ss;
    ss << "Each row should have the same column size: "
        << "row " << (diff - il.begin()) << "'s column size " << diff->size()
        << " vs previous rows' column size " << iter->size();
    throw std::invalid_argument(ss.str());
  }
}

template <typename T>
void CheckAugmentable(const Matrix<T>& coef, const Matrix<T>& extra) {
  if (coef.row_size() != extra.row_size()) {
    std::stringstream ss;
    ss << "Equal row size expected, actual "
        << "(" << coef.row_size() << ", " << coef.column_size() << ") vs "
        << "(" << extra.row_size() << ", " << extra.column_size() << ")";
    throw std::runtime_error(ss.str());
  }
}

void ConcurrentProcess(std::size_t count,
                       std::function<void(std::size_t)> process,
                       std::vector<std::future<void>>* helper_futures) {
  helper_futures->resize(count);
  for (std::size_t index = 0; index < count; ++index) {
    (*helper_futures)[index] = std::async(std::launch::async, process, index);
  }
  for (std::size_t index = 0; index < count; ++index) {
    (*helper_futures)[index].wait();
  }
}

inline void ConcurrentProcess(std::size_t count,
                              std::function<void(std::size_t)> process) {
  std::vector<std::future<void>> futures(count);
  ConcurrentProcess(count, process, &futures);
}

template <typename T>
void Print(const Matrix<T>& mat, std::ostream* os) {
  typedef typename Matrix<T>::size_type size_type;
  for (size_type row = 0; row < mat.row_size(); ++row) {
    for (size_type column = 0; column < mat.row_size(); ++column) {
      (*os) << mat[row][column] << " ";
    }
    (*os) << "\n";
  }
}

template <typename T>
void PrintAugmented(const Matrix<T>& coef, const Matrix<T>& extra,
                    std::ostream* os) {
  CheckAugmentable(coef, extra);
  typedef typename Matrix<T>::size_type size_type;
  for (size_type row = 0; row < coef.row_size(); ++row) {
    for (size_type column = 0; column < coef.row_size(); ++column) {
      (*os) << coef[row][column] << " ";
    }
    (*os) << "| ";
    for (size_type column = 0; column < extra.row_size(); ++column) {
      (*os) << extra[row][column] << " ";
    }
    (*os) << "\n";
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

    bool operator==(const self& other) const {
      return (data_ == other.data_ && row_ == other.row_ &&
              column_ == other.column_);
    }
    bool operator!=(const self& other) const { return !(*this == other); }
    reference operator*() const { return (*data_)[row_][column_]; }
    pointer operator->() const { return &*(*this); }

    // ==== ForwardIterator requirements ====

    self& operator++() {
      ++row_;
      return *this;
    }
    self operator++(int) {
      self result(*this);
      ++*this;
      return result;
    }

    // ==== BidirectionalIterator requirements ====

    self& operator--() {
      --row_;
      return *this;
    }
    self operator--(int){
      self result(*this);
      --*this;
      return result;
    }

    // ==== RandomAccessIterator requirements ====

    self& operator+=(difference_type n) {
      row_ += n;
      return *this;
    }
    self operator+(difference_type n) const {
      self result(*this);
      return result += n;
    }
    friend self operator+(difference_type n, const self& iter) {
      return iter + n;
    }
    self& operator-=(difference_type n) { return *this += (-n); }
    self operator-(difference_type n) const {
      self result(*this);
      return result -= n;
    }
    difference_type operator-(const self& other) const {
      return row_ - other.row_;
    }
    reference operator[](difference_type n) const { return *(*this + n); }
    bool operator<(const self& other) const { return row_ < other.row_; }
    bool operator>(const self& other) const { return other < *this; }
    bool operator>=(const self& other) const { return !(*this < other); }
    bool operator<=(const self& other) const { return !(*this > other); }
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
    reference operator[](size_type index) {
      return (*vec_)[index];
    }
   private:
    friend class Matrix;
    BracketBridge(std::vector<value_type>* vec): vec_(vec) {}
    std::vector<value_type>* vec_;
  };
  class ConstBracketBridge {
   public:
    const_reference operator[](size_type index) const {
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

  Matrix(std::initializer_list<std::initializer_list<value_type>> il)
    : column_size_(0) {
    std::clog << "Matrix::Matrix("
        << "std::initializer_list<std::initializer_list<value_type>>)"
        << std::endl;
    matrix::CheckColumnSizesEqual(il);
    for (auto row_il : il) {
      data_.emplace_back(row_il);
    }
    if (il.size() > 0) {
      column_size_ = il.begin()->size();
    }
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

  Matrix& operator=(const Matrix& other) {
    std::clog << "Matrix::operator=(const Matrix&)" << std::endl;
    if (this != &other) {
      data_ = other.data_;
      column_size_ = other.column_size_;
    }
    return *this;
  }

  Matrix& operator=(Matrix&& other) {
    std::clog << "Matrix::operator=(Matrix&&)" << std::endl;
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

  reference operator()(size_type row, size_type column) {
    matrix::CheckIndicesRange(*this, row, column);
    return data_[row][column];
  }

  const_reference operator()(size_type row, size_type column) const {
    matrix::CheckIndicesRange(*this, row, column);
    return data_[row][column];
  }

  // Supports access pattern `mat[row][column]`.
  // NOTE(clangpp): No index check is performed in this way.
  BracketBridge operator[](size_type row) {
    return BracketBridge(&data_[row]);
  }

  // Supports access pattern `const_mat[row][column]`.
  // NOTE(clangpp): No index check is performed in this way.
  ConstBracketBridge operator[](size_type row) const {
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

  Matrix& operator+=(const Matrix& other) {
    matrix::CheckDimensionMatches(*this, other);
    matrix::ConcurrentProcess(
        row_size(), [this, &other](size_type row) mutable {
          for (size_type column = 0; column < this->column_size(); ++column) {
            data_[row][column] += other.data_[row][column];
          }
        });
    return *this;
  }

  Matrix& operator-=(const Matrix& other) {
    matrix::CheckDimensionMatches(*this, other);
    matrix::ConcurrentProcess(
        row_size(), [this, &other](size_type row) mutable {
          for (size_type column = 0; column < this->column_size(); ++column) {
            data_[row][column] -= other.data_[row][column];
          }
        });
    return *this;
  }

  Matrix& operator*=(const value_type& factor) {
    matrix::ConcurrentProcess(
        row_size(), [this, &factor](size_type row) mutable {
          for (size_type column = 0; column < this->column_size(); ++column) {
            data_[row][column] *= factor;
          }
        });
    return *this;
  }

  Matrix& operator/=(const value_type& factor) {
    matrix::ConcurrentProcess(
        row_size(), [this, &factor](size_type row) mutable {
          for (size_type column = 0; column < this->column_size(); ++column) {
            data_[row][column] /= factor;
          }
        });
    return *this;
  }

  Matrix& operator%=(const value_type& factor) {
    matrix::ConcurrentProcess(
        row_size(), [this, &factor](size_type row) mutable {
          for (size_type column = 0; column < this->column_size(); ++column) {
            data_[row][column] %= factor;
          }
        });
    return *this;
  }

  Matrix& operator*=(const Matrix& other) {
    matrix::CheckDimensionMultipliable(*this, other);
    std::vector<value_type> row_result(other.column_size());
    std::vector<std::future<void>> futures(other.column_size());
    for (size_type row = 0; row < row_size(); ++row) {
      row_result.resize(other.column_size());

      // Concurrently calculates single row in result matrix.
      matrix::ConcurrentProcess(
          other.column_size(),
          [this, &other, row, &row_result](size_type column) mutable {
            row_result[column] = std::inner_product(
                this->row_begin(row), this->row_end(row),
                other.column_begin(column), value_type());
          },
          &futures);

      // Inplace puts calculated row into result matrix.
      data_[row].swap(row_result);
    }
    column_size_ = other.column_size();
    return *this;
  }

  friend Matrix operator*(const Matrix& lhs, Matrix&& rhs) {
    matrix::CheckDimensionMultipliable(lhs, rhs);
    size_type rhs_row_size(rhs.row_size()), rhs_column_size(rhs.column_size());
    Matrix result(std::move(rhs));
    result.row_resize(std::max(rhs_row_size, lhs.row_size()));
    std::vector<value_type> column_result(lhs.row_size());
    std::vector<std::future<void>> futures(lhs.row_size());
    for (size_type column = 0; column < rhs_column_size; ++column) {
      // Concurrently calculates single column in result matrix.
      matrix::ConcurrentProcess(
          lhs.row_size(),
          [&lhs, &result, column, &column_result](size_type row) mutable {
            column_result[row] = std::inner_product(
                lhs.row_begin(row), lhs.row_end(row),
                result.column_begin(column), value_type());
          },
          &futures);

      // Puts calculated column into result matrix.
      std::copy(column_result.begin(), column_result.end(),
                result.column_begin(column));
    }
    result.row_resize(lhs.row_size());
    return result;
  }

  // ==== Elementary operations ====

  // NOTE(clangpp): time complexity O(1), space complexity O(1)
  Matrix& elementary_row_switch(size_type row1, size_type row2) {
    matrix::CheckRowRange(*this, row1);
    matrix::CheckRowRange(*this, row2);
    data_[row1].swap(data_[row2]);
    return *this;
  }

  // NOTE(clangpp): time complexity O(column_size()), space complexity O(1)
  Matrix& elementary_row_multiply(
      size_type row, const value_type& factor,
      std::function<bool(const value_type& value)> is_zero =
          [](const value_type& value) { return value == 0; }) {
    matrix::CheckRowRange(*this, row);
    // elementary multiplication requires factor != 0
    matrix::CheckValueNotZero(factor, is_zero);
    for (size_type column = 0; column < column_size(); ++column) {
      data_[row][column] *= factor;
    }
    return *this;
  }

  // NOTE(clangpp): time complexity O(column_size()), space complexity O(1)
  Matrix& elementary_row_add(size_type row_target, size_type row_adding,
                             const value_type& factor) {
    matrix::CheckRowRange(*this, row_target);
    matrix::CheckRowRange(*this, row_adding);
    // elementary addition requires row_target != row_adding
    matrix::CheckIndicesNotEqual(row_target, row_adding);
    for (size_type column = 0; column < column_size(); ++column) {
      data_[row_target][column] += data_[row_adding][column] * factor;
    }
    return *this;
  }

  // NOTE(clangpp): time complexity O(row_size()), space complexity O(1)
  Matrix& elementary_column_switch(size_type column1, size_type column2) {
    matrix::CheckColumnRange(*this, column1);
    matrix::CheckColumnRange(*this, column1);
    for (size_type row = 0; row < row_size(); ++row) {
      std::swap(data_[row][column1], data_[row][column2]);
    }
    return *this;
  }

  // NOTE(clangpp): time complexity O(row_size()), space complexity O(1)
  Matrix& elementary_column_multiply(
      size_type column, const value_type& factor,
      std::function<bool(const value_type& value)> is_zero =
          [](const value_type& value) { return value == 0; }) {
    matrix::CheckColumnRange(*this, column);
    // elementary multiplication requires factor != 0
    matrix::CheckValueNotZero(factor, is_zero);
    for (size_type row = 0; row < row_size(); ++row) {
      data_[row][column] *= factor;
    }
    return *this;
  }

  // NOTE(clangpp): time complexity O(row_size()), space complexity O(1)
  Matrix& elementary_column_add(
      size_type column_target, size_type column_adding,
      const value_type& factor) {
    matrix::CheckColumnRange(*this, column_target);
    matrix::CheckColumnRange(*this, column_adding);
    // elementary addition requires column_target != column_adding
    matrix::CheckIndicesNotEqual(column_target, column_adding);
    for (size_type row = 0; row < row_size(); ++row) {
      data_[row][column_target] += data_[row][column_adding] * factor;
    }
    return *this;
  }

  // ==== Comparasion operators ====

  bool equal_to(
      const Matrix& other,
      std::function<bool(const value_type&, const value_type&)> value_equal_to =
          std::equal_to<value_type>()) const {
    if (row_size() != other.row_size() ||
        column_size() != other.column_size()) {
      return false;
    }
    std::vector<std::future<bool>> futures(row_size());
    typedef typename Matrix<T>::size_type size_type;
    for (size_type row = 0; row < row_size(); ++row) {
      futures[row] = std::async(
          std::launch::async,
          [this, &other, row, &value_equal_to]() {
            for (size_type column = 0; column < this->column_size(); ++column) {
              if (!value_equal_to((*this)[row][column], other[row][column])) {
                return false;
              }
            }
            return true;
          });
    }
    for (size_type row = 0; row < row_size(); ++row) {
      if (!futures[row].get()) {
        return false;
      }
    }
    return true;
  }

 private:
  std::vector<std::vector<value_type>> data_;
  size_type column_size_;
};

// ==== Arithmetric operators ====

template <typename T>
Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(lhs);
  return result += rhs;
}

template <typename T>
Matrix<T> operator+(const Matrix<T>& lhs, Matrix<T>&& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(std::move(rhs));
  // TODO(clangpp): Dont' know why, but if merge the following two lines to
  // `return result += lhs;`, there will be an additional
  // Matrix::Matrix(const Matrix&) call.
  result += lhs;
  return result;
}

template <typename T>
Matrix<T> operator+(Matrix<T>&& lhs, const Matrix<T>& rhs) {
  return rhs + std::move(lhs);
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(lhs);
  return result -= rhs;
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& lhs, Matrix<T>&& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(std::move(rhs));
  typedef typename Matrix<T>::size_type size_type;
  matrix::ConcurrentProcess(
      result.row_size(),
      [&lhs, &result](size_type row) mutable {
        for (size_type column = 0; column < result.column_size(); ++column) {
          result[row][column] = lhs[row][column] - result[row][column];
        }
      });
  return result;
}

template <typename T>
Matrix<T> operator-(Matrix<T>&& lhs, const Matrix<T>& rhs) {
  matrix::CheckDimensionMatches(lhs, rhs);
  Matrix<T> result(std::move(lhs));
  result -= rhs;
  return result;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& mat, const T& factor) {
  Matrix<T> result(mat);
  return result *= factor;
}

template <typename T>
Matrix<T> operator*(const T& factor, const Matrix<T>& mat) {
  return mat * factor;
}

template <typename T>
Matrix<T> operator*(Matrix<T>&& mat, const T& factor) {
  Matrix<T> result(std::move(mat));
  result *= factor;
  return result;
}

template <typename T>
Matrix<T> operator*(const T& factor, Matrix<T>&& mat) {
  return std::move(mat) * factor;
}

template <typename T>
Matrix<T> operator/(const Matrix<T>& mat, const T& factor) {
  Matrix<T> result(mat);
  return result /= factor;
}

template <typename T>
Matrix<T> operator/(Matrix<T>&& mat, const T& factor) {
  Matrix<T> result(std::move(mat));
  result /= factor;
  return result;
}

template <typename T>
Matrix<T> operator%(const Matrix<T>& mat, const T& factor) {
  Matrix<T> result(mat);
  return result %= factor;
}

template <typename T>
Matrix<T> operator%(Matrix<T>&& mat, const T& factor) {
  Matrix<T> result(std::move(mat));
  result %= factor;
  return result;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  Matrix<T> result(lhs);
  return result *= rhs;
}

template <typename T>
Matrix<T> operator*(Matrix<T>&& lhs, const Matrix<T>& rhs) {
  Matrix<T> result(std::move(lhs));
  result *= rhs;
  return result;
}

// ==== Comparasion operators ====

template <typename T>
bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  return lhs.equal_to(rhs);
}

template <typename T>
bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  return !(lhs == rhs);
}

#endif  // MATRIX_H_
