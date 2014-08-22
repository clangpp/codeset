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
void GaussJordanEliminate(Matrix<T>* mat,
                          UnaryFunction abs = UnaryFunction()) {
  // Augmented matrix has at least one column.
  CheckLess(0, mat->column_size());
  typedef typename Matrix<T>::size_type size_type;
  std::vector<std::future<void>> futures(mat->row_size());

  // Augmented matrix represents exactly max->column_size()-1 variables.
  size_type dimension = std::min(mat->row_size(), mat->column_size()-1);
  for (size_type sentry = 0; sentry < dimension; ++sentry) {
    // Finds (abs) max element of current column.
    auto max_iter = std::max_element(
        mat->column_begin(sentry) + sentry, mat->column_end(sentry),
        [&abs](const T& lhs, const T& rhs) {
          return abs(lhs) < abs(rhs);
        });
    size_type max_row = max_iter - mat->column_begin(sentry);
    if ((*mat)[max_row][sentry] == 0) {
      continue;
    }

    // Concurrently eliminates all rows (except `max_row`) in column `sentry`.
    for (size_type row = 0; row < mat->row_size(); ++row) {
      futures[row] = std::async(
          std::launch::async,
          [mat, sentry, max_row, row]() {
            if (row != max_row) {
              mat->elementary_row_add(
                  row, max_row, -(*mat)[row][sentry]/(*mat)[max_row][sentry]);
            }
          });
    }
    for (size_type row = 0; row < mat->row_size(); ++row) {
      futures[row].wait();
    }

    // Normalizes `max_row` and switches to `sentry` row.
    mat->elementary_row_multiply(max_row, 1 / (*mat)[max_row][sentry]);
    mat->elementary_row_switch(sentry, max_row);
  }
}

}  // namespace math

#endif  // MATH_H_
