#ifndef MATRIX_H_
#define MATRIX_H_

#ifndef LOG_DETAIL
#define LOG_DETAIL
#endif  // LOG_DETAIL

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/**
 * A small tool to log stuff to standard log stream (stderr).
 */
class SimpleLogger {
 public:
  ~SimpleLogger() {
    std::clog << std::endl;
  }
  template <typename T>
  const SimpleLogger& operator << (const T& message) const {
    std::clog << message;
    return *this;
  }
};
class EmptyLogger {
 public:
  template <typename T>
  const SimpleLogger& operator << (const T& message) const { }
};

#ifdef LOG
# undef LOG
#endif  // LOG

# ifdef LOG_DETAIL
#   define LOG (SimpleLogger() << __FILE__ << ":" << __LINE__ << " " \
                << __func__ << "(): ")
# else  // LOG_DETAIL
#   define LOG (EmptyLogger())
# endif  // LOG_DETAIL

/**
 * A simple matrix class based on std::vector, to show the basic usage of C++
 * features.
 */
template <typename T>
class Matrix {
 public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef std::size_t size_type;

 public:
  // ==== Constructors and (default) Destructor ====

  Matrix(size_type row_size = 0, size_type column_size = 0)
      : data_(row_size, std::vector<T>(column_size)),
        column_size_(column_size) {
    LOG << "Normal constructor called with "
        << row_size << " row(s) and " << column_size << " column(s)";
  }

  // NOTE(clangpp): Copy constructor usually declares parameter type as
  // `const Type&`.
  // NOTE(clangpp): In class Matrix<T> scope, Matrix is the same as Matrix<T>.
  Matrix(const Matrix& other)
      : data_(other.data_), column_size_(other.column_size_) {
    LOG << "Copy constructor called";
  }

  // NOTE(clangpp): Move constructor has to declare parameter type as `Type&&`.
  Matrix(Matrix&& other)
      : data_(std::move(other.data_)), column_size_(other.column_size_) {
    LOG << "Move constructor called";
  }

  // ==== Assignment operator ====

  Matrix& operator = (const Matrix& other) {
    data_ = other.data_;
    column_size_ = other.column_size_;
    LOG << "Copy assignment called";
    return *this;
  }

  Matrix& operator = (Matrix&& other) {
    data_ = std::move(other.data_);
    column_size_ = other.column_size_;
    LOG << "Move assignment called";
    return *this;
  }

  // ==== Observers ====

  size_type row_size() const { return data_.size(); }
  size_type column_size() const { return column_size_; }
  std::pair<size_type, size_type> size() const {
    return std::make_pair(row_size(), column_size());
  }
  bool empty() const { return row_size() == 0 || column_size() == 0; }

  // Checks if this is a square matrix.
  bool square() const { return !empty() && (row_size()==column_size()); }

  reference operator () (size_type row, size_type column) {
    check_indices(row, column);
    return data_[row][column];
  }
  const_reference operator () (size_type row, size_type column) const {
    check_indices(row, column);
    return data_[row][column];
  }

  // ==== Calculation operators ====
  // NOTE(clangpp): When overloading operators, try to keep them consistant with
  // their original meaning and function. Or your user would be confused, which
  // would make the benefit of operator overloading cannot beat the
  // understanding cost.

  // NOTE(clangpp): Declares parameter type as `const Type&` to disallow
  // modification on argument.
  Matrix& operator += (const Matrix& other) {
    check_dimension_matches(other);
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] += other.data_[i][j];
      }
    }
    LOG << "Inplace plus (+=) called";
    return *this;
  }

  // NOTE(clangpp): Declares parameter type as `const Type&` to disallow
  // modification on argument.
  // NOTE(clangpp): Adds `const` after function to disallow modification on
  // `this`.
  // NOTE(clangpp): Has to declare return type to `Type` (value form), can't use
  // `Type*` (reference form) or `Type&` (pointer form). Matrix copy is a cost
  // we must pay for now, while RVO or r-value reference will help us bypass it.
  Matrix operator + (const Matrix& other) const {
    check_dimension_matches(other);
    Matrix result(*this);
    result += other;
    LOG << "Plus (const Matrix& + const Matrix&) called";
    return result;
  }

  // NOTE(clangpp): Normally this form is bad, because it's not consistant with
  // original form of plus, which won't modify its operands. I write it here to
  // show the usage of l-value reference and r-value reference.
  // NOTE(clangpp): There will be two forms, `const Matrix& + Matrix&&` is here,
  // `Matrix&& + const Matrix&` will be implemented by non-member function.
  Matrix operator + (Matrix&& other) const {
    check_dimension_matches(other);
    // NOTE(clangpp): rvalue reference `other` has a name now, so following code
    // won't treat it as rvalue reference any more. Wrap with std::move() to
    // make other code treat it as rvalue reference.
    Matrix result(std::move(other));
    result += *this;
    LOG << "Plus (const Matrix& + Matrix&&) called";
    return result;
  }
  friend Matrix operator + (Matrix&& lhs, const Matrix& rhs) {
    lhs.check_dimension_matches(rhs);
    Matrix result(std::move(lhs));
    result += rhs;
    LOG << "Plus (Matrix&& + const Matrix&) called";
    return result;
  }

  // ==== Stream operators ====

  friend std::ostream& operator << (std::ostream& os, const Matrix& matrix) {
    for (const auto& row : matrix.data_) {
      for (const auto& element : row) {
        os << element << " ";
      }
      os << std::endl;
    }
    return os;
  }

 private:
  void check_indices(size_type row, size_type column) const {
    if (row >= row_size() || column >= column_size()) {
      std::stringstream ss;
      ss << "Matrix indices (" << row << ", " << column << ") out of range";
      throw std::out_of_range(ss.str());
    }
  }
  void check_dimension_matches(const Matrix& other) const {
    if (row_size() != other.row_size() ||
        column_size() != other.column_size()) {
      std::stringstream ss;
      ss << "Matrices dimensions don't match: "
         << "(" << row_size() << ", " << column_size() << ") vs "
         << "(" << other.row_size() << ", " << other.column_size() << ")";
      throw std::runtime_error(ss.str());
    }
  }

 private:
  std::vector<std::vector<T>> data_;
  size_type column_size_;  // Stores column size in case data_.empty()
};

#endif  // MATRIX_H_
