// bm-inl.h
#ifndef BM_INL_H_
#define BM_INL_H_

#include "bm.h"
#include <algorithm>

namespace bm {

// =============================================================================
template <typename T>
template <typename RandomAccessIterator>
Subsequence<T>::Subsequence(
		RandomAccessIterator first, RandomAccessIterator last):
	subseq_(first, last) {
	make_bad_character(subseq_.begin(), subseq_.end(), badc_dict_);
	make_good_suffix(subseq_.begin(), subseq_.end(), goods_table_);
}

template <typename T>
typename Subsequence<T>::size_type Subsequence<T>::bad_character(
		const value_type& elem) const {
	typename BadCharacterDictionary::const_iterator iter = 
		badc_dict_.find(elem);
	return iter!=badc_dict_.end() ? iter->second : size();
}

template <typename T>
template <typename RandomAccessIterator>
void Subsequence<T>::make_bad_character(
		RandomAccessIterator first, RandomAccessIterator last,
		BadCharacterDictionary& badc_dict) {
	badc_dict.clear();
	if (first >= last) return;
	size_type seqlen = last - first;
	for (size_type i=0; i<seqlen; ++i, ++first)
		badc_dict[*first] = seqlen-1-i;
}

template <typename T>
template <typename RandomAccessIterator>
void Subsequence<T>::make_good_suffix(
		RandomAccessIterator first, RandomAccessIterator last,
		GoodSuffixTable& goods_table) {
	goods_table.clear();
	if (first >= last) {
		goods_table.push_back(1);
		return;
	}

	// initialize good suffix table
	size_type seqlen = last - first;
	goods_table.resize(seqlen, seqlen);  // all initialized as seqlen
	goods_table.back() = 1;  // last element initialized as 1

	// calculate pre-calculate data for good suffix table
	size_type max_sub_len=0;
	GoodSuffixTable& pre_table = goods_table;
	for (size_type i=seqlen-1; i>0; --i) {
		for (size_type j=0; j<i; ++j) {
			if (!std::equal(first+i, last, first+j)) continue;
			if (0 == j) {
				max_sub_len = seqlen - i;
			} else if (first[i-1] != first[j-1]){
				pre_table[i-1] = j-1;
			}
		}
	}

	// calculate good suffix table by pre-calculate data
	for (size_type i=0; i<seqlen-1; ++i) {
		if (pre_table[i] != seqlen) {
			goods_table[i] = seqlen-1 - pre_table[i];
		} else {
			goods_table[i] = seqlen-1 - i + pre_table[i];
			if (max_sub_len!=0 && seqlen-1-i>=max_sub_len)
				goods_table[i] -= max_sub_len;
		}
	}
}

// =============================================================================
template <typename RandomAccessIterator, typename T>
RandomAccessIterator find(RandomAccessIterator first, RandomAccessIterator last,
		const Subsequence<T>& subseq) {
	if (first >= last) return last;
	std::size_t seqlen = subseq.size(), txtlen=last-first;
	for (std::size_t i=seqlen-1, j=seqlen-1; j<txtlen; i=seqlen-1) {
		while (i!=0 && subseq[i]==first[j]) --i, --j;  // first mismatch
		if (0==i && subseq[i]==first[j]) return first+j;  // one match found
		j += std::max(subseq.good_suffix(i), subseq.bad_character(first[j]));
	}
	return last;  // no match
}

template <typename RandomAccessIterator, typename T>
inline bool search(RandomAccessIterator first, RandomAccessIterator last,
		const Subsequence<T>& subseq) {
	return find(first, last, subseq)!=last;
}

template <typename RandomAccessIterator, typename T>
std::size_t count(RandomAccessIterator first, RandomAccessIterator last,
		const Subsequence<T>& subseq) {
	std::size_t counter = 0;
	first = find(first, last, subseq);
	while (first!=last) {
		++ counter;
		first = find(next(first, subseq), last, subseq);
	}
	return counter;
}

template <typename RandomAccessIterator, typename T>
inline RandomAccessIterator next(
		RandomAccessIterator first, const Subsequence<T>& subseq) {
	return first + subseq.good_suffix(0)+1 - subseq.size();
}

// =============================================================================
template <typename RandomAccessIterator, typename T>
inline SubsequenceIterator<RandomAccessIterator, T>::SubsequenceIterator(
        sequence_iterator first, sequence_iterator last,
		const subsequence& subseq):
    begin_(first), end_(last), psubseq_(&subseq) {
    begin_ = find(begin_, end_, *psubseq_);
    make_match();
}

template <typename RandomAccessIterator, typename T>
inline bool SubsequenceIterator<RandomAccessIterator, T>::operator == (
        const SubsequenceIterator<RandomAccessIterator, T>& rhs) const {
    if (end_of_sequence() && rhs.end_of_sequence())  // both end-of-sequence
        return true;
    else if (end_of_sequence() || rhs.end_of_sequence())  // not both end-of-seq
        return false;
    else {  // both not end-of-sequence
        return begin_==rhs.begin_ && end_==rhs.end_ &&
            psubseq_==rhs.psubseq_ && match_==rhs.match_;  // field compare
    }
}

template <typename RandomAccessIterator, typename T>
inline SubsequenceIterator<RandomAccessIterator, T>&
    SubsequenceIterator<RandomAccessIterator, T>::operator ++ () {
    if (end_of_sequence()) return *this;
    begin_ = find(next(begin_, *psubseq_), end_, *psubseq_);
    make_match();
    return *this;
}

template <typename RandomAccessIterator, typename T>
inline SubsequenceIterator<RandomAccessIterator, T>
    SubsequenceIterator<RandomAccessIterator, T>::operator ++ (int) {
    SubsequenceIterator iter(*this);
    ++(*this);
    return iter;
}

template <typename RandomAccessIterator, typename T>
inline void SubsequenceIterator<RandomAccessIterator, T>::make_match() {
    if (end_of_sequence()) return;
    match_.first = begin_;
    match_.second = begin_!=end_ ? begin_+psubseq_->size() : end_;
}

}  // namespace bm

#endif  // BM_INL_H_
