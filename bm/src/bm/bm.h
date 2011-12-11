// bm.h
// implement Boyer-Moore subsequence searching algorithm
// Author: Yongtian Zhang (yongtianzhang@gmail.com)
// Last modified at: 2011.10.25
#ifndef BM_H_
#define BM_H_

#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

#include "boost/unordered_map.hpp"

namespace bm {

// abstraction for a subsequence, provide BM algorithm support data structure
// =============================================================================
template <typename T>
class Subsequence {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef typename std::vector<value_type>::const_iterator iterator;
	typedef typename std::vector<value_type>::const_reference reference;
	typedef boost::unordered_map<value_type, size_type> BadCharacterDictionary;
	typedef std::vector<size_type> GoodSuffixTable;

public:
	// construct with subsequence range
	template <typename RandomAccessIterator>
	Subsequence(RandomAccessIterator first, RandomAccessIterator last);

	// subsequence element observers
	iterator begin() const { return subseq_.begin(); }
	iterator end() const { return subseq_.end(); }
	size_type size() const { return subseq_.size(); }
	reference operator [] (size_type n) const { return subseq_[n]; }

	// good suffix parameter of position n
	// pre-condition: 0 <= n < size()
	size_type good_suffix(size_type n) const { return goods_table_[n]; }

	// bad character parameter observer
	inline size_type bad_character(const value_type& elem) const;

public:
	// make bad character dictionary of subsequence [first, last)
	template <typename RandomAccessIterator>
	static void make_bad_character(
			RandomAccessIterator first, RandomAccessIterator last,
			BadCharacterDictionary& badc_dict);

	// make good suffix table of subsequence [first, last)
	template <typename RandomAccessIterator>
	static void make_good_suffix(
			RandomAccessIterator first, RandomAccessIterator last,
			GoodSuffixTable& goods_table);

	// friend function: find the 1st occurrence of subseq in 
	//	sequence [first, last), return last if not found
	template <typename RandomAccessIterator, typename U>
	friend RandomAccessIterator find(
			RandomAccessIterator first, RandomAccessIterator last,
			const Subsequence<U>& subseq);

private:
	std::vector<value_type> subseq_;
	BadCharacterDictionary badc_dict_;
	GoodSuffixTable goods_table_;
};  // class Subsequence


// free functions
// =============================================================================
// find the 1st occurrence of subseq in sequence [first, last),
//	return last if not found
template <typename RandomAccessIterator, typename T>
RandomAccessIterator find(RandomAccessIterator first, RandomAccessIterator last,
		const Subsequence<T>& subseq);

// search subseq in sequence [first, last), return true if found
template <typename RandomAccessIterator, typename T>
bool search(RandomAccessIterator first, RandomAccessIterator last,
		const Subsequence<T>& subseq);

// count occurrence of subseq in sequence [first, last)
template <typename RandomAccessIterator, typename T>
std::size_t count(RandomAccessIterator first, RandomAccessIterator last,
		const Subsequence<T>& subseq);

// get next position to be searched
// pre-condition: first donates a match of subseq
template <typename RandomAccessIterator, typename T>
RandomAccessIterator next(
		RandomAccessIterator first, const Subsequence<T>& subseq);


// a convenient class for subsequence iteration in sequence
// =============================================================================
template <typename RandomAccessIterator, typename T =
		typename std::iterator_traits<RandomAccessIterator>::value_type>
class SubsequenceIterator {
public:
	typedef T character;
	typedef RandomAccessIterator sequence_iterator;
	typedef Subsequence<character> subsequence;
	typedef std::pair<sequence_iterator, sequence_iterator> value_type;
	typedef const value_type& reference;
	typedef const value_type* pointer;
	typedef std::ptrdiff_t difference_type;
	typedef std::forward_iterator_tag iterator_category;

public:
	SubsequenceIterator(sequence_iterator first, sequence_iterator last,
			const subsequence& subseq);

	// end-of-sequence iterator
	SubsequenceIterator(): begin_(), end_(), psubseq_(NULL), match_() {}

	// compare two iterators
	bool operator == (const SubsequenceIterator& rhs) const;
	bool operator != (const SubsequenceIterator& rhs) const {
		return !(*this==rhs);
	}

	// standard observers of iterator
	// return a range (std::pair) donates matched range in sequence
	reference operator * () const { return match_; }
	pointer operator -> () const { return &match_; }

	// standard increment operator
	SubsequenceIterator& operator ++ ();
	SubsequenceIterator operator ++ (int);

private:
	// make match result
	void make_match();

    // check whether this is end-of-sequence iterator
    bool end_of_sequence() const { return NULL==psubseq_ || begin_==end_; }

private:
	sequence_iterator begin_;
	sequence_iterator end_;
	const subsequence* psubseq_;
	value_type match_;
};  // class BMIterator

}  // namespace bm

#include "bm-inl.h"

#endif  // BM_H_
