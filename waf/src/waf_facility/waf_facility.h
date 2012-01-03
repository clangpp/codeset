// waf_facility.h
#ifndef WAF_FACILITY_H_
#define WAF_FACILITY_H_

// include for types, not for algorithm
#include "../waf_core/waf_core.h"

#include <cstddef>
#include <istream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

#include "../ac/ac.h"
#include "../serialization/serialization.h"

namespace waf {

// term to/from termid, termid existance predication
// =============================================================================
class TermSet {
public:
    typedef std::string term_type;
    typedef waf::termid_type termid_type;
    typedef waf::size_type size_type;
private:
    typedef ac::TrieMap<char, termid_type> term_set_type;
    typedef std::vector<term_set_type::const_iterator> termid_set_type;

public:  // insert term/termid mapping pair
    bool insert(termid_type termid, const term_type& term);
    bool insert(const term_type& term, termid_type termid) {
        return insert(termid, term);
    }
    bool insert(const std::pair<termid_type, term_type>& mapping) {
        return insert(mapping.first, mapping.second);
    }
    bool insert(const std::pair<term_type, termid_type>& mapping) {
        return insert(mapping.first, mapping.second);
    }

public:  // remove term/termid mapping pair
    bool erase(termid_type termid);
    bool erase(const term_type& term);

public:  // query term/termid existance
    bool search(const term_type& term) const;
    bool search(termid_type termid) const;
    size_type count(const term_type& term) const { return search(term); }
    size_type count(termid_type termid) const { return search(termid); }

public:  // query term/termid mapping, never change set
    termid_type operator [] (const term_type& term) const;
    term_type operator [] (termid_type termid) const;

public:  // observers
    size_type size() const { return term_set_.size(); }
    termid_type max_termid() const { return termid_set_.size()-1; }
    void clear() {
        term_set_.clear();
        termid_set_.clear();
    }

private:
    void remove(termid_type termid, const term_type& term);

private:  // serialization
    friend std::istream& operator >> (std::istream& is, TermSet& termset);
    friend std::ostream& operator << (std::ostream& os, const TermSet& termset);


private:
    term_set_type term_set_;
    termid_set_type termid_set_;
};

// unlimited term frequency vector
// =============================================================================
class FreqVector {
public:
    typedef waf::termid_type termid_type;
    typedef waf::size_type size_type;
    class iterator;
    class const_iterator;

private:
    template <typename FreqVecPointer>
    class basic_iterator:
        public std::iterator<std::random_access_iterator_tag, size_type> {
    public:  // constructors
        friend class iterator;
        friend class const_iterator;
        basic_iterator(FreqVecPointer pfreq_vec, size_type pos):
            pfreq_vec_(pfreq_vec), pos_(pos) {}
        basic_iterator(): pfreq_vec_(NULL), pos_(-1) {}
    public:  // member functions  // TBD: more members to be written
        basic_iterator operator + (difference_type n) const {
            return basic_iterator(pfreq_vec_, pos_+n);
        }
    protected:
        FreqVecPointer pfreq_vec_;
        size_type pos_;
    };

public:
    class iterator: public basic_iterator<FreqVector*> {
    public:
        typedef basic_iterator<FreqVector*> base;
        friend class const_iterator;
    public:
        iterator(FreqVector* pfreq_vec, size_type pos): base(pfreq_vec, pos) {}
        iterator(): base() {}
        iterator(const base& iter): base(iter.pfreq_vec_, iter.pos_) {}
    public:
        size_type& operator * () { return (*base::pfreq_vec_)[base::pos_]; }
        iterator operator + (difference_type n) const {
            return base::operator + (n);
        }
        size_type& operator [] (difference_type n) {
            return * iterator(*this + n);
        }
    };

    class const_iterator: public basic_iterator<const FreqVector*> {
    public:
        typedef basic_iterator<const FreqVector*> base;
        const_iterator(const FreqVector* pfreq_vec, size_type pos):
            base(pfreq_vec, pos) {}
        const_iterator(): base() {}
        const_iterator(const base& iter): base(iter.pfreq_vec_, iter.pos_) {}
        const_iterator(const iterator& other):
            base(other.pfreq_vec_, other.pos_) {}
    public:
        size_type operator * () const { return (*pfreq_vec_)[pos_]; }
        const_iterator operator + (difference_type n) const {
            return base::operator + (n);
        }
        size_type operator [] (difference_type n) const {
            return * const_iterator(*this + n);
        }
    };

public:  // iterator observers
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, term_freqs_.size()); }
    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end() const {
        return const_iterator(this, term_freqs_.size());
    }

public:  // term frequency observers
    size_type& operator [] (termid_type termid);
    size_type operator [] (termid_type termid) const;

public:  // member functions  // TBD: more functions to be written
    void clear() { term_freqs_.clear(); }

public:  // serialization
    friend std::istream& operator >> (std::istream& is, FreqVector& freqvec);
    friend std::ostream& operator << (
            std::ostream& os, const FreqVector& freqvec);

private:
    std::vector<size_type> term_freqs_;
};

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

// predicate facility
// =============================================================================
// class Always, to implement always true facility
template <typename R>
class Always {
public:
    typedef R return_type;
    Always(const R& value = R()): value_(value) {}
    template <typename T>
    const R& operator () (const T&) const { return value_; }
private:
    R value_;
};

// factory method: create Always<R> instance
template <typename R>
Always<R> always(const R& value) { return Always<R>(value); }

// global instance, Always<bool>(true)
inline Always<bool> care_all() { return always(true); }


// class TermSetObserver, to check whether a termid is in TermSet or not
class TermSetObserver {
public:
    TermSetObserver(const TermSet& termset): pterm_set_(&termset) {}
    bool operator () (termid_type termid) const {
        return pterm_set_->search(termid);
    }
private:
    const TermSet* pterm_set_;
};

// factory method: create a Predicate function boject
inline TermSetObserver care_in(const TermSet& termset) {
    return TermSetObserver(termset);
}


// class FreqVecObserver, to query frequency of a termid
class FreqVecObserver {
public:
    FreqVecObserver(const FreqVector& freqvec): pfreq_vec_(&freqvec) {}
    size_type operator () (termid_type termid) const {
        return (*pfreq_vec_)[termid];
    }
private:
    const FreqVector* pfreq_vec_;
};

// factory method: create a UnaryFunction function object
inline FreqVecObserver freq_dict(const FreqVector& freqvec) {
    return FreqVecObserver(freqvec);
}

}  // namespace waf

#endif  // WAF_FACILITY_H_
