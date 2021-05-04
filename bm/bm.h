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
  Subsequence(RandomAccessIterator first, RandomAccessIterator last)
      : subseq_(first, last) {
    make_bad_character(subseq_.begin(), subseq_.end(), badc_dict_);
    make_good_suffix(subseq_.begin(), subseq_.end(), goods_table_);
  }

  // subsequence element observers
  iterator begin() const { return subseq_.begin(); }
  iterator end() const { return subseq_.end(); }
  size_type size() const { return subseq_.size(); }
  reference operator[](size_type n) const { return subseq_[n]; }

  // good suffix parameter of position n
  // pre-condition: 0 <= n < size()
  size_type good_suffix(size_type n) const { return goods_table_[n]; }

  // bad character parameter observer
  inline size_type bad_character(const value_type& elem) const {
    typename BadCharacterDictionary::const_iterator iter =
        badc_dict_.find(elem);
    return iter != badc_dict_.end() ? iter->second : size();
  }

 public:
  // make bad character dictionary of subsequence [first, last)
  template <typename RandomAccessIterator>
  static void make_bad_character(RandomAccessIterator first,
                                 RandomAccessIterator last,
                                 BadCharacterDictionary& badc_dict) {
    badc_dict.clear();
    if (first >= last) return;
    size_type seqlen = last - first;
    for (size_type i = 0; i < seqlen; ++i, ++first)
      badc_dict[*first] = seqlen - 1 - i;
  }

  // make good suffix table of subsequence [first, last)
  template <typename RandomAccessIterator>
  static void make_good_suffix(RandomAccessIterator first,
                               RandomAccessIterator last,
                               GoodSuffixTable& goods_table) {
    goods_table.clear();
    if (first >= last) {
      goods_table.push_back(1);
      return;
    }

    // initialize good suffix table
    size_type seqlen = last - first;
    goods_table.resize(seqlen, seqlen);  // all initialized as seqlen
    goods_table.back() = 1;              // last element initialized as 1

    // calculate pre-calculate data for good suffix table
    size_type max_sub_len = 0;
    GoodSuffixTable& pre_table = goods_table;
    for (size_type i = seqlen - 1; i > 0; --i) {
      for (size_type j = 0; j < i; ++j) {
        if (!std::equal(first + i, last, first + j)) continue;
        if (0 == j) {
          max_sub_len = seqlen - i;
        } else if (first[i - 1] != first[j - 1]) {
          pre_table[i - 1] = j - 1;
        }
      }
    }

    // calculate good suffix table by pre-calculate data
    for (size_type i = 0; i < seqlen - 1; ++i) {
      if (pre_table[i] != seqlen) {
        goods_table[i] = seqlen - 1 - pre_table[i];
      } else {
        goods_table[i] = seqlen - 1 - i + pre_table[i];
        if (max_sub_len != 0 && seqlen - 1 - i >= max_sub_len)
          goods_table[i] -= max_sub_len;
      }
    }
  }

 private:
  std::vector<value_type> subseq_;
  BadCharacterDictionary badc_dict_;
  GoodSuffixTable goods_table_;
};

// free functions
// =============================================================================
// find the 1st occurrence of subseq in sequence [first, last),
//	return last if not found
template <typename RandomAccessIterator, typename T>
RandomAccessIterator find(RandomAccessIterator first, RandomAccessIterator last,
                          const Subsequence<T>& subseq) {
  if (first >= last) return last;
  std::size_t seqlen = subseq.size(), txtlen = last - first;
  for (std::size_t i = seqlen - 1, j = seqlen - 1; j < txtlen; i = seqlen - 1) {
    while (i != 0 && subseq[i] == first[j]) --i, --j;       // first mismatch
    if (0 == i && subseq[i] == first[j]) return first + j;  // one match found
    j += std::max(subseq.good_suffix(i), subseq.bad_character(first[j]));
  }
  return last;  // no match
}

// search subseq in sequence [first, last), return true if found
template <typename RandomAccessIterator, typename T>
bool search(RandomAccessIterator first, RandomAccessIterator last,
            const Subsequence<T>& subseq) {
  return find(first, last, subseq) != last;
}

// count occurrence of subseq in sequence [first, last)
template <typename RandomAccessIterator, typename T>
std::size_t count(RandomAccessIterator first, RandomAccessIterator last,
                  const Subsequence<T>& subseq) {
  std::size_t counter = 0;
  first = find(first, last, subseq);
  while (first != last) {
    ++counter;
    first = find(next(first, subseq), last, subseq);
  }
  return counter;
}

// get next position to be searched
// pre-condition: first donates a match of subseq
template <typename RandomAccessIterator, typename T>
RandomAccessIterator next(RandomAccessIterator first,
                          const Subsequence<T>& subseq) {
  return first + subseq.good_suffix(0) + 1 - subseq.size();
}

// a convenient class for subsequence iteration in sequence
// =============================================================================
template <typename RandomAccessIterator,
          typename T =
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
                      const subsequence& subseq)
      : begin_(first), end_(last), psubseq_(&subseq) {
    begin_ = find(begin_, end_, *psubseq_);
    make_match();
  }

  // end-of-sequence iterator
  SubsequenceIterator() = default;

  // compare two iterators
  bool operator==(const SubsequenceIterator& rhs) const {
    if (end_of_sequence() && rhs.end_of_sequence()) {  // both end-of-sequence
      return true;
    } else if (end_of_sequence() ||
               rhs.end_of_sequence()) {  // not both end-of-seq
      return false;
    } else {  // both not end-of-sequence
      return begin_ == rhs.begin_ && end_ == rhs.end_ &&
             psubseq_ == rhs.psubseq_ && match_ == rhs.match_;  // field compare
    }
  }
  bool operator!=(const SubsequenceIterator& rhs) const {
    return !(*this == rhs);
  }

  // standard observers of iterator
  // return a range (std::pair) donates matched range in sequence
  reference operator*() const { return match_; }
  pointer operator->() const { return &match_; }

  // standard increment operator
  SubsequenceIterator& operator++() {
    if (end_of_sequence()) {
      return *this;
    }
    begin_ = find(next(begin_, *psubseq_), end_, *psubseq_);
    make_match();
    return *this;
  }
  SubsequenceIterator operator++(int) {
    SubsequenceIterator iter(*this);
    ++(*this);
    return iter;
  }

 private:
  // make match result
  void make_match() {
    if (end_of_sequence()) {
      return;
    }
    match_.first = begin_;
    match_.second = begin_ != end_ ? begin_ + psubseq_->size() : end_;
  }

  // check whether this is end-of-sequence iterator
  bool end_of_sequence() const { return nullptr == psubseq_ || begin_ == end_; }

 private:
  sequence_iterator begin_;
  sequence_iterator end_;
  const subsequence* psubseq_ = nullptr;
  value_type match_ = {};
};

}  // namespace bm

#endif  // BM_H_
