#include "waf_facility.h"

#include <stdexcept>
#include <utility>

namespace waf {

// term to/from termid, termid existance predication
// =============================================================================
bool TermSet::insert(termid_type termid, const term_type& term) {
  if (search(termid) || search(term)) {
    return false;
  }
  term_set_type::iterator pos = term_set_.insert(term, termid).first;
  if (termid_set_.size() <= termid) {
    termid_set_.resize(termid + 1, term_set_.end());
  }
  termid_set_[termid] = pos;
  return true;
}

bool TermSet::erase(termid_type termid) {
  if (!search(termid)) {
    return false;
  }
  term_type term = (*this)[termid];
  remove(termid, term);
  return true;
}

bool TermSet::erase(const term_type& term) {
  if (!search(term)) {
    return false;
  }
  termid_type termid = (*this)[term];
  remove(termid, term);
  return true;
}

bool TermSet::search(const term_type& term) const {
  return term_set_.count(term) > 0;
}

bool TermSet::search(termid_type termid) const {
  return termid_set_.size() > termid && termid_set_[termid] != term_set_.end();
}

TermSet::termid_type TermSet::operator[](const term_type& term) const {
  term_set_type::const_iterator pos = term_set_.find(term);
  if (term_set_.end() == pos) {
    throw std::runtime_error(
        "TermSet::operator [](term_type) const: term not exist");
  }
  return pos.value();
}

TermSet::term_type TermSet::operator[](termid_type termid) const {
  if (termid_set_.size() <= termid || term_set_.end() == termid_set_[termid]) {
    throw std::runtime_error(
        "TermSet::operator [](termid_type) const: termid not exist");
  }
  const std::vector<char>& term = termid_set_[termid].key();
  return term_type(term.begin(), term.end());
}

void TermSet::remove(termid_type termid, const term_type& term) {
  // remove term/termid
  term_set_.erase(term);
  termid_set_[termid] = term_set_.end();

  // trim termid set
  while (!termid_set_.empty() && term_set_.end() == termid_set_.back()) {
    termid_set_.pop_back();
  }
}

std::istream& operator>>(std::istream& is, TermSet& termset) {
  using namespace serialization;
  termset.clear();
  std::pair<TermSet::termid_type, TermSet::term_type> mapping;
  while (is >> mapping) {
    termset.insert(mapping);
  }
  return is;
}

std::ostream& operator<<(std::ostream& os, const TermSet& termset) {
  for (termid_type i = 0; i < termset.termid_set_.size(); ++i) {
    if (termset.term_set_.end() == termset.termid_set_[i]) {
      continue;
    }
    const std::vector<char>& raw_term = termset.termid_set_[i].key();
    TermSet::term_type term(raw_term.begin(), raw_term.end());
    os << std::make_pair(i, term) << std::endl;
  }
  return os;
}

// term frequency vector
// =============================================================================
FreqVector::size_type& FreqVector::operator[](termid_type termid) {
  if (termid >= term_freqs_.size()) {
    term_freqs_.resize(termid + 1, 0);
  }
  return term_freqs_[termid];
}

FreqVector::size_type FreqVector::operator[](termid_type termid) const {
  return termid < term_freqs_.size() ? term_freqs_[termid] : 0;
}

std::istream& operator>>(std::istream& is, FreqVector& freqvec) {
  freqvec.clear();
  for (std::pair<FreqVector::termid_type, FreqVector::size_type> termfreq;
       is >> termfreq; freqvec[termfreq.first] = termfreq.second) {
  }
  return is;
}

std::ostream& operator<<(std::ostream& os, const FreqVector& freqvec) {
  for (FreqVector::termid_type i = 0; i < freqvec.term_freqs_.size(); ++i) {
    if (freqvec.term_freqs_[i] == 0) {
      continue;
    }
    os << std::make_pair(i, freqvec.term_freqs_[i]) << std::endl;
  }
  return os;
}

}  // namespace waf
