// waf_core.h
#ifndef WAF_CORE_H_
#define WAF_CORE_H_

#include <cstddef>
#include <istream>
#include <ostream>

#include "../cross_list/cross_list.h"
#include "../sparse_matrix/sparse_matrix.h"

namespace waf {

// waf data type
// =============================================================================
typedef std::size_t size_type;  // type of non-negative counters
typedef size_type termid_type;  // type of termid
typedef double distance_type;  // type of term (mean) distance
typedef double force_type;  // type of Word Activation Force
typedef double affinity_type;  // type of affinity measure between two terms
typedef std::pair<distance_type, size_type> cooccur_type;  // (mean-distance, count) pair

// termid for delimeter
const termid_type delim_termid= static_cast<termid_type>(-1);
const affinity_type null_affinity = static_cast<affinity_type>(-1);


// waf kernel function
// =============================================================================
// pre-condition: *(freq_iter + *max_element(termid_first, termid_last)) is accessable
template <typename InputIterator,
         typename Predicate, typename RandomAccessIterator>
void term_frequency(
        InputIterator termid_first, InputIterator termid_last,
		Predicate care, RandomAccessIterator freq_iter);

// append co-occurrence information of [termid_first, termid_last) onto co_mat
// element in co_mat is (total-distance, count) pair
// pre-condition: *max_element(termid_first, termid_last) < co_mat.row_size()
// pre-condition: *max_element(termid_first, termid_last) < co_mat.col_size()
template <typename InputIterator,
         typename Predicate1, typename Predicate2>
void co_occurrence(
        InputIterator termid_first, InputIterator termid_last,
		Predicate1 care_left, Predicate2 care_right,
        size_type co_win, sparse_matrix<cooccur_type>& co_mat);

// inplace convert (total-distance, count) to (mean-distance, count)
template <typename ForwardIterator>
void mean_distance(ForwardIterator co_first, ForwardIterator co_last);

// inplace convert (mean-distance, count) to (total-distance, count)
template <typename ForwardIterator>
void total_distance(ForwardIterator co_first, ForwardIterator co_last);

// pre-condition: co_info.first>0 && left_freq>0 && right_freq>0
inline force_type word_activation_force(
        const cooccur_type& co_info, size_type left_freq, size_type right_freq);

template <typename InputIterator, typename Predicate1,
         typename Predicate2, typename UnaryFunction>
affinity_type affinity_or_mean(
        InputIterator first1, InputIterator last1, Predicate1 care1,
        InputIterator first2, InputIterator last2, Predicate2 care2,
        UnaryFunction iter_termid, affinity_type or_nolink = null_affinity);

// pre-condition: waf_mat1.row_size()==waf_mat1.col_size()
// pre-condition: waf_mat2.row_size()==waf_mat2.col_size()
// pre-condition: i1<waf_mat1.row_size() && i2<waf_mat2.row_size()
template <typename Predicate1, typename Predicate2>
affinity_type affinity_measure(
		const cross_list<force_type>& waf_mat1, termid_type i1, Predicate1 back1, 
		const cross_list<force_type>& waf_mat2, termid_type i2, Predicate2 back2,
        affinity_type affinity_nolink = null_affinity);


// waf matrix interface
// =============================================================================
template <typename Predicate1, typename Predicate2, typename UnaryFunction>
void word_activation_force(
        const cross_list<cooccur_type>& co_mat, 
        Predicate1 care_left, Predicate2 care_right,
        UnaryFunction term_freq, force_type prec, cross_list<force_type>& waf_mat);

template <typename Predicate1, typename Predicate2, typename UnaryFunction>
void word_activation_force(
        std::istream& co_mat_is, Predicate1 care_left, Predicate2 care_right,
        UnaryFunction term_freq, force_type prec, std::ostream& waf_mat_os);

template <typename Predicate1, typename Predicate2>
void affinity_measure(
        const cross_list<force_type>& waf_mat, Predicate1 care, Predicate2 back,
        affinity_type prec, affinity_type affinity_nolink,
        cross_list<affinity_type>& a_mat);

template <typename Predicate1, typename Predicate2>
void affinity_measure(const cross_list<force_type>& waf_mat,
        Predicate1 care, Predicate2 back, affinity_type prec,
        affinity_type affinity_nolink, std::ostream& a_mat_os);

template <typename Predicate1, typename Predicate2, typename OutputIterator>
void affinity_measure(
        const cross_list<force_type>& waf_mat1, Predicate1 back1,
        const cross_list<force_type>& waf_mat2, Predicate2 back2,
        affinity_type affinity_nolink, OutputIterator term_a_iter);

template <typename Predicate1, typename Predicate2>
void affinity_measure(
        const cross_list<force_type>& waf_mat1, Predicate1 back1,
        const cross_list<force_type>& waf_mat2, Predicate2 back2,
        affinity_type affinity_nolink, cross_list<affinity_type>& a_mat);

template <typename Predicate1, typename Predicate2>
void affinity_measure(
        const cross_list<force_type>& waf_mat1, Predicate1 back1,
        const cross_list<force_type>& waf_mat2, Predicate2 back2,
        affinity_type affinity_nolink, std::ostream& a_mat_os);

}  // namespace waf

#include "waf_core-inl.h"

#endif  // WAF_CORE_H_
