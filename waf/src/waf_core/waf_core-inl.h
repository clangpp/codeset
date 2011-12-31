// waf_core-inl.h
#ifndef WAF_CORE_INL_H_
#define WAF_CORE_INL_H_

#include "waf_core.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <sstream>
#include <stdexcept>

#include "../serialization/serialization.h"

namespace waf {

// waf kernel function
// =============================================================================
template <typename InputIterator,
         typename Predicate, typename RandomAccessIterator>
void term_frequency(
        InputIterator termid_first, InputIterator termid_last,
		Predicate pred, RandomAccessIterator freq_first) {

	for (; termid_first!=termid_last; ++termid_first) {
		termid_type termid = static_cast<termid_type>(*termid_first);
		if (termid!=delim_termid && pred(termid))
			++ *(freq_first+termid);
	}
}

template <typename ForwardIterator,
         typename Predicate1, typename Predicate2>
void co_occurrence(
        ForwardIterator termid_first, ForwardIterator termid_last,
		Predicate1 pred_left, Predicate2 pred_right,
        size_type co_win, sparse_matrix<cooccur_type>& co_mat) {

	for (size_type distance=1; distance<co_win; ++distance) {

		// prepare two iterator
		size_type delim_counter=0;
		std::deque<termid_type> q_win;
		ForwardIterator left(termid_first), right(left);
		for (size_type i=0; i<distance && right!=termid_last; ++i,++right) {
			termid_type termid = static_cast<termid_type>(*right);
			delim_counter += (delim_termid==termid);
			q_win.push_back(termid);
		}

		// scan term range, culculate co-occurrence with distance
		for (; right!=termid_last; ++left, ++right) {
			termid_type left_termid = static_cast<termid_type>(*left);
			termid_type right_termid = static_cast<termid_type>(*right);

			delim_counter += (delim_termid==right_termid);
			q_win.pop_front();  // pop *left out of window, while *right not in yet

			if (0==delim_counter &&
                    pred_left(left_termid) && pred_right(right_termid)) {

                // TBD: to be optimized
				// count co-occurrence only when neither *left nor *right in window
				if (find(q_win.begin(), q_win.end(), left_termid)==q_win.end() &&
						find(q_win.begin(), q_win.end(), right_termid)==q_win.end()) {
					sparse_matrix<cooccur_type>::value_type& co=co_mat.iat(left_termid, right_termid);
					co.first+=distance; ++co.second;
				}
			}

			q_win.push_back(*right);  // push *right into window
			delim_counter -= (delim_termid==left_termid);
		}
	}
}

template <typename ForwardIterator>
void mean_distance(ForwardIterator co_first, ForwardIterator co_last) {
	for (; co_first!=co_last; ++co_first)
		co_first->first /= co_first->second;
}

template <typename ForwardIterator>
void total_distance(ForwardIterator co_first, ForwardIterator co_last) {
	for (; co_first!=co_last; ++co_first)
		co_first->first *= co_first->second;
}

inline force_type word_activation_force(
        const cooccur_type& co_info, size_type left_freq, size_type right_freq) {
    force_type f_ij = static_cast<force_type>( co_info.second );
    force_type f_i = static_cast<force_type>( left_freq );
    force_type f_j = static_cast<force_type>( right_freq );
    distance_type d_ij_mean = co_info.first;
    force_type waf = f_ij * f_ij / (f_i * f_j * d_ij_mean * d_ij_mean);
	return waf;
}

template <typename InputIterator, typename Predicate1,
         typename Predicate2, typename UnaryFunction>
affinity_type affinity_or_mean(
        InputIterator first1, InputIterator last1, Predicate1 pred1,
        InputIterator first2, InputIterator last2, Predicate2 pred2,
        UnaryFunction iter_termid, affinity_type or_nolink) {

    affinity_type or_sum = 0;
    size_type or_count = 0;
	while (first1!=last1 || first2!=last2) {
		while (first1!=last1 && !pred1(iter_termid(first1))) ++first1;
		while (first2!=last2 && !pred2(iter_termid(first2))) ++first2;
        if (first1==last1 && first2==last2) {  // both reach end
            break;
        } else if (first1==last1 && first2!=last2) {  // "left" side reach end
            ++first2;
        } else if (first1!=last1 && first2==last2) {  // "right" side reach end
            ++first1;
        } else {  // neither side reach end
			force_type waf1 = *first1++, waf2 = *first2++;
			or_sum += std::min(waf1,waf2) / std::max(waf1,waf2);
        }
        ++or_count;
	}
    return or_count>0 ? or_sum/or_count : or_nolink;
}

template <typename Predicate1, typename Predicate2>
affinity_type affinity_measure(
		const cross_list<force_type>& waf_mat1, termid_type i1, Predicate1 pred1, 
		const cross_list<force_type>& waf_mat2, termid_type i2, Predicate2 pred2,
        affinity_type affinity_nolink) {

	// calculate k_mean (scan col_i and col_j)
    affinity_type k_mean = affinity_or_mean(
            waf_mat1.cbegin_of_col(i1),waf_mat1.cend_of_col(i1), pred1,
            waf_mat2.cbegin_of_col(i2),waf_mat2.cend_of_col(i2), pred2,
            std::mem_fun_ref(
                &cross_list<force_type>::const_col_iterator::row_index), -1);
    if (k_mean<=0 && k_mean>-0.01) return 0;  // k_mean == 0

	// calculate l_mean (scan row_i and row_j)
    affinity_type l_mean = affinity_or_mean(
            waf_mat1.cbegin_of_row(i1),waf_mat1.cend_of_row(i1), pred1,
            waf_mat2.cbegin_of_row(i2),waf_mat2.cend_of_row(i2), pred2,
            std::mem_fun_ref(
                &cross_list<force_type>::const_row_iterator::col_index), -1);
    if (l_mean<=0 && l_mean>-0.01) return 0;  // l_mean == 0

    // there are neither in-link nor out-link
    if (k_mean<0 && l_mean<0) return affinity_nolink;

	// calculate affinity measure
	return std::sqrt(k_mean*l_mean);
}


// waf matrix interface
// =============================================================================
template <typename Predicate1, typename Predicate2, typename UnaryFunction>
void word_activation_force(
        const cross_list<cooccur_type>& co_mat, 
        Predicate1 pred_left, Predicate2 pred_right,
        UnaryFunction term_freq, force_type prec, cross_list<force_type>& waf_mat) {

	waf_mat.clear();
    size_type term_size = std::max(co_mat.row_size(), co_mat.col_size());
    waf_mat.resize(term_size, term_size);

    cross_list<cooccur_type>::const_iterator co_iter = co_mat.cbegin();
    cross_list<cooccur_type>::const_iterator co_end = co_mat.cend();
    for (; co_iter!=co_end; ++co_iter) {

		// verify there is at least one term(id) concerned
		termid_type i = co_iter.row_index(), j = co_iter.col_index();
		if (!pred_left(i) && !pred_right(j)) continue;

		// calculate waf from term i to term j which the element denotes
        force_type waf = word_activation_force(*co_iter, term_freq(i), term_freq(j));

		// check precision and store waf into waf_mat
		if (waf >= prec) waf_mat.rset(i, j, waf);
    }
}

template <typename Predicate1, typename Predicate2, typename UnaryFunction>
void word_activation_force(
        std::istream& co_mat_is, Predicate1 pred_left, Predicate2 pred_right,
        UnaryFunction term_freq, force_type prec, std::ostream& waf_mat_os) {

    typedef serialization::sparse_matrix::Cell<force_type> waf_cell_type;
    serialization::sparse_matrix::Cell<cooccur_type> co_cell;
    serialization::sparse_matrix::Dimension dimension;
    size_type curr_line(-1), term_size(0);
    char beg_ch = 0;
    while (co_mat_is >> beg_ch) {
        co_mat_is.putback(beg_ch);

        force_type waf = 0;
        bool unknown_pattern = false;
        switch (beg_ch) {
        case '(':  // cell
            co_mat_is >> co_cell;
            if (pred_left(co_cell.row) || pred_right(co_cell.column)) {

                // calculate waf value
                force_type waf = word_activation_force(co_cell.value,
                        term_freq(co_cell.row), term_freq(co_cell.column));
                if (waf < prec) break;

                // perform line separating
                if (-1 == curr_line) {
                    curr_line = co_cell.row;
                } else if (co_cell.row != curr_line) {
                    curr_line = co_cell.row;
                    waf_mat_os << std::endl;
                }

                // write waf cell to ostream
                waf_mat_os << waf_cell_type(
                        co_cell.row, co_cell.column, co_cell.value);
            }
            break;
        case '[':  // dimension
            co_mat_is >> dimension;
            term_size = std::max(dimension.row, dimension.column);
            dimension.row = dimension.column = term_size;
            waf_mat_os << dimension << std::endl;
            break;
        default:
            unknown_pattern = true;
            break;
        }
        if (unknown_pattern) break;  // unknown pattern, break loop
    }
}

template <typename Predicate>
void affinity_measure(
        const cross_list<force_type>& waf_mat, Predicate pred,
        affinity_type prec, affinity_type affinity_nolink,
        cross_list<affinity_type>& a_mat) {

    // argument checking
    if (waf_mat.row_size()!=waf_mat.col_size()) {
        std::stringstream ss;
        ss << "waf::affinity_measure(waf_mat, pred, prec, a_nolink, a_mat):\n";
        ss << "waf_mat row size must be exactly the same as column size, but\n";
        ss << "\twaf_mat: row=" << waf_mat.row_size() << ", column=" << waf_mat.col_size() << "\n";
        throw std::invalid_argument(ss.str());
    }

    a_mat.clear();
	size_type term_size = waf_mat.row_size();
    a_mat.resize(term_size, term_size);

    for (size_type i=0; i<term_size; ++i) {
        if (!pred(i)) continue;

        for (size_type j=i; j<term_size; ++j) {
            if (!pred(j)) continue;
            affinity_type a = affinity_measure(
                    waf_mat, i, pred, waf_mat, j, pred, affinity_nolink);
            if (0<=a && a<prec) continue;
            a_mat.rset(i, j, a);
            a_mat.rset(j, i, a);
        }
    }
}

template <typename Predicate>
void affinity_measure(const cross_list<force_type>& waf_mat, Predicate pred,
        affinity_type prec, affinity_type affinity_nolink, std::ostream& a_mat_os) {

    // argument checking
    if (waf_mat.row_size()!=waf_mat.col_size()) {
        std::stringstream ss;
        ss << "waf::affinity_measure(waf_mat, pred, prec, a_nolink, a_mat_os):\n";
        ss << "waf_mat row size must be exactly the same as column size, but\n";
        ss << "\twaf_mat: row=" << waf_mat.row_size() << ", column=" << waf_mat.col_size() << "\n";
        throw std::invalid_argument(ss.str());
    }

    typedef serialization::sparse_matrix::Cell<affinity_type> a_cell_type;
    typedef serialization::sparse_matrix::Dimension dimension_type;
    cross_list<affinity_type> a_mat;
	size_type term_size = waf_mat.row_size();
    a_mat.resize(term_size, term_size);
    dimension_type dimension(a_mat.row_size(), a_mat.col_size());

    for (size_type i=0; i<term_size; ++i) {
        if (!pred(i)) continue;

        // calculate one row of a_mat
        for (size_type j=i; j<term_size; ++j) {
            if (!pred(j)) continue;
            affinity_type a = affinity_measure(
                    waf_mat, i, pred, waf_mat, j, pred, affinity_nolink);
            if (0<=a && a<prec) continue;
            a_mat.rset(i, j, a);
            a_mat.rset(j, i, a);
        }

        // write one row of a_mat to ostream
        cross_list<affinity_type>::row_iterator row_iter = a_mat.begin_of_row(i);
        cross_list<affinity_type>::row_iterator row_end = a_mat.end_of_row(i);
        for (; row_iter!=row_end; ++row_iter)
            a_mat_os << a_cell_type(i, row_iter.col_index(), *row_iter);

        // erase this row from a_mat
        row_iter = a_mat.begin_of_row(i);
        if (row_iter != row_end) {
            a_mat.erase(row_iter, row_end);
            a_mat_os << std::endl;
        }
    }
    a_mat_os << dimension << std::endl;
}

template <typename Predicate1, typename Predicate2, typename OutputIterator>
void affinity_measure(
        const cross_list<force_type>& waf_mat1, Predicate1 pred1,
        const cross_list<force_type>& waf_mat2, Predicate2 pred2,
        affinity_type affinity_nolink, OutputIterator term_a_iter) {

    // argument checking
    if (waf_mat1.row_size()!=waf_mat1.col_size() ||
            waf_mat2.row_size()!=waf_mat2.col_size() ||
            waf_mat1.row_size()!=waf_mat2.row_size()) {
        std::stringstream ss;
        ss << "waf::affinity_measure(waf_mat1, pred1, waf_mat2, pred2, a_nolink, term_a_iter):\n";
        ss << "all dimensions of waf matrices must be exactly the same, but\n";
        ss << "\twaf_mat1: row=" << waf_mat1.row_size() << ", column=" << waf_mat1.col_size() << "\n";
        ss << "\twaf_mat2: row=" << waf_mat2.row_size() << ", column=" << waf_mat2.col_size() << "\n";
        throw std::invalid_argument(ss.str());
    }

    size_type term_size = waf_mat1.row_size();
    for (termid_type i=0; i<term_size; ++i, ++term_a_iter) {
        *term_a_iter = affinity_measure(
                waf_mat1, i, pred1, waf_mat2, i, pred2, affinity_nolink);
    }
}

}  // namespace waf

#endif  // WAF_CORE_INL_H_
