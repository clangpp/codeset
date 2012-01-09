// waf_core-inl.h
#ifndef WAF_CORE_INL_H_
#define WAF_CORE_INL_H_

#include "waf_core.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <set>
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
		Predicate care, RandomAccessIterator freq_first) {

	for (; termid_first!=termid_last; ++termid_first) {
		termid_type termid = static_cast<termid_type>(*termid_first);
		if (termid!=delim_termid && care(termid))
			++ *(freq_first + termid);
	}
}

namespace internal {

inline void comat_enlarge(
        sparse_matrix<cooccur_type>& co_mat,
        termid_type termid1, termid_type termid2) {

    termid_type max_termid = std::max(termid1, termid2);
    if (co_mat.row_size() <= max_termid) {
        co_mat.resize(max_termid+1, max_termid+1);  // enlarge matrix

        waf::size_type index_dim = co_mat.sparse().first;
        waf::size_type index_hope = 
            static_cast<waf::size_type>(std::sqrt(1.0*co_mat.size()));
        if (index_hope >= 2 * index_dim) {
            co_mat.sparse(index_hope,index_hope);  // sparse matrix
        }
    }
}

}  // namespace internal

template <typename InputIterator,
         typename Predicate1, typename Predicate2>
void co_occurrence(
        InputIterator termid_first, InputIterator termid_last,
		Predicate1 care_left, Predicate2 care_right,
        size_type co_win, sparse_matrix<cooccur_type>& co_mat) {

    co_mat.clear();
    co_mat.sparse(8, 8);  // no special meaning, just greater than 0

    // initialize term window
    std::deque<termid_type> term_win;
    std::set<termid_type> term_unique;
    for (; termid_first!=termid_last &&
            term_win.size()<co_win; ++termid_first) {
        term_win.push_back(*termid_first);
    }

    // scan term window, and move window
    while (term_win.size() >= 2) {  // at least contains two terms

        // analize co-occurrence in term window (win[0] to rest)
        if (delim_termid!=term_win[0] && care_left(term_win[0])) {

            term_unique.clear();
            term_unique.insert(term_win[0]);
            for (waf::size_type i=1; i<term_win.size(); ++i) {

                if (delim_termid==term_win[i]) break;
                if (!care_right(term_win[i])) continue;

                if (term_win[0] == term_win[i]) {
                    internal::comat_enlarge(co_mat, term_win[0], term_win[i]);
                    waf::cooccur_type& co = co_mat.iat(term_win[0], term_win[i]);
                    co.first+=i, ++co.second;
                    break;
                }

                if (term_unique.count(term_win[i])) continue;

                term_unique.insert(term_win[i]);
                internal::comat_enlarge(co_mat, term_win[0], term_win[i]);
                waf::cooccur_type& co = co_mat.iat(term_win[0], term_win[i]);
                co.first+=i, ++co.second;
            }
        }

        // right move term window by one position
        term_win.pop_front();
        if (termid_first != termid_last)
            term_win.push_back(*termid_first++);
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
        InputIterator first1, InputIterator last1, Predicate1 care1,
        InputIterator first2, InputIterator last2, Predicate2 care2,
        UnaryFunction iter_termid, affinity_type or_nolink) {

    affinity_type or_sum = 0;
    size_type or_count = 0;
	while (first1!=last1 || first2!=last2) {
		while (first1!=last1 && !care1(iter_termid(first1))) ++first1;
		while (first2!=last2 && !care2(iter_termid(first2))) ++first2;
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
		const cross_list<force_type>& waf_mat1, termid_type i1, Predicate1 back1, 
		const cross_list<force_type>& waf_mat2, termid_type i2, Predicate2 back2,
        affinity_type affinity_nolink) {

	// calculate k_mean (scan col_i and col_j)
    affinity_type k_mean = affinity_or_mean(
            waf_mat1.cbegin_of_col(i1),waf_mat1.cend_of_col(i1), back1,
            waf_mat2.cbegin_of_col(i2),waf_mat2.cend_of_col(i2), back2,
            std::mem_fun_ref(
                &cross_list<force_type>::const_col_iterator::row_index), -1);
    if (k_mean<=0 && k_mean>-0.01) return 0;  // k_mean == 0

	// calculate l_mean (scan row_i and row_j)
    affinity_type l_mean = affinity_or_mean(
            waf_mat1.cbegin_of_row(i1),waf_mat1.cend_of_row(i1), back1,
            waf_mat2.cbegin_of_row(i2),waf_mat2.cend_of_row(i2), back2,
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
        Predicate1 care_left, Predicate2 care_right,
        UnaryFunction term_freq, force_type prec, cross_list<force_type>& waf_mat) {

	waf_mat.clear();
    size_type term_size = std::max(co_mat.row_size(), co_mat.col_size());
    waf_mat.resize(term_size, term_size);

    cross_list<cooccur_type>::const_iterator co_iter = co_mat.cbegin();
    cross_list<cooccur_type>::const_iterator co_end = co_mat.cend();
    for (; co_iter!=co_end; ++co_iter) {

		// verify there is at least one term(id) concerned
		termid_type i = co_iter.row_index(), j = co_iter.col_index();
		if (!care_left(i) || !care_right(j)) continue;

		// calculate waf from term i to term j which the element denotes
        force_type waf = word_activation_force(*co_iter, term_freq(i), term_freq(j));

		// check precision and store waf into waf_mat
		if (waf >= prec) waf_mat.rset(i, j, waf);
    }
}

template <typename Predicate1, typename Predicate2, typename UnaryFunction>
void word_activation_force(
        std::istream& co_mat_is, Predicate1 care_left, Predicate2 care_right,
        UnaryFunction term_freq, force_type prec, std::ostream& waf_mat_os) {

    typedef serialization::sparse_matrix::Cell<force_type> waf_cell_type;
    serialization::sparse_matrix::Cell<cooccur_type> co_cell;
    serialization::sparse_matrix::Dimension dimension;
    size_type curr_line(-1), term_size(0);
    char beg_ch = 0;
    while (co_mat_is >> beg_ch) {
        co_mat_is.putback(beg_ch);

        bool unknown_pattern = false;
        switch (beg_ch) {
        case '(':  // cell
            co_mat_is >> co_cell;
            if (care_left(co_cell.row) && care_right(co_cell.column)) {

                // calculate waf value
                force_type waf = word_activation_force(co_cell.value,
                        term_freq(co_cell.row), term_freq(co_cell.column));
                if (waf < prec) break;

                // perform line separating
                if (static_cast<size_type>(-1) == curr_line) {
                    curr_line = co_cell.row;
                } else if (co_cell.row != curr_line) {
                    curr_line = co_cell.row;
                    waf_mat_os << std::endl;
                }

                // write waf cell to ostream
                waf_mat_os << waf_cell_type(co_cell.row, co_cell.column, waf);
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

template <typename Predicate1, typename Predicate2>
void affinity_measure(
        const cross_list<force_type>& waf_mat, Predicate1 care, Predicate2 back,
        affinity_type prec, affinity_type affinity_nolink,
        cross_list<affinity_type>& a_mat) {

    // argument checking
    if (waf_mat.row_size()!=waf_mat.col_size()) {
        std::stringstream ss;
        ss << "waf::affinity_measure(waf_mat, care, back, prec, a_nolink, a_mat):\n";
        ss << "waf_mat row size must be exactly the same as column size, but\n";
        ss << "\twaf_mat: row=" << waf_mat.row_size() << ", column=" << waf_mat.col_size() << "\n";
        throw std::invalid_argument(ss.str());
    }

    a_mat.clear();
	size_type term_size = waf_mat.row_size();
    a_mat.resize(term_size, term_size);

    for (size_type i=0; i<term_size; ++i) {
        if (!care(i)) continue;

        for (size_type j=i; j<term_size; ++j) {
            if (!care(j)) continue;
            affinity_type a = affinity_measure(
                    waf_mat, i, back, waf_mat, j, back, affinity_nolink);
            if (0<=a && a<prec) continue;
            a_mat.rset(i, j, a);
            a_mat.rset(j, i, a);
        }
    }
}

template <typename Predicate1, typename Predicate2>
void affinity_measure(const cross_list<force_type>& waf_mat,
        Predicate1 care, Predicate2 back, affinity_type prec,
        affinity_type affinity_nolink, std::ostream& a_mat_os) {

    // argument checking
    if (waf_mat.row_size()!=waf_mat.col_size()) {
        std::stringstream ss;
        ss << "waf::affinity_measure(waf_mat, care, back, prec, a_nolink, a_mat_os):\n";
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
        if (!care(i)) continue;

        // calculate one row of a_mat
        for (size_type j=i; j<term_size; ++j) {
            if (!care(j)) continue;
            affinity_type a = affinity_measure(
                    waf_mat, i, back, waf_mat, j, back, affinity_nolink);
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
        const cross_list<force_type>& waf_mat1, Predicate1 back1,
        const cross_list<force_type>& waf_mat2, Predicate2 back2,
        affinity_type affinity_nolink, OutputIterator term_a_iter) {

    // argument checking
    if (waf_mat1.row_size()!=waf_mat1.col_size() ||
            waf_mat2.row_size()!=waf_mat2.col_size() ||
            waf_mat1.row_size()!=waf_mat2.row_size()) {
        std::stringstream ss;
        ss << "waf::affinity_measure(waf_mat1, back1, waf_mat2, back2, a_nolink, term_a_iter):\n";
        ss << "all dimensions of waf matrices must be exactly the same, but\n";
        ss << "\twaf_mat1: row=" << waf_mat1.row_size() << ", column=" << waf_mat1.col_size() << "\n";
        ss << "\twaf_mat2: row=" << waf_mat2.row_size() << ", column=" << waf_mat2.col_size() << "\n";
        throw std::invalid_argument(ss.str());
    }

    size_type term_size = waf_mat1.row_size();
    for (termid_type i=0; i<term_size; ++i, ++term_a_iter) {
        *term_a_iter = affinity_measure(
                waf_mat1, i, back1, waf_mat2, i, back2, affinity_nolink);
    }
}

}  // namespace waf

#endif  // WAF_CORE_INL_H_
