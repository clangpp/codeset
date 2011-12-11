// ac.h
// an implement of Aho-Corasick automation algorithm
// mainly simulate std::map's design concept
// Author: Yongtian Zhang (yongtianzhang@gmail.com)
// Created At: 2011.11.03
// Last Modified At: 2011.11.11
#ifndef AC_H_
#define AC_H_

// configuration
// =============================================================================
// #define AC_USING_CPP11
#define AC_USING_BOOST
// #define AC_USING_CPP98

// include header files
// =============================================================================
#include <cstddef>
#include <iterator>
#include <utility>
#include <stdexcept>

#if defined(AC_USING_CPP11)  // using c++11
	#include <unordered_map>
#elif defined(AC_USING_BOOST)  // using boost
	#include <boost/unordered_map.hpp>
#else  // using c++98
	#include <map>
#endif  // AC_USING_*

namespace ac {

// node to form trie
// =============================================================================
template <typename Character, typename Mapped>
struct TrieNode {
public:
	typedef Character character;
	typedef Mapped mapped_type;
	typedef TrieNode self;

#if defined(AC_USING_CPP11)  // using c++11
	typedef std::unordered_map<character, self> child_table;
#elif defined(AC_USING_BOOST)  // using boost
	typedef boost::unordered_map<character, self> child_table;
#else  // using c++98
	typedef std::map<character, self> child_table;
#endif  // AC_USING_*

	typedef typename child_table::iterator child_iterator;

public:
	self* parent;
	child_table children;
	character route;
	std::ptrdiff_t height;
	self* fail_pointer;

public:
	TrieNode(): parent(NULL), route(character()), height(0), mapped_(NULL) {}
	~TrieNode() { erase_mapped(); }
	void set_mapped(const mapped_type& mapped);
	bool has_mapped() const { return mapped_!=NULL; }
	mapped_type& mapped();
	const mapped_type& mapped() const;
	void erase_mapped();
	bool eos() const { return has_mapped(); }

private:
	mapped_type* mapped_;
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
template <typename Character, typename Mapped=bool>
class Automation{
public:
	typedef TrieNode<Character, Mapped> node;
	typedef node* node_pointer;
	typedef const node* const_node_pointer;
	typedef typename node::character character;
	typedef typename node::mapped_type mapped_type;

public:
	Automation(): fail_pointers_updated_(false) {}

	// insert a subsequence into automation
	// return whether operation taken place
	template <typename ForwardIterator>
	bool insert(ForwardIterator first, ForwardIterator last,
			const mapped_type& mapped=mapped_type());
	template <typename ForwardIterator>
	bool insert(const std::pair<ForwardIterator, ForwardIterator>& range,
			const mapped_type& mapped=mapped_type()) {
		return insert(range.first, range.second, mapped);
	}
	template <typename Container>
	bool insert(const Container& c, const mapped_type& mapped=mapped_type()) {
		return insert(c.begin(), c.end(), mapped);
	}

	// remove a subsequence from automation
	// return whether operation taken place
	template <typename ForwardIterator>
	bool erase(ForwardIterator first, ForwardIterator last);
	template <typename Container>
	bool erase(Container& c) { return erase(c.begin(), c.end()); }
	template <typename ForwardIterator>
	bool erase(const std::pair<ForwardIterator, ForwardIterator>& range) {
		return erase(range.first, range.second);
	}

	// empty the whole automation
	void clear();
	bool empty() const;

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
	template <typename BidirectionalIterator, typename NodePointer>
	bool match_first(
			BidirectionalIterator first, BidirectionalIterator last,
			NodePointer& cursor,
			std::pair<BidirectionalIterator, BidirectionalIterator>& fail,
			std::pair<NodePointer, NodePointer>& fail_node);

	// node_pointer root_pointer() { return &root_; }
	const_node_pointer root_pointer() const { return &root_; }

protected:
	template <typename ForwardIterator>
	bool insert(ForwardIterator first, ForwardIterator last,
			const mapped_type& mapped, node_pointer& eos_node);

	void update_fail_pointers();

protected:
	node root_;
	bool fail_pointers_updated_;
};

// namespace free functions
// =============================================================================
template <typename ForwardIterator, typename NodePointer>
void match(ForwardIterator first, ForwardIterator last, NodePointer& cursor,
		std::pair<ForwardIterator, ForwardIterator>& mismatch,
		std::pair<NodePointer, NodePointer>& mismatch_node);

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
template <typename BidirectionalIterator, typename Character, typename Mapped>
std::size_t count(BidirectionalIterator first, BidirectionalIterator last,
		Automation<Character, Mapped>& subset);

// subsequence set
// =============================================================================
template <typename Character, typename Mapped = bool>
class SubsequenceSet: public Automation<Character, Mapped> {
public:
	typedef Automation<Character, Mapped> base;
	typedef typename base::mapped_type mapped_type;
	typedef typename base::node_pointer node_pointer;

	template <typename BidirectionalIterator>
	struct match_result:
		public std::pair<BidirectionalIterator, BidirectionalIterator> {
	public:
		typedef std::pair<BidirectionalIterator, BidirectionalIterator> base;
		match_result(BidirectionalIterator first, BidirectionalIterator last,
				node_pointer pnode=NULL): base(first, last),  pnode_(pnode){}
		match_result(): base(), pnode_(NULL) {}
		mapped_type& mapped() { return pnode_->mapped(); }
	private:
		node_pointer pnode_;
	};

	template <typename BidirectionalIterator,
			 typename Character1, typename Mapped1>
	friend class SubsequenceIterator;

public:
	template <typename ForwardIterator>
	mapped_type& operator [] (const std::pair<ForwardIterator, ForwardIterator>& range);
	template <typename Container>
	mapped_type& operator [] (const Container& c) {
		return (*this)[std::make_pair(c.begin(), c.end())];
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
	SubsequenceIterator(sequence_iterator first, sequence_iterator last,
			subsequence_set& subset);

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

	std::pair<sequence_iterator, sequence_iterator> fail_;
	std::pair<node_pointer, node_pointer> fail_node_;
	node_pointer cursor_;
	bool found_;

	value_type match_;
};  // class SubsequenceIterator

}  // namespace ac

#include "ac-inl.h"

#endif  // AC_H_
