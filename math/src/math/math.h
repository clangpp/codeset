#ifndef MATH_H_
#define MATH_H_

#include <algorithm>
#include <cmath>
#include <cstddef>  // size_t
#include <functional>
#include <future>
#include <sstream>
#include <utility>
#include <vector>

#include "../../../matrix/src/matrix/matrix.h"

namespace math {

void ConcurrentProcess(std::size_t first, std::size_t last,
                       std::function<void(std::size_t)> process,
                       std::vector<std::future<void>>* helper_futures) {
  if (first >= last) {
    return;
  }
  helper_futures->resize(last - first);
  for (std::size_t index = first; index < last; ++index) {
    (*helper_futures)[index-first] =
        std::async(std::launch::async, process, index);
  }
  for (std::size_t index = first; index < last; ++index) {
    (*helper_futures)[index-first].wait();
  }
}

inline void ConcurrentProcess(std::size_t first, std::size_t last,
                              std::function<void(std::size_t)> process) {
  std::vector<std::future<void>> futures;
  ConcurrentProcess(first, last, process, &futures);
}

template <typename T>
struct TrivialAbsLess {
  bool operator()(const T& lhs, const T& rhs) {
    return std::abs(lhs) < std::abs(rhs);
  }
};

template <typename T>
struct TrivialIsZero {
  bool operator()(const T& value) {
    return value == 0;
  }
};

// absolute_less: Callable, `bool ret = absolute_less(v1, v2);` should be
// is_zero: Callable, `bool ret = is_zero(v);` should be satified.
//  e.g. is_zero(double v) -> std::abs(v) < 1e-6;
template <typename T, typename AbsoluteLess = TrivialAbsLess<T>,
          typename IsZero = TrivialIsZero<T>>
void GaussJordanEliminate(
    Matrix<T>* coefficient_matrix,
    Matrix<T>* extra_matrix = nullptr,
    AbsoluteLess absolute_less = AbsoluteLess(),
    IsZero is_zero = IsZero()) {
  if (extra_matrix) {
    matrix::CheckAugmentable(*coefficient_matrix, *extra_matrix);
  }

  // Helpers
  auto a_mat = coefficient_matrix;  // shorter name
  auto b_mat = extra_matrix;  // shorter name
  typedef typename Matrix<T>::size_type size_type;
  typedef typename Matrix<T>::value_type value_type;
  std::vector<std::future<void>> futures(a_mat->row_size());

  size_type pivot_row = 0, pivot_column = 0;
  for (; pivot_row < a_mat->row_size() &&
      pivot_column < a_mat->column_size(); ++pivot_column) {
    // Finds max (absolute) value of current column.
    auto max_iter = std::max_element(
        a_mat->column_begin(pivot_column) + pivot_row,
        a_mat->column_end(pivot_column),
        absolute_less);
    size_type max_row = max_iter - a_mat->column_begin(pivot_column);
    if (is_zero((*a_mat)[max_row][pivot_column])) {
      continue;
    }

    // Switches `max_row` to `pivot_row`.
    a_mat->elementary_row_switch(pivot_row, max_row);
    if (b_mat) {
      b_mat->elementary_row_switch(pivot_row, max_row);
    }

    // Concurrently eliminates `pivot_column`'s coefficients (in all rows
    // except `pivot_row`).
    ConcurrentProcess(
        0, a_mat->row_size(),
        [a_mat, b_mat, pivot_row, pivot_column](size_type row) {
          if (row != pivot_row) {
            value_type scaler = -(*a_mat)[row][pivot_column] /
                (*a_mat)[pivot_row][pivot_column];
            a_mat->elementary_row_add(row, pivot_row, scaler);
            if (b_mat) {
              b_mat->elementary_row_add(row, pivot_row, scaler);
            }
          }
        }, &futures);

    // Normalizes row `pivot`.
    value_type scaler = 1 / (*a_mat)[pivot_row][pivot_column];
    a_mat->elementary_row_multiply(pivot_row, scaler, is_zero);
    if (b_mat) {
      b_mat->elementary_row_multiply(pivot_row, scaler, is_zero);
    }

    ++pivot_row;
  }
}

// absolute_less: Callable, `bool ret = absolute_less(v1, v2);` should be
// is_zero: Callable, `bool ret = is_zero(v);` should be satified.
//  e.g. is_zero(double v) -> std::abs(v) < 1e-6;
// returns: rank of coefficient_matrix.
template <typename T, typename AbsoluteLess = TrivialAbsLess<T>,
          typename IsZero = TrivialIsZero<T>>
typename Matrix<T>::size_type GaussEliminate(
    Matrix<T>* coefficient_matrix,
    Matrix<T>* extra_matrix = nullptr,
    AbsoluteLess absolute_less = AbsoluteLess(),
    IsZero is_zero = IsZero()) {
  if (extra_matrix) {
    matrix::CheckAugmentable(*coefficient_matrix, *extra_matrix);
  }

  // Helpers
  Matrix<T>* a_mat = coefficient_matrix;  // shorter name
  Matrix<T>* b_mat = extra_matrix;  // shorter name
  typedef typename Matrix<T>::size_type size_type;
  typedef typename Matrix<T>::value_type value_type;
  std::vector<std::future<void>> futures(a_mat->row_size());

  size_type pivot_row = 0, pivot_column = 0;
  for (; pivot_row < a_mat->row_size() &&
      pivot_column < a_mat->column_size(); ++pivot_column) {
    // Finds max (absolute) value of current column.
    auto max_iter = std::max_element(
        a_mat->column_begin(pivot_column) + pivot_row,
        a_mat->column_end(pivot_column),
        absolute_less);
    size_type max_row = max_iter - a_mat->column_begin(pivot_column);
    if (is_zero((*a_mat)[max_row][pivot_column])) {
      continue;
    }

    // Switches `max_row` to `pivot_row`.
    a_mat->elementary_row_switch(pivot_row, max_row);
    if (b_mat) {
      b_mat->elementary_row_switch(pivot_row, max_row);
    }

    // Concurrently eliminates `pivot_column`'s coefficients (in all rows
    // except `pivot_row`).
    ConcurrentProcess(
        pivot_row + 1, a_mat->row_size(),
        [a_mat, b_mat, pivot_row, pivot_column](size_type row) {
          value_type scaler = -(*a_mat)[row][pivot_column] /
              (*a_mat)[pivot_row][pivot_column];
          for (size_type column = pivot_column + 1;
              column < a_mat->column_size(); ++column) {
            (*a_mat)[row][column] += scaler * (*a_mat)[pivot_row][column];
          }
          (*a_mat)[row][pivot_column] = value_type(0);

          if (b_mat) {  // Does the same row operation to b_mat.
            b_mat->elementary_row_add(row, pivot_row, scaler);
          }
        }, &futures);

    ++pivot_row;
  }
  return pivot_row;
}

}  // namespace math

#endif  // MATH_H_
