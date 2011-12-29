//waf.h
/**************************************************************************
module: waf
file(s): cross_list.h, sparse_matrix.h, waf.h
function: provide toolset for Word Activation Force (or WAF) model calculation.

	Word Activation Force is a model for text modeling raised by Prof. Jun Guo,
	Pattern Recognition and Intelligent System Lab., BUPT. It models a text as
	a matrix of WAF measure, which is defined by directed co-occurrence of terms.

	This toolset provides some basic type defines to make code readable, provides
	some data structures to implement concepts in WAF model, and provides some
	algorithm to perform calculations in WAF model.

	Considering large amount data processing, I choose deque rather than vector
	to contain termids. As most matrices in WAF model are sparse matrices, I
	write cross list data structures 'cross_list' and 'sparse_matrix' to 
	represent these matrices.

	Most algorithm are written in an iterator range style, to provide higher
	efficiency and greater flexibility.

author: Yongtian Zhang, yongtianzhang@gmail.com
finish-time: 2011.03.28
last-modified: 2011.06.02

license:
	This code is not under any license, anyone can get it, use it, modified it, redistribute it without notice me. 
	I don't think there is no bug in it, so before you use it, please check and test carefully, I don't promise its quality. ^_^
	If you find any bug, please report it to me, I'll be very grateful to you. Discussion is also welcome.

**************************************************************************/
#ifndef WAF_H
#define WAF_H

#include <cstddef>
#include <map>
#include <deque>
#include <string>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <utility>
#include <numeric>
#include <functional>

#include "../cross_list/cross_list.h"
#include "../sparse_matrix/sparse_matrix.h"

namespace waf {

//----------------------basic types define-------------------------------

//basic types
typedef std::size_t size_type;					//type of non-negative counters
typedef size_type termid_type;					//type of termid
typedef double distance_type;					//type of term distance, choose double for mean calculation
typedef double force_type;						//type of Word Activation Force
typedef double affinity_type;					//type of affinity measure between two terms

//------------------------data structures--------------------------------

//delim_termid denotes delimiters, it is used only with '==' and '!=',
//it need not to be a valid termid, but must be of termid_type.
const termid_type delim_termid=static_cast<termid_type>(-1);

//class repeat_iterator, to random access and get the same value
template <typename T>
class repeat_iterator:
	public std::iterator<std::random_access_iterator_tag,T>
{
protected:
	typedef typename std::iterator<std::random_access_iterator_tag,T> iter_base;
public:
	typedef typename iter_base::iterator_category iterator_category;
	typedef typename iter_base::value_type value_type;
	typedef typename iter_base::difference_type difference_type;
	typedef typename iter_base::pointer pointer;
	typedef typename iter_base::reference reference;
public:
	repeat_iterator(const T& value=T()):m_value(value) {}
	reference operator * () {return m_value;}
	pointer operator -> () {return &m_value;}
	repeat_iterator& operator ++ () {return *this;}
	repeat_iterator& operator ++ (int) {return *this;}
	repeat_iterator& operator -- () {return *this;}
	repeat_iterator& operator -- (int) {return *this;}
	repeat_iterator& operator += (difference_type n) {return *this;}
	repeat_iterator& operator -= (difference_type n) {return *this;}
	repeat_iterator& operator + (difference_type n) {return *this;}
	repeat_iterator& operator - (difference_type n) {return *this;}
	reference operator [] (difference_type n) {return m_value;}
	bool operator == (const repeat_iterator& rhs) const {return this==&rhs;}
	bool operator != (const repeat_iterator& rhs) const {return !(*this==rhs);}
private:
	T m_value;
};

template <typename T>
repeat_iterator<T> repeater(const T& value) {return repeat_iterator<T>(value);}

//--------------------------algorithm------------------------------------

//function: calculate max termid in termid range [termid_first,termid_last)
//	with *(identity_vec_beg+termid)!=false
//parameter: termid_first, termid_last: [termid_first, termid_last) denotes a range of termids
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//return: max termid in range [termid_first,termid_last) with *(identity_vec_beg+termid)!=false
//	and termid!=delim_termid
template <typename InputIteraotr, typename RandomAccessIterator>
termid_type max_termid(InputIteraotr termid_first, InputIteraotr termid_last,
		RandomAccessIterator identity_vec_beg)
{
	termid_type max_termid=0;
	for (; termid_first!=termid_last; ++termid_first)//search for max termid
	{
		termid_type termid=static_cast<termid_type>(*termid_first);
		if (delim_termid!=termid && *(identity_vec_beg+termid) && termid>max_termid)
			max_termid = termid;
	}
	return max_termid;
}

//function: calculate max termid in termid range [termid_first,termid_last)
template <typename InputIteraotr>
termid_type max_termid(InputIteraotr termid_first, InputIteraotr termid_last)
{
	return max_termid(termid_first,termid_last,repeater(true));
}

//function: calculate max termid in termid range [termid_first,termid_last)
//	with *(identity_vec_beg+termid)!=false
//parameter: termid_first, termid_last: [termid_first, termid_last) denotes a range of termids
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: bg_iden_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is background.
//return: max termid in range [termid_first,termid_last) with *(identity_vec_beg+termid)!=false
//	and termid!=delim_termid
template <typename InputIteraotr,
	typename RandomAccessIterator1, typename RandomAccessIterator2>
termid_type max_termid(InputIteraotr termid_first, InputIteraotr termid_last,
		RandomAccessIterator1 identity_vec_beg, RandomAccessIterator2 bg_iden_vec_beg)
{
	termid_type max_termid=0;
	for (; termid_first!=termid_last; ++termid_first)//search for max termid
	{
		termid_type termid=static_cast<termid_type>(*termid_first);
		if (delim_termid==termid)
			continue;
		if (*(identity_vec_beg+termid) && termid>max_termid)
		{
			max_termid = termid;
			continue;
		}
		if (*(bg_iden_vec_beg+termid) && termid>max_termid)
			max_termid = termid;
	}
	return max_termid;
}

//function: calculate term frequency of concerned termids in range [termid_first,termid_last) 
//	add results into term frequency vector tf_vec represents
//parameter: termid_first,termid_last: [termid_first,termid_last) denotes a range of termids
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: tf_vec_beg: an iterator that denotes a vector storing term frequency
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: the vector tf_vec_beg denotes is large enough to contain max valid termid
//postcondition: every termids amount in [termid_first,termid_last) is added to *(tf_vec_beg+termid)
template <typename InputIterator,
	typename RandomAccessIterator1, typename RandomAccessIterator2>
void term_frequency(InputIterator termid_first, InputIterator termid_last,
		RandomAccessIterator1 identity_vec_beg, RandomAccessIterator2 tf_vec_beg)
{
	while (termid_first!=termid_last)
	{
		termid_type termid=static_cast<termid_type>(*termid_first++);
		if (termid!=delim_termid && *(identity_vec_beg+termid))
			++ *(tf_vec_beg + termid);
	}
}

//function: calculate term frequency of termids in range [termid_first,termid_last) 
//	add results into term frequency vector tf_vec represents
//parameter: termid_first,termid_last: [termid_first,termid_last) denotes a range of termids
//parameter: tf_vec_beg: an iterator that denotes a vector storing term frequency
//precondition: tf_vec is large enough to contain max termid
//postcondition: every termids amount in [termid_first,termid_last) is added to *(tf_vec_beg+termid)
template <typename InputIterator, typename RandomAccessIterator>
void term_frequency(InputIterator termid_first, InputIterator termid_last,
		RandomAccessIterator tf_vec_beg)
{
	term_frequency(termid_first,termid_last,repeater(true),tf_vec_beg);
}

//function: calculate co-occurrence of concerned 'termids in
//	range [termid_first,termid_last) and put result into co_mat
//parameter: termid_first,termid_last: [termid_first,termid_last) denotes a range of termids
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: co_win: co-occurrence window size, max term distance considered is co_win-1
//parameter: co_mat: a matrix that stores co-occurrence information of termids
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: co_mat is large enough to contain max termid
//postcondition: co-occurrence information of termids in [termid_first, termid_last)
//	with co-occurrence window co_win is stored into co_mat
template <typename ForwardIterator, typename RandomAccessIterator>
void co_occurrence(ForwardIterator termid_first, ForwardIterator termid_last,
		RandomAccessIterator identity_vec_beg, size_type co_win,
		sparse_matrix<std::pair<distance_type,size_type> >& co_mat)
{
	typedef sparse_matrix<std::pair<distance_type,size_type> > matrix_type;
	//co_mat.clear();							//no clear, just append info

	//for every possible term distance
	for (size_type distance=1; distance<co_win; ++distance)
	{
		//prepare two iterator
		size_type delim_counter=0;
		std::deque<termid_type> q_win;
		ForwardIterator left=termid_first, right=left;
		for (size_type i=0; i<distance && right!=termid_last; ++i,++right)
		{
			termid_type termid=static_cast<termid_type>(*right);
			delim_counter += (delim_termid==termid);
			q_win.push_back(termid);
		}

		//scan term range, culculate co-occurrence of distance
		for (; right!=termid_last; ++left,++right)
		{
			termid_type left_termid=static_cast<termid_type>(*left);
			termid_type right_termid=static_cast<termid_type>(*right);
			delim_counter += (delim_termid==right_termid);
			q_win.pop_front();					//pop *left out, while *right not in yet.
			if (0==delim_counter && *(identity_vec_beg + left_termid)
					&& *(identity_vec_beg + right_termid))
			{
				//count co-occurrence only when neither *left nor *right in window
				if (find(q_win.begin(),q_win.end(),left_termid)==q_win.end() &&
						find(q_win.begin(),q_win.end(),right_termid)==q_win.end())
				{
					matrix_type::value_type& co=co_mat.iat(left_termid,right_termid);
					co.first+=distance; ++co.second;
				}
			}
			q_win.push_back(*right);			//push *right in
			delim_counter -= (delim_termid==left_termid);
		}
	}

	//don't calculate mean distance, just addup to total distance
	////calculate mean distance of each pair of co-occurrence terms
	//matrix_type::iterator co_iter=co_mat.begin();
	//matrix_type::iterator co_end=co_mat.end();
	//for (; co_iter!=co_end; ++co_iter)
	//	co_iter->first /= co_iter->second;		//element exist => termid_first>0
}

//function: calculate co-occurrence of 'termids in range [termid_first,termid_last) 
//	put result into co_mat
//parameter: termid_first,termid_last: [termid_first,termid_last) denotes a range of termids
//parameter: co_win: co-occurrence window size, max term distance considered is co_win-1
//parameter: co_mat: a matrix that stores co-occurrence information of termids
//precondition: co_mat is large enough to contain max termid
//postcondition: co-occurrence information of termids in [termid_first, termid_last)
//	with co-occurrence window co_win is stored into co_mat
template <typename InputIterator>
void co_occurrence(InputIterator termid_first, InputIterator termid_last, size_type co_win,
		sparse_matrix<std::pair<distance_type,size_type> >& co_mat)
{
	co_occurrence(termid_first,termid_last,repeater(true),co_win,co_mat);
}

//function: calculate co-occurrence of concerned 'termids in
//	range [termid_first,termid_last) and put result into co_mat
//	concerned co-occurrence pairs: concerned->concerned, concerned->background, background->concerned
//parameter: termid_first,termid_last: [termid_first,termid_last) denotes a range of termids
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: bg_iden_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is background.
//parameter: co_win: co-occurrence window size, max term distance considered is co_win-1
//parameter: co_mat: a matrix that stores co-occurrence information of termids
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: co_mat is large enough to contain max termid
//postcondition: co-occurrence information of termids in [termid_first, termid_last)
//	with co-occurrence window co_win is stored into co_mat
template <typename ForwardIterator,
	typename RandomAccessIterator1, typename RandomAccessIterator2>
void co_occurrence(ForwardIterator termid_first, ForwardIterator termid_last,
		RandomAccessIterator1 identity_vec_beg, RandomAccessIterator2 bg_iden_vec_beg,
		size_type co_win, sparse_matrix<std::pair<distance_type,size_type> >& co_mat)
{
	typedef sparse_matrix<std::pair<distance_type,size_type> > matrix_type;
	//co_mat.clear();							//no clear, just append info

	//for every possible term distance
	for (size_type distance=1; distance<co_win; ++distance)
	{
		//prepare two iterator
		size_type delim_counter=0;
		std::deque<termid_type> q_win;
		ForwardIterator left=termid_first, right=left;
		for (size_type i=0; i<distance && right!=termid_last; ++i,++right)
		{
			termid_type termid=static_cast<termid_type>(*right);
			delim_counter += (delim_termid==termid);
			q_win.push_back(termid);
		}

		//scan term range, culculate co-occurrence of distance
		for (; right!=termid_last; ++left,++right)
		{
			termid_type left_termid=static_cast<termid_type>(*left);
			termid_type right_termid=static_cast<termid_type>(*right);
			delim_counter += (delim_termid==right_termid);
			q_win.pop_front();					//pop *left out, while *right not in yet.
			if (0==delim_counter)
			{
				//check legal pair
				bool is_cc_pair=*(identity_vec_beg+left_termid) && *(identity_vec_beg+right_termid);
				bool is_cb_pair=*(identity_vec_beg+left_termid) && *(bg_iden_vec_beg+right_termid);
				bool is_bc_pair=*(bg_iden_vec_beg+left_termid) && *(identity_vec_beg+right_termid);
				if (is_cc_pair || is_cb_pair || is_bc_pair)
				{
					//count co-occurrence only when neither *left nor *right in window
					if (find(q_win.begin(),q_win.end(),left_termid)==q_win.end() &&
							find(q_win.begin(),q_win.end(),right_termid)==q_win.end())
					{
						matrix_type::value_type& co=co_mat.iat(left_termid,right_termid);
						co.first+=distance; ++co.second;
					}
				}
			}
			q_win.push_back(*right);			//push *right in
			delim_counter -= (delim_termid==left_termid);
		}
	}

	//don't calculate mean distance, just addup to total distance
	////calculate mean distance of each pair of co-occurrence terms
	//matrix_type::iterator co_iter=co_mat.begin();
	//matrix_type::iterator co_end=co_mat.end();
	//for (; co_iter!=co_end; ++co_iter)
	//	co_iter->first /= co_iter->second;		//element exist => termid_first>0
}

//function: calculate mean distance of each distance-count pair
//parameter: d_n_first,d_n_last: [d_n_first,d_n_last) denotes a range of pair<distance,count>
//postcondition: every (d,n) pair in [d_n_first,d_n_last) is set to (d/n,n)
template <typename ForwardIterator>
void mean_distance(ForwardIterator d_n_first, ForwardIterator d_n_last)
{
	for (; d_n_first!=d_n_last; ++d_n_first)
		d_n_first->first /= d_n_first->second;
}

//function: calculate total distance of each distance-count pair,
//	as reverse operation of mean_distance
//parameter: d_n_first,d_n_last: [d_n_first,d_n_last) denotes a range of pair<distance,count>
//postcondition: every (d,n) pair in [d_n_first,d_n_last) is set to (d*n,n)
template <typename ForwardIterator>
void total_distance(ForwardIterator d_n_first, ForwardIterator d_n_last)
{
	for (; d_n_first!=d_n_last; ++d_n_first)
		d_n_first->first *= d_n_first->second;
}

//function: calculate WAF of terms based on co-occurrence matrix
//	put result into waf_mat
//parameter: co_mat: a matrix that stores co-occurrence information of terms
//parameter: tf_vec_beg: an iterator that denotes a vector storing term frequency
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: precision: minimum waf value waf_mat stores, waf value less
//	than precision will be ignored.
//parameter: waf_mat: a matrix that stores WAF of terms
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: co_mat and tf_vec have the same term amount
//precondition: waf_mat is large enough to contain max termid
//postcondition: waf values of terms are stored into waf_mat
template <typename RandomAccessIterator1, typename RandomAccessIterator2>
void word_activation_force(
		const cross_list<std::pair<distance_type,size_type> >& co_mat,
		RandomAccessIterator1 tf_vec_beg, RandomAccessIterator2 identity_vec_beg,
		force_type precision, cross_list<force_type>& waf_mat)
{
	typedef cross_list<std::pair<distance_type,size_type> > co_matrix_type;
	waf_mat.clear();

	co_matrix_type::const_iterator co_iter=co_mat.cbegin();
	co_matrix_type::const_iterator co_end=co_mat.cend();
	for (; co_iter!=co_end; ++co_iter)			//every element in co_mat
	{
		//check if term pair is concerned
		termid_type i=co_iter.row_index(), j=co_iter.col_index();
		if (!(*(identity_vec_beg+i) && *(identity_vec_beg+j)))
			continue;

		//calculate waf from term i to term j which the element denotes
		force_type f_ij=static_cast<force_type>( co_iter->second );
		force_type f_i=static_cast<force_type>( *(tf_vec_beg+i) );
		force_type f_j=static_cast<force_type>( *(tf_vec_beg+j) );
		distance_type d_ij_mean=co_iter->first;
		force_type waf=f_ij * f_ij / (f_i * f_j * d_ij_mean * d_ij_mean);

		//check precision and store waf into waf_mat
		if (waf>=precision)
			waf_mat.rset(i,j,waf);
	}
}

//function: calculate WAF of terms based on co-occurrence matrix
//	put result into waf_mat
//parameter: co_mat: a matrix that stores co-occurrence information of terms
//parameter: tf_vec_beg: an iterator that denotes a vector storing term frequency
//parameter: precision: minimum waf value waf_mat stores, waf value less
//	than precision will be ignored.
//parameter: waf_mat: a matrix that stores WAF of terms
//precondition: co_mat and tf_vec have the same term amount
//precondition: waf_mat is large enough to contain max termid
//postcondition: waf values of terms are stored into waf_mat
template <typename RandomAccessIterator>
void word_activation_force(
		const cross_list<std::pair<distance_type,size_type> >& co_mat,
		RandomAccessIterator tf_vec_beg, force_type precision,
		cross_list<force_type>& waf_mat)
{
	word_activation_force(co_mat,tf_vec_beg,repeater(true),precision,waf_mat);
}

//function: affinity utility: count
//note: internal utility
//parameter: Iterator, should be cross_list<force_type>::row_iterator/col_iterator/iterator
template <typename Iterator, typename RandomAccessIterator, typename TermIdOf>
std::pair<affinity_type,size_type> affinity_count(
		Iterator first1, Iterator last1, Iterator first2, Iterator last2,
		RandomAccessIterator identity_vec_beg, TermIdOf termid_of)
{
	std::pair<affinity_type,size_type> cnt(0,0);
	while (first1!=last1 || first2!=last2)
	{
		//boundary conditions
		while (first1!=last1 && !*(identity_vec_beg + termid_of(first1)))
			++first1;							//skip unconcerned node
		if (first1==last1)						//count concerned node when one iter reach end
		{
			for (; first2!=last2; ++first2)
				if (*(identity_vec_beg + termid_of(first2)))
					++cnt.second;
			break;
		}
		while (first2!=last2 && !*(identity_vec_beg + termid_of(first2)))
			++first2;
		if (first2==last2)
		{
			for (; first1!=last1; ++first1)
				if (*(identity_vec_beg + termid_of(first1)))
					++cnt.second;
			break;
		}

		//deal with a pair of in-link wafs
		++cnt.second;
		if (termid_of(first1)==termid_of(first2))
		{
			force_type waf1 = *first1++, waf2 = *first2++;
			cnt.first += std::min(waf1,waf2)/std::max(waf1,waf2);
		}
		else if (termid_of(first1)<termid_of(first2))
			++first1;
		else //termid_of(first1)>termid_of(first2)
			++first2;
	}
	return cnt;
}

//function: calculate affinity measure between two termids in two waf matrices
//parameter: waf_mat1,waf_mat2: matrices that store WAF measure of terms
//parameter: i1,i2: termids that will calculate affinity measure
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//precondition: waf_mat1 and waf_mat2 have the same term amount
//postcondition: affinity measure of i1 and i2 between waf_mat1 and
//	waf_mat2 are all returned
template <typename RandomAccessIterator>
affinity_type affinity_measure(
		const cross_list<force_type>& waf_mat1, termid_type i1,
		const cross_list<force_type>& waf_mat2, termid_type i2,
		RandomAccessIterator identity_vec_beg)
{
	typedef cross_list<force_type> waf_matrix_type;
	affinity_type zero=static_cast<affinity_type>(0);

	//if term have neither in-link nor out-link,
	//just drop it, don't need to analyze affinity
	//note: (from Prof. Jun Guo, 2011.05.11_15'30)
	if (waf_mat1.size_of_row(i1)==0 && waf_mat1.size_of_col(i2)==0)
		return zero;
	if (waf_mat2.size_of_row(i2)==0 && waf_mat2.size_of_col(i2)==0)
		return zero;

	//calculate k_mean (scan col_i and col_j)
	std::pair<affinity_type,size_type> k=
		affinity_count(waf_mat1.cbegin_of_col(i1),waf_mat1.cend_of_col(i1),
				waf_mat2.cbegin_of_col(i2),waf_mat2.cend_of_col(i2), identity_vec_beg,
				std::mem_fun_ref(&waf_matrix_type::const_col_iterator::row_index));
	//note: k_mean=1 if no in-link (from Prof. Jun Guo, 2011.03.28_11'30)
	affinity_type k_mean=k.second ? k.first/k.second : 1;
	if (k_mean<=zero)
		return zero;

	//calculate l_mean (scan row_i and row_j)
	std::pair<affinity_type,size_type> l=
		affinity_count(waf_mat2.cbegin_of_row(i1),waf_mat2.cend_of_row(i1),
				waf_mat2.cbegin_of_row(i2),waf_mat2.cend_of_row(i2), identity_vec_beg,
				std::mem_fun_ref(&waf_matrix_type::const_row_iterator::col_index));
	//note: l_mean=1 if no out-link (from Prof. Jun Guo, 2011.03.28_11'30)
	affinity_type l_mean=l.second ? l.first/l.second : 1;
	if (l_mean<=zero)
		return zero;

	//calculate affinity measure
	return std::sqrt(k_mean*l_mean);
}

//function: calculate affinity measure between two termids in two waf matrices
//parameter: waf_mat1,waf_mat2: matrices that store WAF measure of terms
//parameter: i1,i2: termids that will calculate affinity measure
//precondition: waf_mat1 and waf_mat2 have the same term amount
//postcondition: affinity measure of i1 and i2 between waf_mat1 and
//	waf_mat2 are all returned
inline affinity_type affinity_measure(
		const cross_list<force_type>& waf_mat1, termid_type i1,
		const cross_list<force_type>& waf_mat2, termid_type i2)
{
	return affinity_measure(waf_mat1,i1,waf_mat2,i2,repeater(true));
}

//utility: add up information in range
//note: internal utility
//parameter: Iterator, should be cross_list<T>::row_iterator/col_iterator/iterator
template <typename Iterator, typename RandomAccessIterator, typename TermIdOf>
std::pair<typename Iterator::value_type,size_type> add_up(
		Iterator first, Iterator last, RandomAccessIterator identity_vec_beg, TermIdOf termid_of)
{
	std::pair<typename Iterator::value_type,size_type> result(0,0);
	for (; first!=last; ++first)
	{
		if (*(identity_vec_beg + termid_of(first)))
		{
			result.first += *first;
			++result.second;
		}
	}
	return result;
}

//function: calculate affinity measure of waf_mat, put result into a_mat
//parameter: waf_mat: a matrix that stores WAF measure of terms
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: precision: minimum affinity measure that a_mat stores,
//	affinity measure that less than precision will be ignored.
//parameter: a_mat: a matrix that stores affinity measures of terms
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: waf_mat and a_mat have the same term amount
//postcondition: affinity measures of waf_mat are all stored into a_mat
template <typename RandomAccessIterator>
void affinity_measure(const cross_list<force_type>& waf_mat,
		RandomAccessIterator identity_vec_beg, affinity_type precision,
		cross_list<affinity_type>& a_mat)
{
	typedef cross_list<force_type> waf_matrix_type;
	a_mat.clear();
	size_type n=waf_mat.row_size();

	//tbc: test optimization
	//optimization: calculate in-link and out-link information of every concerned node
	std::deque<std::pair<force_type,size_type> > in_stat(n),out_stat(n);
	for (termid_type i=0; i<n; ++i)
	{
		if (!*(identity_vec_beg+i))
			continue;

		//in-link and out-link statistic information
		in_stat[i] = add_up(
				waf_mat.cbegin_of_col(i),waf_mat.cend_of_col(i),identity_vec_beg,
				std::mem_fun_ref(&waf_matrix_type::const_col_iterator::row_index));
		out_stat[i] = add_up(
				waf_mat.cbegin_of_row(i),waf_mat.cend_of_row(i),identity_vec_beg,
				std::mem_fun_ref(&waf_matrix_type::const_row_iterator::col_index));
	}

	//calculate affinity of each pair
	for (termid_type i=0; i<n; ++i)
	{
		if (!*(identity_vec_beg+i))
			continue;

		a_mat.rset(i,i,static_cast<affinity_type>(1));//a_mat(i,i) is always 1
		for (termid_type j=i+1; j<n; ++j)		//a_mat(i,j) == a_mat(j,i)
		{
			if (!*(identity_vec_beg+j))
				continue;

			//tbc: test optimization
			//optimization: pre judge before exact calculation
			//a = sqrt((Sigma(min(waf_ki,waf_kj)/max(waf_ki,waf_kj))/union(ki,kj))
			//		* (Sigma(min(waf_il,waf_jl)/max(waf_il,waf_jl))/union(il,jl)))
			// <= sqrt((1.0*min(ki,kj)/max(ki,kj)) * (1.0*min(il,jl)/max(il,jl)))
			//  = a_max
			//s.t. a_max<precision => a<precision
			size_type in_min=std::min(in_stat[i].second,in_stat[j].second);
			size_type in_max=std::max(in_stat[i].second,in_stat[j].second);
			size_type out_min=std::min(out_stat[i].second,out_stat[j].second);
			size_type out_max=std::max(out_stat[i].second,out_stat[j].second);
			affinity_type a_in_max=static_cast<affinity_type>(1.0)*in_min/in_max;
			affinity_type a_out_max=static_cast<affinity_type>(1.0)*out_min/out_max;
			affinity_type a_max=std::sqrt(a_in_max*a_out_max);
			if (a_max<precision)
				continue;

			//calculate affinity
			//affinity_type a=affinity_measure(waf_mat,i,waf_mat,j,identity_vec_beg);
			affinity_type a=affinity_measure(waf_mat,i,waf_mat,j);
			if (a>=precision)
			{
				a_mat.rset(i,j,a);
				a_mat.rset(j,i,a);
			}
		}
	}
}

//function: calculate affinity measure of waf_mat, put result into a_mat
//parameter: waf_mat: a matrix that stores WAF measure of terms
//parameter: precision: minimum affinity measure that a_mat stores,
//	affinity measure that less than precision will be ignored.
//parameter: a_mat: a matrix that stores affinity measures of terms
//precondition: waf_mat and a_mat have the same term amount
//postcondition: affinity measures of waf_mat are all stored into a_mat
//note: declare as inline to avoid multi definition error in linking
inline void affinity_measure(const cross_list<force_type>& waf_mat,
		affinity_type precision, cross_list<affinity_type>& a_mat)
{
	affinity_measure(waf_mat,repeater(true),precision,a_mat);
}

//function: calculate affinity measure between two waf matrices, put result into a vector
//parameter: waf_mat1,waf_mat2: matrices that store WAF measure of terms
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: a_vec_beg: an iterator that denotes a vector to store affinity measures
//precondition: waf_mat1 and waf_mat2 have the same term amount
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: the vector that a_vec_beg denotes is large enough to contain max valid termid
//postcondition: affinity measures between waf_mat1 and waf_mat2 are all stored into vector
//	that a_vec_beg denotes
template <typename RandomAccessIterator, typename OutputIterator>
void affinity_measure(
		const cross_list<force_type>& waf_mat1, const cross_list<force_type>& waf_mat2,
		RandomAccessIterator identity_vec_beg, affinity_type precision, OutputIterator a_vec_beg)
{
	bool same_mat=(waf_mat1==waf_mat2);
	size_type n=waf_mat1.row_size();
	for (termid_type i=0; i<n; ++i,++a_vec_beg)
	{
		if (!*(identity_vec_beg+i))
			*a_vec_beg = static_cast<affinity_type>(0);
		else
		{
			affinity_type a=0;
			if (same_mat)
				a =  static_cast<affinity_type>(1);
			else
				a = affinity_measure(waf_mat1,i,waf_mat2,i,repeater(true));

			*a_vec_beg = (a>=precision ? a : static_cast<affinity_type>(0));
		}
	}
}

//function: calculate affinity measure between two waf matrices, put result into a vector
//parameter: waf_mat1,waf_mat2: matrices that store WAF measure of terms
//parameter: a_vec_beg: an iterator that denotes a vector to store affinity measures
//precondition: waf_mat1 and waf_mat2 have the same term amount
//precondition: the vector that a_vec_beg denotes is large enough to contain max valid termid
//postcondition: affinity measures between waf_mat1 and waf_mat2 are all stored into vector
//	that a_vec_beg denotes
//note: (from Prof. Jun Guo, 2011.04.08_10'00)
template <typename OutputIterator>
void affinity_measure( const cross_list<force_type>& waf_mat1,
		const cross_list<force_type>& waf_mat2, affinity_type precision,
		OutputIterator a_vec_beg)
{
	affinity_measure(waf_mat1,waf_mat2,repeater(true),precision,a_vec_beg);
}

//--------------------------thinking in waf---------------------------------

//function: calculate co-occurrence (term distance is exactly 'd')
//	of 'termids in range [termid_first,termid_last), put result into co_mat
//parameter: termid_first,termid_last: [termid_first,termid_last) denotes a range of termids
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: distance: distance between co-occurrence terms that being considered
//parameter: co_mat: a matrix that stores co-occurrence information of termids
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: co_mat is large enough to contain max termid
//postcondition: co-occurrence amount of termids in [termid_first, termid_last)
//	with co-occurrence distance d is added into co_mat
template <typename InputIterator, typename RandomAccessIterator>
void co_occurrence(InputIterator termid_first, InputIterator termid_last,
		RandomAccessIterator identity_vec_beg, size_type distance,
		sparse_matrix<size_type>& co_mat)
{
	//prepare two iterator
	size_type delim_counter=0;
	std::deque<termid_type> q_win;
	InputIterator left=termid_first, right=left;
	for (size_type i=0; i<distance && right!=termid_last; ++i,++right)
	{
		delim_counter += (delim_termid==*right);
		q_win.push_back(*right);
	}

	//scan term range, culculate co-occurrence of distance
	for (; right!=termid_last; ++left,++right)
	{
		delim_counter += (delim_termid==*right);
		q_win.pop_front();					//pop *left out, while *right not in yet.
		if (0==delim_counter && *(identity_vec_beg + *left)
				&& *(identity_vec_beg + *right))
		{
			//count co-occurrence only when neither *left nor *right in window
			if (find(q_win.begin(),q_win.end(),*left)==q_win.end() &&
					find(q_win.begin(),q_win.end(),*right)==q_win.end())
			{
				++co_mat.iat(*left,*right);
			}
		}
		q_win.push_back(*right);			//push *right in
		delim_counter -= (delim_termid==*left);
	}
}

//function: calculate co-occurrence (term distance is exactly 'd')
//	of termids in range [termid_first,termid_last), put result into co_mat
//parameter: termid_first,termid_last: [termid_first,termid_last) denotes a range of termids
//parameter: distance: distance between co-occurrence terms that being considered
//parameter: co_mat: a matrix that stores co-occurrence information of termids
//precondition: co_mat is large enough to contain max termid
//postcondition: co-occurrence amount of termids in [termid_first, termid_last)
//	with co-occurrence distance d is added into co_mat
template <typename InputIterator>
void co_occurrence(InputIterator termid_first, InputIterator termid_last,
		size_type distance, sparse_matrix<size_type>& co_mat)
{
	co_occurrence(termid_first,termid_last,repeater(true),distance,co_mat);
}

//function: calculate 'word activatioin force'(or WAF) of terms based on co-occurrence matrix
//	put result into waf_mat
//parameter: co_mat: a matrix that stores co-occurrence information of terms
//parameter: distance: distance between each pair of co-occurrence terms
//parameter: tf_vec_beg: an iterator that denotes a vector storing term frequency
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: precision: minimum waf value waf_mat stores, waf value less
//	than precision will be ignored.
//parameter: waf_mat: a matrix that stores WAF of terms
//precondition: co_mat and term frequency vector have the same term amount
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: waf_mat is large enough to contain max termid
//postcondition: waf values of terms are stored into waf_mat
template <typename RandomAccessIterator1, typename RandomAccessIterator2>
void word_activation_force(const cross_list<size_type>& co_mat, size_type distance,
		RandomAccessIterator1 tf_vec_beg, RandomAccessIterator2 identity_vec_beg,
		force_type precision, cross_list<force_type>& waf_mat)
{
	typedef cross_list<size_type> co_matrix_type;
	waf_mat.clear();

	co_matrix_type::const_iterator co_iter=co_mat.cbegin();
	co_matrix_type::const_iterator co_end=co_mat.cend();
	for (; co_iter!=co_end; ++co_iter)			//every element in co_mat
	{
		//check if term pair is concerned
		termid_type i=co_iter.row_index(), j=co_iter.col_index();
		if (!(*(identity_vec_beg+i) && *(identity_vec_beg+j)))
			continue;

		//calculate taf from term i to term j which the element denotes
		force_type f_ij=static_cast<force_type>( *co_iter );
		force_type f_i=static_cast<force_type>( *(tf_vec_beg+i) );
		force_type f_j=static_cast<force_type>( *(tf_vec_beg+j) );
		distance_type d_ij=static_cast<distance_type>( distance );
		force_type waf=f_ij * f_ij / (f_i * f_j * d_ij * d_ij);

		//check precision and store waf into waf_mat
		if (waf>=precision)
			waf_mat.rset(i,j,waf);
	}
}

//function: calculate 'word activatioin force'(or WAF) of terms based on co-occurrence matrix
//	put result into waf_mat
//parameter: co_mat: a matrix that stores co-occurrence information of terms
//parameter: distance: distance between each pair of co-occurrence terms
//parameter: tf_vec_beg: an iterator that denotes a vector storing term frequency
//parameter: precision: minimum waf value waf_mat stores, waf value less
//	than precision will be ignored.
//parameter: waf_mat: a matrix that stores WAF of terms
//precondition: co_mat and term frequency vector have the same term amount
//precondition: waf_mat is large enough to contain max termid
//postcondition: waf values of terms are stored into waf_mat
template <typename RandomAccessIterator>
void word_activation_force(const cross_list<size_type>& co_mat, size_type distance,
		RandomAccessIterator tf_vec_beg, force_type precision, cross_list<force_type>& waf_mat)
{
	word_activation_force(co_mat,distance,tf_vec_beg,repeater(true),precision,waf_mat);
}

//function: calculate affinity measure between two matrices, put result into a_mat
//parameter: waf_mat1,waf_mat2: matrices that store WAF measure of terms
//parameter: identity_vec_beg: an iterator that denotes a vector storing
//	boolean compatible values, to identify whether a termid is concerned.
//parameter: precision: minimum affinity measure that a_mat stores,
//	affinity measure that less than precision will be ignored.
//parameter: a_mat: a matrix that stores affinity measures of terms
//precondition: the vector identity_vec_beg denotes is large enough to contain
//	max valid termid
//precondition: waf_mat1, waf_mat2 and a_mat have the same term amount
//postcondition: affinity measures between waf_mat1 and waf_mat2 are all stored into a_mat
template <typename RandomAccessIterator>
void affinity_measure(
		const cross_list<force_type>& waf_mat1, const cross_list<force_type>& waf_mat2,
		RandomAccessIterator identity_vec_beg, affinity_type precision,
		cross_list<affinity_type>& a_mat)
{
	typedef cross_list<force_type> waf_matrix_type;
	a_mat.clear();

	size_type n=waf_mat1.row_size();
	for (termid_type i=0; i<n; ++i)
	{
		if (!*(identity_vec_beg+i))
			continue;
		for (termid_type j=0; j<n; ++j)
		{
			if (!*(identity_vec_beg+j))
				continue;
			affinity_type a=affinity_measure(waf_mat1,i,waf_mat2,j,repeater(true));
			if (a>=precision)
				a_mat.rset(i,j,a);
		}
	}
}

//function: calculate affinity measure between two waf matrices, put result into a_mat
//parameter: waf_mat1,waf_mat2: matrices that store WAF measure of terms
//parameter: precision: minimum affinity measure that a_mat stores,
//	affinity measure that less than precision will be ignored.
//parameter: a_mat: a matrix that stores affinity measures of terms
//precondition: waf_mat1, waf_mat2 and a_mat have the same term amount
//postcondition: affinity measures between waf_mat1 and waf_mat2 are all stored into a_mat
//note: declare as inline to avoid multi definition error in linking
inline void affinity_measure(
		const cross_list<force_type>& waf_mat1, const cross_list<force_type>& waf_mat2,
		affinity_type precision, cross_list<affinity_type>& a_mat)
{
	affinity_measure(waf_mat1,waf_mat2,repeater(true),precision,a_mat);
}

} //namespace waf

#endif //WAF_H
