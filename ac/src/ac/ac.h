// ac.h
// an implement of Aho-Corasick automation algorithm
// mainly simulate std::map's design concept
// Author: Yongtian Zhang (yongtianzhang@gmail.com)
// Created At: 2011.11.03
// Last Modified At: 2011.12.07

#ifndef AC_H_
#define AC_H_

// configuration macro
// =============================================================================
// #define AC_USING_CPP11
// #define AC_USING_BOOST
#define AC_USING_CPP98

// include header files
// =============================================================================
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

// configuration specific types
// =============================================================================

#if defined(AC_USING_CPP11)  // using c++11

#include <unordered_map>
namespace ac {
	template <typename Key, typename T>
	class Map: public std::unordered_map<Key, T> {};
}  // namespace ac

#elif defined(AC_USING_BOOST)  // using boost

#include <boost/unordered_map.hpp>
namespace ac {
	template <typename Key, typename T>
	class Map: public boost::unordered_map<Key, T> {};
}  // namespace ac

#else  // using c++98

#include <map>
namespace ac {
	template <typename Key, typename T>
	class Map: public std::map<Key, T> {};
}  // namespace ac

#endif  // AC_USING_*


namespace ac {

// node to form trie
// =============================================================================
template <typename Character, typename T>
struct TrieNode {
public:
	typedef Character character_type;
	typedef T value_type;
	typedef TrieNode self;
	typedef Map<character_type, self*> child_table;

public:
	self* parent;
	child_table children;
	character_type route;
	std::ptrdiff_t height;
	self* fail_pointer;

public:
	TrieNode(): parent(NULL), route(character_type()), height(0), pvalue_(NULL) {}
	~TrieNode();

	void set_value(const value_type& value);
	bool has_value() const { return pvalue_!=NULL; }
	value_type& value();
	const value_type& value() const;
	void erase_value();

	bool eos() const { return has_value(); }

	void clear_children();

private:
	value_type* pvalue_;
};

// node pointer traits
template <typename NodePointer> struct node_pointer_traits;
template <typename TrieNodeT>
struct node_pointer_traits<TrieNodeT*> {
	typedef typename TrieNodeT::child_table::iterator child_iterator;
};
template <typename TrieNodeT>
struct node_pointer_traits<const TrieNodeT*> {
	typedef typename TrieNodeT::child_table::const_iterator child_iterator;
};

// AC automation
// =============================================================================
template <typename Character, typename T>
class Automation{
public:
	typedef TrieNode<Character, T> node;
	typedef Character character_type;
	typedef T mapped_type;
	typedef node* node_pointer;
	typedef const node* const_node_pointer;
	typedef std::size_t size_type;

public:
	Automation(): fail_pointers_updated_(false), counter_(0) {}

	// insert a subsequence into automation
	// return whether operation taken place
	template <typename InputIterator>
	bool insert(InputIterator first, InputIterator last,
			const mapped_type& value, node_pointer& eos_node);

	// remove a subsequence from automation
	// return whether operation taken place
	template <typename InputIterator>
	bool erase(InputIterator first, InputIterator last);

	// // find a subsequence in automation
	// // return whether found or not
	// // post-condition: if returned true, eos_node denotes the result.
	// template <typename InputIterator, typename NodePointer>
	// bool find(InputIterator first, InputIterator last,
	// 		NodePointer& eos_node) const;

	// empty automation
	void clear();
	bool empty() const;
	size_type size() const { return counter_; }

	// find first mismatch position between [first, last) and *this
	// with returned pair /r/:
	//	[first, r.first) is the longest unspecified match
	//	[first, r.second) is the longest subsequence match
	template <typename ForwardIterator>
	std::pair<ForwardIterator, ForwardIterator> mismatch(
		ForwardIterator first, ForwardIterator last) const;

	// match first subsequence in [first, last), start from /cursor/
	// return whether a match found
	// if returns true, then with the first match position /sub/:
	//	[sub, fail.first) is the longest unspecified match,
	//		fail_node.first is pointer to the mismatch code
	//	[sub, fail.second) is the longest subsequence match
	//		fail_node.second is pointer to the mismatch code
	// pre-condition: /cursor/ must be in Automation
	// post-condition: cursor == fail_node.first
	template <typename ForwardIterator, typename NodePointer>
	bool match_first(
			ForwardIterator first, ForwardIterator last, NodePointer& cursor,
			std::pair<ForwardIterator, ForwardIterator>& fail,
			std::pair<NodePointer, NodePointer>& fail_node);

	node_pointer root_pointer() { return &root_; }
	const_node_pointer root_pointer() const { return &root_; }

private:
	void update_fail_pointers();
    node_pointer find_fail_pointer(
        node_pointer parent_fail_pointer, const character_type& route);

private:
	node root_;
	bool fail_pointers_updated_;
	size_type counter_;
};

// namespace free functions
// =============================================================================
namespace internal {
// look for subsequence [first, last) in trie tree denoted by /cursor/
// return whether found or not. if found, /cursor/ denotes the position
template <typename InputIterator, typename NodePointer>
bool find(InputIterator first, InputIterator last, NodePointer& cursor);

// try to find match in trie tree, cursor denotes the begin searching position.
template <typename InputIterator, typename NodePointer>
void match(InputIterator first, InputIterator last, NodePointer& cursor,
		std::pair<InputIterator, InputIterator>& mismatch,
		std::pair<NodePointer, NodePointer>& mismatch_node);
}  // namespace internal

// find first subsequence in /subset/ in [first, last)
// return subsequence range in [first, last) if found, [last, last) otherwise
template <typename BidirectionalIterator, typename Character, typename Mapped>
std::pair<BidirectionalIterator, BidirectionalIterator> find_first_of(
		BidirectionalIterator first, BidirectionalIterator last,
		Automation<Character, Mapped>& subset);

// check whether any subsequence in /subset/ can be found in [first, last)
template <typename BidirectionalIterator, typename Character, typename Mapped>
bool search(BidirectionalIterator first, BidirectionalIterator last,
		Automation<Character, Mapped>& subset);

// how many subsequence in /subset/ can be found in [first, last)
template <typename ForwardIterator, typename Character, typename Mapped>
std::size_t count(ForwardIterator first, ForwardIterator last,
		Automation<Character, Mapped>& subset);

// trie map
// perform std::map interface or like
// =============================================================================
template <typename Character, typename T>
class TrieMap: public Automation<Character, T> {
public:
	typedef Automation<Character, T> base;
	typedef typename base::character_type character_type;
	typedef typename base::mapped_type mapped_type;
	typedef typename base::node node;
	typedef typename base::node_pointer node_pointer;
	typedef typename base::const_node_pointer const_node_pointer;
	typedef std::size_t size_type;

    template <typename NodePointer> class basic_iterator;
	typedef basic_iterator<node_pointer> iterator;
	typedef basic_iterator<const_node_pointer> const_iterator;

	// TBD: to be implemented.
	template <typename NodePointer>
	class basic_iterator {
	public:
		typedef NodePointer node_pointer;
		typedef typename
			node_pointer_traits<node_pointer>::child_iterator child_iterator;
        typedef std::vector<typename TrieMap::character_type> key_type;
        typedef typename TrieMap::mapped_type value_type;
		template <typename NodePointer2> friend class basic_iterator;
	public:
		basic_iterator(node_pointer p, child_iterator pos):
			parent_(p), position_(pos) {}
		basic_iterator(): parent_(NULL), position_() {}
        basic_iterator(const iterator& other): parent_(other.parent_),
                position_(other.position_), path_(other.path_) {}
        value_type& value() { return position_->second->value(); }
        const value_type& value() const { return position_->second->value(); }
        const key_type& key() const {
            if (path_.empty()) {  // calculate once
                node_pointer p = parent_;
                child_iterator pos = position_;
                path_.push_back(pos->first);  // nearest route
                while (p->parent != NULL) {  // back search
                    path_.push_back(p->route);  // record route
                    pos = p->parent->children.find(p->route);
                    p = p->parent;
                }
                std::reverse(path_.begin(), path_.end());  // natural order
            }
            return path_;
        }
		template <typename NodePointer2>
		bool operator == (const basic_iterator<NodePointer2>& other) const {
			return parent_==other.parent_ && position_==other.position_;
		}
		template <typename NodePointer2>
		bool operator != (const basic_iterator<NodePointer2>& other) const {
			return !(*this == other);
		}
	private:
		bool next();  // move to next node  // TBD.
		bool next_eos();  // move to next eos node  // TBD.
	private:
		node_pointer parent_;
		child_iterator position_;
        mutable key_type path_;  // it is const_iterator who push me to use 'mutable'!
	};

public:  // iterator observers
	iterator begin() {
		return iterator(base::root_pointer(),
				base::root_pointer()->children.begin());
	}
	iterator end() {
		return iterator(base::root_pointer(),
				base::root_pointer()->children.end());
	}
	const_iterator begin() const {
		return const_iterator(base::root_pointer(),
				base::root_pointer()->children.begin());
	}
	const_iterator end() const {
		return const_iterator(base::root_pointer(),
				base::root_pointer()->children.end());
	}

public:
	TrieMap() {}
	
	// insert
	template <typename InputIterator>
	std::pair<iterator, bool> insert(
			InputIterator first, InputIterator last, const mapped_type& value);
	template <typename InputIterator>
	std::pair<iterator, bool> insert(
			const std::pair<InputIterator, InputIterator>& range,
			const mapped_type& value) {
		return insert(range.first, range.second, value);
	}
	template <typename Container>
	std::pair<iterator, bool> insert(
			const Container& sequence, const mapped_type& value) {
		return insert(sequence.begin(), sequence.end(), value);
	}

	// erase
	template <typename InputIterator>
	bool erase(const std::pair<InputIterator, InputIterator>& range) {
		return base::erase(range.first, range.second);
	}
	template <typename Container>
	bool erase(const Container& sequence) {
		return base::erase(sequence.begin(), sequence.end());
	}

	// find
	template <typename InputIterator>
	iterator find(InputIterator first, InputIterator last);
	template <typename InputIterator>
	iterator find(const std::pair<InputIterator, InputIterator>& range) {
		return find(range.begin(), range.end());
	}
	template <typename Container>
	iterator find(const Container& sequence) {
		return find(sequence.begin(), sequence.end());
	}
	template <typename InputIterator>
	const_iterator find(InputIterator first, InputIterator last) const;
	template <typename InputIterator>
	const_iterator find(
			const std::pair<InputIterator, InputIterator>& range) const {
		return find(range.begin(), range.end());
	}
	template <typename Container>
	const_iterator find(const Container& sequence) const {
		return find(sequence.begin(), sequence.end());
	}

	// operator []
	template <typename InputIterator>
	mapped_type& operator [] (const std::pair<InputIterator, InputIterator>& range);
	template <typename Container>
	mapped_type& operator [] (const Container& c) {
		return (*this)[std::make_pair(c.begin(), c.end())];
	}

	// observers
	template <typename InputIterator>
	size_type count(InputIterator first, InputIterator last) const {
		return find(first, last)!=end() ? 1 : 0;
	}
	template <typename InputIterator>
	size_type count(const std::pair<InputIterator, InputIterator>& range) const {
		return count(range.first, range.second);
	}
	template <typename Container>
	size_type count(const Container& sequence) const {
		return count(sequence.begin(), sequence.end());
	}
	
public:
	using base::insert;
	using base::erase;
	using base::mismatch;
	using base::match_first;
};

// subsequence set
// =============================================================================
template <typename Character, typename Mapped = bool>
class SubsequenceSet: public TrieMap<Character, Mapped> {
public:
	typedef TrieMap<Character, Mapped> base;
	typedef typename base::mapped_type mapped_type;
	typedef typename base::node_pointer node_pointer;
	typedef typename base::iterator iterator;
	typedef typename base::const_iterator const_iterator;

	template <typename BidirectionalIterator>
	struct match_result:
		public std::pair<BidirectionalIterator, BidirectionalIterator> {
	public:
		typedef std::pair<BidirectionalIterator, BidirectionalIterator> base;
		match_result(BidirectionalIterator first, BidirectionalIterator last,
				node_pointer pnode=NULL): base(first, last),  pnode_(pnode){}
		match_result(): base(), pnode_(NULL) {}
		mapped_type& mapped() { return pnode_->value(); }
	private:
		node_pointer pnode_;
	};

	template <typename BidirectionalIterator,
			 typename Character1, typename Mapped1>
	friend class SubsequenceIterator;

public:
	using base::insert;
	template <typename InputIterator>
	std::pair<iterator, bool> insert(InputIterator first, InputIterator last) {
		return base::insert(first, last, mapped_type());
	}
	template <typename InputIterator>
	std::pair<iterator, bool> insert(
			const std::pair<InputIterator, InputIterator>& range) {
		return base::insert(range, mapped_type());
	}
	template <typename Container>
	std::pair<iterator, bool> insert(const Container& sequence) {
		return base::insert(sequence, mapped_type());
	}
};

// subsequence iterator
// =============================================================================
template <typename BidirectionalIterator, typename Mapped = bool,
		 typename Character =
		 typename std::iterator_traits<BidirectionalIterator>::value_type>
class SubsequenceIterator {
public:
	typedef SubsequenceSet<Character, Mapped> subsequence_set;
	typedef BidirectionalIterator sequence_iterator;
	typedef typename subsequence_set::node_pointer node_pointer;
	typedef typename subsequence_set::
		template match_result<BidirectionalIterator> value_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef std::ptrdiff_t difference_type;
	typedef std::forward_iterator_tag iterator_category;

public:
	// construct iterator with sequence [first, last) and subsequence set
	// /subset/. /overlap/ denotes whether matched words can overlap with
	// each other or not.
	SubsequenceIterator(sequence_iterator first, sequence_iterator last,
			subsequence_set& subset, bool overlap=true);

	// end-of-sequence iterator
	SubsequenceIterator();

	// compare two iterators
	bool operator == (const SubsequenceIterator& rhs) const;
	bool operator != (const SubsequenceIterator& rhs) const {
		return !(*this==rhs);
	}

	// standard observers of iterator
	// return a range (std::pair) donates matched range in sequence
	reference operator * () { return match_; }
	pointer operator -> () { return &match_; }

	// standard increment operator
	SubsequenceIterator& operator ++ ();
	SubsequenceIterator operator ++ (int);

private:
	// find next match result
	// pre-condition: !end_of_sequence();
	void find_match();

    // check whether this is end-of-sequence iterator
    bool end_of_sequence() const;

private:
	sequence_iterator begin_;
	sequence_iterator end_;
	subsequence_set* psubset_;
	bool overlap_;

	std::pair<sequence_iterator, sequence_iterator> fail_;
	std::pair<node_pointer, node_pointer> fail_node_;
	node_pointer cursor_;
	bool found_;

	value_type match_;
};  // class SubsequenceIterator

}  // namespace ac

#include "ac-inl.h"

#endif  // AC_H_
