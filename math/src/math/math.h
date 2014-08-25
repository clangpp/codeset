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

  size_type dimension = std::min(a_mat->row_size(), a_mat->column_size());
  for (size_type pivot = 0; pivot < dimension; ++pivot) {
    // Finds max (absolute) value of current column.
    auto max_iter = std::max_element(
        a_mat->column_begin(pivot) + pivot, a_mat->column_end(pivot),
        absolute_less);
    size_type max_row = max_iter - a_mat->column_begin(pivot);
    if (is_zero((*a_mat)[max_row][pivot])) {
      continue;
    }

    // Switches `max_row` to `pivot` row.
    a_mat->elementary_row_switch(pivot, max_row);
    if (b_mat) {
      b_mat->elementary_row_switch(pivot, max_row);
    }

    // Concurrently eliminates column `pivot`'s coefficients (in all rows
    // except row `pivot`).
    ConcurrentProcess(
        0, a_mat->row_size(),
        [a_mat, b_mat, pivot](size_type row) {
          if (row != pivot) {
            value_type factor = -(*a_mat)[row][pivot] / (*a_mat)[pivot][pivot];
            a_mat->elementary_row_add(row, pivot, factor);
            if (b_mat) {
              b_mat->elementary_row_add(row, pivot, factor);
            }
          }
        });

    // Normalizes row `pivot`.
    value_type factor = 1 / (*a_mat)[pivot][pivot];
    a_mat->elementary_row_multiply(pivot, factor, is_zero);
    if (b_mat) {
      b_mat->elementary_row_multiply(pivot, factor, is_zero);
    }
  }
}

// absolute_less: Callable, `bool ret = absolute_less(v1, v2);` should be
// is_zero: Callable, `bool ret = is_zero(v);` should be satified.
//  e.g. is_zero(double v) -> std::abs(v) < 1e-6;
// returns: rank of coefficient_matrix.
template <typename T, typename AbsoluteLess = TrivialAbsLess<T>,
          typename IsZero = TrivialIsZero<T>>
std::size_t GaussEliminate(
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

  std::size_t rank = 0;
  size_type dimension = std::min(a_mat->row_size(), a_mat->column_size());
  for (size_type pivot = 0; pivot < dimension; ++pivot) {
    // Finds max (absolute) value of current column.
    auto max_iter = std::max_element(
        a_mat->column_begin(pivot) + pivot, a_mat->column_end(pivot),
        absolute_less);
    size_type max_row = max_iter - a_mat->column_begin(pivot);
    if (is_zero((*a_mat)[max_row][pivot])) {
      continue;
    }

    // Switches `max_row` to `pivot` row.
    a_mat->elementary_row_switch(pivot, max_row);
    if (b_mat) {
      b_mat->elementary_row_switch(pivot, max_row);
    }

    // Concurrently eliminates column `pivot`'s coefficients (in all rows
    // except row `pivot`).
    ConcurrentProcess(
        pivot + 1, a_mat->row_size(),
        [&a_mat, &b_mat, pivot](size_type row) {
          value_type factor = -(*a_mat)[row][pivot] / (*a_mat)[pivot][pivot];
          for (size_type column = pivot + 1;
              column < a_mat->column_size(); ++column) {
            (*a_mat)[row][column] += factor * (*a_mat)[pivot][column];
          }
          (*a_mat)[row][pivot] = value_type(0);

          if (b_mat) {  // Does the same row operation to b_mat.
            b_mat->elementary_row_add(row, pivot, factor);
          }
        }, &futures);

    // Updates matrix meta-information.
    ++rank;
  }
  return rank;
}

}  // namespace math

#endif  // MATH_H_
