//waf_text.h
/**************************************************************************
module: waf_text
file(s): cross_list.h, sparse_matrix.h, waf.h, waf_text.h
function: provide toolset for text Word Activation Force (or WAF) analysis.

	This module is a patch to module waf, specially for text analysis.

	This module provides a data structure for term information representing,
	and provides some stream based functions for termid file analysis.

author: Yongtian Zhang, yongtianzhang@gmail.com
finish-time: 2011.04.20
last-modified: 2011.06.02

license:
	This code is not under any license, anyone can get it, use it, modified it, redistribute it without notice me. 
	I don't think there is no bug in it, so before you use it, please check and test carefully, I don't promise its quality. ^_^
	If you find any bug, please report it to me, I'll be very grateful to you. Discussion is also welcome.

**************************************************************************/
#ifndef WAF_TEXT_H
#define WAF_TEXT_H

#include <deque>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <iterator>
#include <cmath>
#include "waf.h"

//------------------------------data structure------------------------------

//class template: store term information
template <typename Term>
class term_info
{
public:
	typedef Term term_type;
	typedef waf::termid_type termid_type;
	typedef waf::size_type frequency_type;
	typedef waf::size_type size_type;

	typedef std::deque<term_type> term_of_id_type;
	typedef std::map<term_type,termid_type> id_of_term_type;
	typedef std::deque<frequency_type> freq_of_id_type;

	class frequency_iterator:					//this stuff is for identity param of algorithm
		public std::iterator<std::random_access_iterator_tag,frequency_type>
	{
	protected:
		typedef typename std::iterator<std::random_access_iterator_tag,
				frequency_type> iter_base;
	public:
		typedef typename iter_base::iterator_category iterator_category;
		typedef typename iter_base::value_type value_type;
		typedef typename iter_base::difference_type difference_type;
		typedef const frequency_type* pointer;
		typedef const frequency_type& reference;
	public:
		frequency_iterator(const freq_of_id_type& tf_vec, size_type index=0,
				frequency_type dft_freq=0):
			m_tf_vec(tf_vec),m_index(index),m_dft_freq(dft_freq)
		{
		}
		frequency_iterator(const frequency_iterator& other):
			m_tf_vec(other.m_tf_vec),m_index(other.m_index),
			m_dft_freq(other.m_dft_freq)
		{
		}
		reference operator * ()
		{
			return m_index<m_tf_vec.size() ? m_tf_vec[m_index] : m_dft_freq;
		}
		pointer operator -> () {return &*(*this);}
		frequency_iterator& operator ++ () {++m_index; return *this;}
		frequency_iterator& operator ++ (int)
		{
			frequency_iterator iter(*this);
			++(*this); return iter;
		}
		frequency_iterator& operator -- () {--m_index; return *this;}
		frequency_iterator& operator -- (int)
		{
			frequency_iterator iter(*this);
			--(*this); return iter;
		}
		frequency_iterator& operator += (difference_type n) {m_index+=n; return *this;}
		frequency_iterator& operator -= (difference_type n) {m_index-=n; return *this;}
		frequency_iterator operator + (difference_type n)
		{
			frequency_iterator iter(*this);
			return iter+=n;
		}
		frequency_iterator operator - (difference_type n)
		{
			frequency_iterator iter(*this);
			return iter-=n;
		}
		reference operator [] (difference_type n)
		{
			return n<m_tf_vec.size() ? m_tf_vec[n] : m_dft_freq;
		}
		bool operator == (const frequency_iterator& rhs) const
		{
			return this->m_tf_vec==rhs.m_tf_vec && m_index==rhs.m_index
				&& m_dft_freq==rhs.m_dft_freq;
		}
		bool operator != (const frequency_iterator& rhs) const {return !(*this==rhs);}
	protected:
		const freq_of_id_type& m_tf_vec;
		size_type m_index;
		const frequency_type m_dft_freq;
	};

public:
	bool insert(const term_type& term, termid_type termid, frequency_type freq)
	{
		if (m_id_of_term.count(term))
			return false;
		m_id_of_term[term] = termid;

		if (m_term_of_id.size()<=termid)
			m_term_of_id.resize(termid+1);
		m_term_of_id[termid] = term;

		if (m_freq_of_id.size()<=termid)
			m_freq_of_id.resize(termid+1);
		m_freq_of_id[termid] = freq;

		return true;
	}

	termid_type term_freq_inc(const term_type& term, frequency_type freq=1)
	{
		typename id_of_term_type::const_iterator iter=m_id_of_term.find(term);
		if (iter!=m_id_of_term.end())
		{
			m_freq_of_id[iter->second] += freq;
			return iter->second;
		}

		termid_type termid=m_id_of_term[term] = termid_size();
		term_of_id(termid) = term;
		freq_of_id(termid) = freq;
		return termid;
	}

	size_type count(const term_type& term) const
	{
		typename id_of_term_type::const_iterator iter=m_id_of_term.find(term);
		if (m_id_of_term.end()==iter)
			return 0;
		return m_freq_of_id[iter->second];
	}

	termid_type& id_of_term(const term_type& term)
	{
		if (!m_id_of_term.count(term))
			return m_id_of_term[term]=waf::delim_termid;
		return m_id_of_term[term];
	}

	term_type& term_of_id(termid_type termid)
	{
		if (termid>=termid_size())
			resize(termid+1);
		return m_term_of_id[termid];
	}

	frequency_type& freq_of_id(termid_type termid)
	{
		if (termid>=termid_size())
			resize(termid+1);
		return m_freq_of_id[termid];
	}

	const termid_type& id_of_term(const term_type& term) const
	{
		typename id_of_term_type::const_iterator iter=m_id_of_term.find(term);
		if (m_id_of_term.end()==iter)
			throw std::invalid_argument("term_info::id_of_term(const term_type&) const: term not in record!");
		return iter->second;
	}

	const term_type& term_of_id(termid_type termid) const
	{
		if (termid>=m_term_of_id.size())
			throw std::out_of_range("term_info::term_of_id(termid_type) const: termid too large!");
		return m_term_of_id[termid];
	}

	const frequency_type& freq_of_id(termid_type termid) const
	{
		if (termid>=m_freq_of_id.size())
			throw std::out_of_range("term_info::freq_of_id(termid_type) const: termid too large!");
		return m_freq_of_id[termid];
	}

	id_of_term_type& id_of_term() {return m_id_of_term;}
	term_of_id_type& term_of_id() {return m_term_of_id;}
	freq_of_id_type& freq_of_id() {return m_freq_of_id;}

	const id_of_term_type& id_of_term() const {return m_id_of_term;}
	const term_of_id_type& term_of_id() const {return m_term_of_id;}
	const freq_of_id_type& freq_of_id() const {return m_freq_of_id;}
	frequency_iterator freq_identifier() const
	{
		return frequency_iterator(m_freq_of_id);
	}

	size_type term_size() const {return m_id_of_term.size();}
	size_type termid_size() const {return m_term_of_id.size();}
	void resize(size_type termid_size)
	{
		m_term_of_id.resize(termid_size);
		m_freq_of_id.resize(termid_size);
	}
	void clear()
	{
		m_id_of_term.clear();
		m_term_of_id.clear();
		m_freq_of_id.clear();
	}

	//output term_info to std::ostream
	friend std::ostream& operator << (std::ostream& os, const term_info& ti)
	{
		const freq_of_id_type& freq_of_id=ti.freq_of_id();

		//sort by term frequency decreasing
		typedef std::pair<frequency_type,termid_type> tf_id;
		std::deque<tf_id> tf_ids;
		for (termid_type i=0; i<freq_of_id.size(); ++i)
			tf_ids.push_back(tf_id(freq_of_id[i],i));
		sort(tf_ids.begin(),tf_ids.end(), std::greater<tf_id>());

		//output to ostream
		os << "termid_size=" << ti.termid_size() << std::endl;
		os << "<term_freq>\t<termid>\t<term>" << std::endl;
		for (std::deque<tf_id>::iterator iter=tf_ids.begin();
				iter!=tf_ids.end(); ++iter)
		{
			os << iter->first << "\t" << iter->second
				<< "\t" << ti.term_of_id(iter->second) << std::endl;
		}
		return os;
	}

	//input term_info from std::istream
	friend std::istream& operator >> (std::istream& is, term_info& ti)
	{
		ti.clear();
		size_type termid_size=0;
		std::string text;
		getline(is,text,'='); is >> termid_size;
		getline(is,text); getline(is,text);		//ignore description line
		ti.resize(termid_size);
		while (is.peek()!=EOF)
		{
			term_type term;
			termid_type termid=0;
			frequency_type freq=0;
			is >> freq >> termid;				//get freq and termid
			getline(is,text,'\t');
			getline(is,text);
			std::stringstream ss(text); ss >> term;//get term
			if (is)								//in case for illegal line
				ti.insert(term,termid,freq);
		}
		return is;
	}

protected:
	term_of_id_type m_term_of_id;				//map termid to term
	id_of_term_type m_id_of_term;				//map term to termid
	freq_of_id_type m_freq_of_id;				//map termid to frequency
};

//stores node information of cross list
template <typename T>
struct node_info
{
	waf::termid_type row;
	waf::termid_type col;
	T value;
	node_info(waf::termid_type r=0,waf::termid_type c=0, T v=T()):
		row(r),col(c),value(v)
	{
	}
	static bool value_greater(const node_info& lhs, const node_info& rhs)
	{
		if (lhs.value>rhs.value || rhs.value>lhs.value)
			return lhs.value > rhs.value;
		if (lhs.row<rhs.row || rhs.row<lhs.row)
			return lhs.row < rhs.row;
		return lhs.col < rhs.col;
	}
};

template <typename T>
node_info<T> make_node_info(
		waf::termid_type r=0,waf::termid_type c=0, T v=T())
{
	node_info<T> ti;
	ti.row = r; ti.col = c; ti.value = v;
	return ti;
}

//--------------------------------algorithm---------------------------------

//function: calculate co-occurrence of window co_win
//parameter: termid_stream: stream that stores termid sequence, list delimited with -1 or '\n'
template <typename RandomAccessIterator>
void co_occurrence(std::istream& termid_stream, RandomAccessIterator identity_vec_beg,
		waf::size_type co_win, sparse_matrix<std::pair<waf::distance_type,waf::size_type> >& co_mat)
{
	std::string line;
	while (getline(termid_stream,line))
	{
		std::stringstream ss(line);
		std::istream_iterator<long> is_iter(ss), is_end;
		std::deque<long> paragraph;
		copy(is_iter,is_end,back_inserter(paragraph));//extract termid sequence to paragraph
		waf::co_occurrence(paragraph.begin(),paragraph.end(),identity_vec_beg,co_win,co_mat);
	}
}

//function: calculate co-occurrence of window co_win
//parameter: termid_stream: stream that stores termid sequence, list delimited with -1 or '\n'
//note: declare as inline to avoid multi definition error in linking
inline void co_occurrence(std::istream& termid_stream, waf::size_type co_win,
		sparse_matrix<std::pair<waf::distance_type,waf::size_type> >& co_mat)
{
	co_occurrence(termid_stream,waf::repeater(true),co_win,co_mat);
}

//function: calculate co-occurrence of window co_win
//parameter: termid_stream: stream that stores termid sequence, list delimited with -1 or '\n'
template <typename RandomAccessIterator1, typename RandomAccessIterator2>
void co_occurrence(std::istream& termid_stream, RandomAccessIterator1 identity_vec_beg,
		RandomAccessIterator2 bg_iden_vec_beg, waf::size_type co_win, 
		sparse_matrix<std::pair<waf::distance_type,waf::size_type> >& co_mat)
{
	std::string line;
	while (getline(termid_stream,line))
	{
		std::stringstream ss(line);
		std::istream_iterator<long> is_iter(ss), is_end;
		std::deque<long> paragraph;
		copy(is_iter,is_end,back_inserter(paragraph));//extract termid sequence to paragraph
		waf::co_occurrence(paragraph.begin(), paragraph.end(),
				identity_vec_beg, bg_iden_vec_beg, co_win, co_mat);
	}
}

//function: calculate co-occurrence information of termid file
//parameter: termid_file: filename of termid file
template <typename RandomAccessIterator>
void scan_termid_file(const std::string& termid_file, RandomAccessIterator identity_vec_beg,
		waf::size_type co_win, sparse_matrix<std::pair<waf::distance_type,waf::size_type> >& co_mat)
{
	waf::termid_type max_id=0;
	{											//get max termid in termid_file
		std::ifstream fin(termid_file.c_str());
		std::istream_iterator<long> is_iter(fin),is_end;
		max_id = waf::max_termid(is_iter,is_end,identity_vec_beg);
	}
	if (co_mat.row_size()<=max_id)				//resize co_mat's dimension
	{
		co_mat.resize(max_id+1,max_id+1);
	}
	{											//co-occurrence calculation
		std::ifstream fin(termid_file.c_str());
		co_occurrence(fin,identity_vec_beg,co_win,co_mat);
	}
	std::pair<waf::size_type,waf::size_type> sp=co_mat.sparse();
	waf::size_type sp_dim=static_cast<waf::size_type>(std::sqrt(1.0*co_mat.size()));
	if (sp_dim > 2*sp.first)					//sparse matrix
		co_mat.sparse(sp_dim,sp_dim);
}

//function: calculate co-occurrence information of termid file
//parameter: termid_file: filename of termid file
//note: declare as inline to avoid multi definition error in linking
inline void scan_termid_file(const std::string& termid_file, waf::size_type co_win,
		sparse_matrix<std::pair<waf::distance_type,waf::size_type> >& co_mat)
{
	scan_termid_file(termid_file,waf::repeater(true),co_win,co_mat);
}

//function: calculate co-occurrence information of termid file, with background terms
//parameter: termid_file: filename of termid file
template <typename RandomAccessIterator1, typename RandomAccessIterator2>
void scan_termid_file(const std::string& termid_file, RandomAccessIterator1 identity_vec_beg,
		RandomAccessIterator2 bg_iden_vec_beg, waf::size_type co_win,
		sparse_matrix<std::pair<waf::distance_type,waf::size_type> >& co_mat)
{
	waf::termid_type max_id=0;
	{											//get max termid in termid_file
		std::ifstream fin(termid_file.c_str());
		std::istream_iterator<long> is_iter(fin),is_end;
		max_id = waf::max_termid(is_iter,is_end,identity_vec_beg,bg_iden_vec_beg);
	}
	if (co_mat.row_size()<=max_id)				//resize co_mat's dimension
	{
		co_mat.resize(max_id+1,max_id+1);
	}
	{											//co-occurrence calculation
		std::ifstream fin(termid_file.c_str());
		co_occurrence(fin,identity_vec_beg,bg_iden_vec_beg,co_win,co_mat);
	}
	std::pair<waf::size_type,waf::size_type> sp=co_mat.sparse();
	waf::size_type sp_dim=static_cast<waf::size_type>(std::sqrt(1.0*co_mat.size()));
	if (sp_dim > 2*sp.first)					//sparse matrix
		co_mat.sparse(sp_dim,sp_dim);
}

//function: calculate term frequency of termid file
//parameter: termid_file: filename of termid file
template <typename RandomAccessIterator, typename Term>
void scan_termid_file(const std::string& termid_file,
		RandomAccessIterator identity_vec_beg, term_info<Term>& tinfo)
{
	waf::termid_type max_id=0;
	{											//get max termid in termid_file
		std::ifstream fin(termid_file.c_str());
		std::istream_iterator<long> is_iter(fin),is_end;
		max_id = waf::max_termid(is_iter,is_end,identity_vec_beg);
	}
	if (tinfo.termid_size()<=max_id)
	{
		tinfo.resize(max_id+1);
	}
	{											//co-occurrence calculation
		std::ifstream fin(termid_file.c_str());
		std::istream_iterator<long> is_iter(fin),is_end;
		waf::term_frequency(is_iter,is_end,identity_vec_beg,tinfo.freq_of_id().begin());
	}
}

//function: calculate term frequency of termid file
//parameter: termid_file: filename of termid file
template <typename Term>
void scan_termid_file(const std::string& termid_file, term_info<Term>& tinfo)
{
	scan_termid_file(termid_file,waf::repeater(true),tinfo);
}

//function: convert term to termid
template <typename InputIterator, typename TermType, typename OutputIterator>
void term_to_termid(InputIterator term_beg, InputIterator term_end,
		const std::set<TermType>& delim_set, term_info<TermType>& tinfo,
		OutputIterator termid_iter)
{
	for (; term_beg!=term_end; ++term_beg,++termid_iter)
	{
		if (delim_set.count(*term_beg))
			*termid_iter = waf::delim_termid;
		else
			*termid_iter = tinfo.term_freq_inc(*term_beg);
	}
}

template <typename InputIterator, typename TermType, typename OutputIterator>
void term_to_termid(InputIterator term_beg, InputIterator term_end,
		const term_info<TermType>& tinfo, OutputIterator termid_iter)
{
	for (; term_beg!=term_end; ++term_beg,++termid_iter)
	{
		try
		{
			*termid_iter = tinfo.id_of_term(*term_beg);
		}
		catch (const std::exception& e)
		{
			*termid_iter = waf::delim_termid;
		}
	}
}

template <typename TermType>
void scan_term_file(
		const std::string& term_file, const std::set<TermType>& delim_set,
		term_info<TermType>& tinfo, const std::string& termid_file)
{
	std::ifstream fin(term_file.c_str());
	std::ofstream fout(termid_file.c_str());
	std::string text;
	while (getline(fin,text))
	{
		std::stringstream ss(text);
		std::istream_iterator<std::string> is_iter(ss),is_end;
		term_to_termid(is_iter,is_end,delim_set,tinfo,
				std::ostream_iterator<long>(fout," "));
		if (fin.good())
			fout << '\n';
	}
}

#endif //WAF_TEXT_H
