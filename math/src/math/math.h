#ifndef MATH_H_
#define MATH_H_

#include <algorithm>
#include <cmath>
#include <future>
#include <sstream>
#include <utility>
#include <vector>

#include "../../../matrix/src/matrix/matrix.h"

namespace math {

void CheckLess(size_t lhs, size_t rhs) {
  if (!(lhs < rhs)) {
    std::stringstream ss;
    ss << "`lhs < rhs` expected, actual " << lhs << " and " << rhs;
    throw std::runtime_error(ss.str());
  }
}

template <typename T>
struct AbsoluteValue {
  T operator()(const T& value) const { return std::abs(value); }
};

// abs: `abs(value1) < abs(value2)` should be satified.
template <typename T, typename UnaryFunction = AbsoluteValue<T>>
void GaussJordanEliminate(Matrix<T>* augmented_matrix,
                          UnaryFunction abs = UnaryFunction()) {
  // Augmented matrix has at least one column.
  CheckLess(0, augmented_matrix->column_size());

  // Helpers
  auto mat = augmented_matrix;  // shorter name
  typedef typename Matrix<T>::size_type size_type;
  std::vector<std::future<void>> futures(mat->row_size());

  // Augmented matrix represents exactly max->column_size()-1 variables.
  size_type dimension = std::min(mat->row_size(), mat->column_size()-1);
  for (size_type pivot = 0; pivot < dimension; ++pivot) {
    // Finds (abs) max element of current column.
    auto max_iter = std::max_element(
        mat->column_begin(pivot) + pivot, mat->column_end(pivot),
        [&abs](const T& lhs, const T& rhs) {
          return abs(lhs) < abs(rhs);
        });
    size_type max_row = max_iter - mat->column_begin(pivot);
    if ((*mat)[max_row][pivot] == 0) {
      continue;
    }

    // Concurrently eliminates all rows (except `max_row`) in column `pivot`.
    for (size_type row = 0; row < mat->row_size(); ++row) {
      futures[row] = std::async(
          std::launch::async,
          [mat, pivot, max_row, row]() {
            if (row != max_row) {
              mat->elementary_row_add(
                  row, max_row, -(*mat)[row][pivot]/(*mat)[max_row][pivot]);
            }
          });
    }
    for (size_type row = 0; row < mat->row_size(); ++row) {
      futures[row].wait();
    }

    // Normalizes `max_row` and switches to `pivot` row.
    mat->elementary_row_multiply(max_row, 1 / (*mat)[max_row][pivot]);
    mat->elementary_row_switch(pivot, max_row);
  }
}

}  // namespace math

#endif  // MATH_H_
