#ifndef MATRIX_H_
#define MATRIX_H_

// Author: clangpp@gmail.com
//
// Intention:
// C++11 r-value reference is already released and well supported, it's time to
// write a matrix class to take advantage of that, to perform space-cheaper
// matrix arithmetics.

#include <cstddef>  // size_t
#include <sstream>
#include <utility>  // std::move()
#include <vector>

#include <iostream>  // debug

template <typename T>
class Matrix {
 public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef std::size_t size_type;

 public:
  // ==== Constructors and (default) Destructor ====

  Matrix(size_type row_size, size_type column_size,
                  const value_type& filled = value_type())
    : data_(row_size), std::vector<value_type>(column_size, filled) {
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
    return make_pair(row_size(), column_size());
  }

  bool empty() const { return row_size() == 0 || column_size() == 0; }

  reference operator () (size_type row, size_type column) {
    check_indices_range(row, column);
    return data_[row][column];
  }

  const_reference operator () (size_type row, size_type column) const {
    check_indices_range(row, column);
    return data_[row][column];
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

  // ==== Arithmetric operators ====

  Matrix& operator += (const Matrix& other) {
    check_dimension_matches(other);
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] += other.data_[i][j];
      }
    }
    return *this;
  }

  Matrix& operator -= (const Matrix& other) {
    check_dimension_matches(other);
    for (size_type i=0; i<row_size(); ++i) {
      for (size_type j=0; j<column_size(); ++j) {
        data_[i][j] -= other.data_[i][j];
      }
    }
    return *this;
  }

  friend Matrix<T> operator - (const Matrix<T>& lhs, Matrix<T>&& rhs) {
    lhs.check_dimension_matches(rhs);
    Matrix<T> result(std::move(rhs));
    for (size_type i=0; i<result.row_size(); ++i) {
      for (size_type j=0; j<result.column_size(); ++j) {
        result.data_[i][j] = lhs.data_[i][j] - result.data_[i][j];
      }
    }
    return result;
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

 private:
  void check_indices_range(size_type row, size_type column) const {
    if (row >= row_size() || column >= column_size()) {
      std::stringstream ss;
      ss << "Matrix indices (" << row << ", " << column << ") out of range";
      throw std::out_of_range(ss.str());
    }
  }

 private:
  std::vector<std::vector<value_type>> data_;
  size_type column_size_;
};

// ==== Arithmetric operators ====

template <typename T>
Matrix<T> operator + (const Matrix<T>& lhs, const Matrix<T>& rhs) {
  lhs.check_dimension_matches(rhs);
  Matrix<T> result(lhs);
  result += rhs;
  return result;
}

template <typename T>
Matrix<T> operator + (const Matrix<T>& lhs, Matrix<T>&& rhs) {
  lhs.check_dimension_matches(rhs);
  Matrix<T> result(std::move(rhs));
  result += lhs;
  return result;
}

template <typename T>
Matrix<T> operator + (Matrix<T>&& lhs, const Matrix<T>& rhs) {
  return rhs + std::move(lhs);
}

template <typename T>
Matrix<T> operator - (const Matrix<T>& lhs, const Matrix<T>& rhs) {
  lhs.check_dimension_matches(rhs);
  Matrix<T> result(lhs);
  result -= rhs;
  return result;
}

template <typename T>
Matrix<T> operator - (Matrix<T>&& lhs, const Matrix<T>& rhs) {
  lhs.check_dimension_matches(rhs);
  Matrix<T> result(std::move(lhs));
  result -= rhs;
  return result;
}

template <typename T>
Matrix<T> operator * (const Matrix<T>& mat, const T& factor) {
  Matrix<T> result(mat);
  result *= factor;
  return result;
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
  result /= factor;
  return result;
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
  result %= factor;
  return result;
}

template <typename T>
Matrix<T> operator % (Matrix<T>&& mat, const T& factor) {
  Matrix<T> result(std::move(mat));
  result %= factor;
  return result;
}

#endif  // MATRIX_H_
