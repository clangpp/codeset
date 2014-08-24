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

void CheckSizeEqual(std::size_t size1, std::size_t size2) {
  if (!(size1 == size2)) {
    std::stringstream ss;
    ss << "`size1 == size2` expected, actual " << size1 << " and " << size2;
    throw std::runtime_error(ss.str());
  }
}

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

// NOTE(clangpp): absolute_less and is_zero are provided for user to pass
// special rules for value type, e.g. is_zero(double v) -> std::abs(v) < 1e-6;
template <typename T>
void GaussJordanEliminate(
    Matrix<T>* coefficient_matrix,
    Matrix<T>* constant_matrix = nullptr,
    std::function<bool(const T&, const T&)> absolute_less =
        [](const T& lhs, const T& rhs) {
          return std::abs(lhs) < std::abs(rhs);
        },
    std::function<bool(const T&)> is_zero =
        [](const T& value) { return value == 0; }) {
  if (constant_matrix) {
    CheckSizeEqual(coefficient_matrix->row_size(),
                   constant_matrix->row_size());
  }

  // Helpers
  auto a_mat = coefficient_matrix;  // shorter name
  auto b_mat = constant_matrix;  // shorter name
  typedef typename Matrix<T>::size_type size_type;
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

    // Concurrently eliminates all rows (except `max_row`) in column `pivot`.
    for (size_type row = 0; row < a_mat->row_size(); ++row) {
      futures[row] = std::async(
          std::launch::async,
          [a_mat, b_mat, pivot, max_row, row]() {
            if (row != max_row) {
              T factor = -(*a_mat)[row][pivot] / (*a_mat)[max_row][pivot];
              a_mat->elementary_row_add(row, max_row, factor);
              if (b_mat) {
                b_mat->elementary_row_add(row, max_row, factor);
              }
            }
          });
    }
    for (size_type row = 0; row < a_mat->row_size(); ++row) {
      futures[row].wait();
    }

    // Normalizes `max_row` and switches to `pivot` row.
    T factor = 1 / (*a_mat)[max_row][pivot];
    a_mat->elementary_row_multiply(max_row, factor, is_zero);
    a_mat->elementary_row_switch(pivot, max_row);
    if (b_mat) {
      b_mat->elementary_row_multiply(max_row, factor, is_zero);
      b_mat->elementary_row_switch(pivot, max_row);
    }
  }
}

}  // namespace math

#endif  // MATH_H_
