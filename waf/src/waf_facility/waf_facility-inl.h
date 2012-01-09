// waf_facility-inl.h
#ifndef WAF_FACILITY_INL_H_
#define WAF_FACILITY_INL_H_

#include "waf_facility.h"

namespace waf {

// Cell compare by value
// =============================================================================
template <typename T>
bool cell_value_greater(
        const serialization::sparse_matrix::Cell<T>& lhs,
        const serialization::sparse_matrix::Cell<T>& rhs) {

    if (lhs.value>rhs.value || rhs.value>lhs.value) {  // value (greater)
        return lhs.value > rhs.value;
    } else if (lhs.row != rhs.row) {  // row (less)
        return lhs.row < rhs.row;
    } else {  // column (less)
        return lhs.column < rhs.column;
    }
}

}  // namespace waf

#endif  // WAF_FACILITY_INL_H_
