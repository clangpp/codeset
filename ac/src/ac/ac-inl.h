// ac-inl.h
#ifndef AC_INL_H_
#define AC_INL_H_

#include "ac.h"

#include <queue>

namespace ac {

// agreement
// =============================================================================
// 1. TrieNode::children has no empty pointer element

// struct TrieNode<Character, T>
// =============================================================================
template <typename Character, typename T>
TrieNode<Character, T>::~TrieNode() {
	erase_value();
	clear_children();
}

template <typename Character, typename T>
void TrieNode<Character, T>::set_value(const value_type& value) {
	if (has_value())
		*pvalue_ = value;
	else
		pvalue_ = new value_type(value);
}

template <typename Character, typename T>
typename TrieNode<Character, T>::value_type&
	TrieNode<Character, T>::value() {
	if (!has_value())
		throw std::runtime_error("TrieNode::value(): no value error");
	return *pvalue_;
}

template <typename Character, typename T>
const typename TrieNode<Character, T>::value_type&
	TrieNode<Character, T>::value() const {
	if (!has_value())
		throw std::runtime_error("TrieNode::value() const: no value error");
	return *pvalue_;
}

template <typename Character, typename T>
void TrieNode<Character, T>::erase_value() {
	if (!has_value()) return;
	delete pvalue_;
	pvalue_ = NULL;
}

template <typename Character, typename T>
void TrieNode<Character, T>::clear_children() {
	for (typename child_table::iterator iter=children.begin();
			iter!=children.end(); ++iter) {
		delete iter->second;
	}
	children.clear();
}

// class Automation<Character, T>
// =============================================================================
template <typename Character, typename T>
template <typename InputIterator>
bool Automation<Character, T>::insert(
		InputIterator first, InputIterator last,
		const mapped_type& value, node_pointer& eos_node) {
	eos_node = &root_;
	for (; first!=last; ++first) {
		node_pointer& s = eos_node->children[*first];
		if (NULL == s) s = new node();
		s->parent = eos_node;
		s->route = *first;
		s->height = eos_node->height + 1;
		eos_node = s;
	}
	bool already_exist = eos_node->eos();
	if (!already_exist) {
		eos_node->set_value(value);
		fail_pointers_updated_ = false;
		++counter_;
	}
	return !already_exist;
}

template <typename Character, typename T>
template <typename InputIterator>
bool Automation<Character, T>::erase(
		InputIterator first, InputIterator last) {
	typedef typename
		node_pointer_traits<node_pointer>::child_iterator child_iterator;

	// check whether route exists in trie tree
	node_pointer p = &root_;
	for (InputIterator curr=first; curr!=last; ++curr) {
		child_iterator child = p->children.find(*curr);
		if (p->children.end() == child) return false;  // not found, do nothing
		p = child->second;
	}

	// remove related nodes
	p->erase_value();  // clear node mapped value (eos flag)
	while (p!=&root_ && !p->eos() &&
			p->children.empty()) {  // no further path, remove this path
		node_pointer s = p->parent;
		s->children.erase(p->route);
		delete p;
		p = s;
	}
	fail_pointers_updated_ = false;
	--counter_;
	return true;
}

template <typename Character, typename T>
void Automation<Character, T>::clear() {
	root_.erase_value();
	root_.clear_children();
	fail_pointers_updated_ = false;
	counter_ = 0;
}

template <typename Character, typename T>
bool Automation<Character, T>::empty() const {
	return root_.children.empty() && !root_.has_value();
}

template <typename Character, typename T>
template <typename ForwardIterator>
inline std::pair<ForwardIterator, ForwardIterator>
		Automation<Character, T>::mismatch(
		ForwardIterator first, ForwardIterator last) const {
	std::pair<ForwardIterator, ForwardIterator> mismatch;
	std::pair<const_node_pointer, const_node_pointer> mismatch_node;
	const_node_pointer cursor = &root_;
	internal::match(first, last, cursor, mismatch, mismatch_node);
	return mismatch;
}

template <typename Character, typename T>
template <typename ForwardIterator, typename NodePointer>
bool Automation<Character, T>::match_first(
		ForwardIterator first, ForwardIterator last,
		NodePointer& cursor,
		std::pair<ForwardIterator, ForwardIterator>& fail,
		std::pair<NodePointer, NodePointer>& fail_node) {
	if (!fail_pointers_updated_)  // confirm fail pointers
		update_fail_pointers();
	for (ForwardIterator curr=first; curr!=last;) {
		bool search_from_root = (&root_==cursor);

		// find the first mismatch
		internal::match(curr, last, cursor, fail, fail_node);

		// prepare for next search
		cursor = cursor->fail_pointer;  // jump to next node
		if (fail.second != curr) {  // one subsequence match found
			return true;
		} else if (fail.first != curr) {  // matched some characters
			curr = fail.first;
		} else if (!search_from_root) {  // search not start from root
			continue;
		} else {  // no even one character match
			++ curr;
		}
	}
	return false;
}

template <typename Character, typename T>
void Automation<Character, T>::update_fail_pointers() {
	typedef typename
		node_pointer_traits<node_pointer>::child_iterator child_iterator;

	// fail pointer of root_ is root_ itself (for unity)
	root_.fail_pointer = &root_;

	// push children of root_ into queue
	std::queue<node_pointer> node_queue;
	for (child_iterator iter = root_.children.begin();
			iter!=root_.children.end(); ++iter) {
		node_pointer p = iter->second;
		p->fail_pointer = &root_;  // fail pointer set to root_
		node_queue.push(p);
	}

	// BFS, to calculate fail pointers
	for (; !node_queue.empty(); node_queue.pop()) {
		node_pointer p = node_queue.front();
		node_pointer pfail = p->fail_pointer;

		// for each child of the current node
		for (child_iterator child = p->children.begin();
				child!=p->children.end(); ++child) {
			node_pointer pchild = child->second;
			node_queue.push(pchild);  // push the child into queue

			// look for the same key to fail pointer's children
			child_iterator fail_child = pfail->children.find(child->first);
			pchild->fail_pointer = fail_child!=pfail->children.end() ?
				fail_child->second : &root_;
		}
	}

	// set updated flag
	fail_pointers_updated_ = true;
}

// namespace free functions
// =============================================================================
namespace internal {

template <typename InputIterator, typename NodePointer>
bool find(InputIterator first, InputIterator last, NodePointer& cursor) {
	typedef typename
		node_pointer_traits<NodePointer>::child_iterator child_iterator;

	// check whether route exists in trie tree
	for (InputIterator curr=first; curr!=last; ++curr) {
		child_iterator child = cursor->children.find(*curr);
		if (cursor->children.end() == child)  // not found, do nothing
			return false;
		cursor = child->second;
	}
	return true;
}

template <typename InputIterator, typename NodePointer>
void match(
		InputIterator first, InputIterator last, NodePointer& cursor,
		std::pair<InputIterator, InputIterator>& mismatch,
		std::pair<NodePointer, NodePointer>& mismatch_node) {
	mismatch = std::make_pair(first, first);
	mismatch_node = std::make_pair(cursor, cursor);
	for (InputIterator curr=first; curr!=last;) {
		typename node_pointer_traits<NodePointer>::child_iterator iter = 
			cursor->children.find(*curr);
		if (cursor->children.end() == iter) break;  // no more match

		cursor = iter->second;  // cursor move to next value
		++ curr;  // the off-by-one matter

		mismatch.first = curr;
		mismatch_node.first = cursor;
		if (cursor->eos()) {
			mismatch.second = curr;
			mismatch_node.second = cursor;
		}
	}
}

}  // namespace internal

template <typename BidirectionalIterator, typename Character, typename Mapped>
inline std::pair<BidirectionalIterator, BidirectionalIterator> find_first_of(
		BidirectionalIterator first, BidirectionalIterator last,
		Automation<Character, Mapped>& subset) {
	typedef typename
		Automation<Character, Mapped>::const_node_pointer node_pointer;

	// find first matched subsequence
	std::pair<BidirectionalIterator, BidirectionalIterator> fail;
	std::pair<node_pointer, node_pointer> fail_node;
	node_pointer cursor = subset.root_pointer();
	bool found = subset.match_first(first, last, cursor, fail, fail_node);

	// make result
	if (found) {
		fail.first = fail.second;
		std::advance(fail.first, -fail_node.second->height);
		return fail;
	} else {
		return make_pair(last, last);
	}
}

template <typename BidirectionalIterator, typename Character, typename Mapped>
inline bool search(BidirectionalIterator first, BidirectionalIterator last,
		Automation<Character, Mapped>& subset) {
	return find_first_of(first, last, subset).first != last;
}

template <typename ForwardIterator, typename Character, typename Mapped>
std::size_t count(ForwardIterator first, ForwardIterator last,
		Automation<Character, Mapped>& subset) {
	typedef typename
		Automation<Character, Mapped>::const_node_pointer node_pointer;
	std::pair<ForwardIterator, ForwardIterator> fail;
	std::pair<node_pointer, node_pointer> fail_node;
	node_pointer cursor = subset.root_pointer();
	ForwardIterator curr = first;
	std::size_t amount = 0;
	while (subset.match_first(curr, last, cursor, fail, fail_node)) {
		++ amount;
		curr = fail.first;
	}
	return amount;
}

// TrieMap
// =============================================================================
template <typename Character, typename T>
template <typename InputIterator>
std::pair<typename TrieMap<Character, T>::iterator, bool>
		TrieMap<Character, T>::insert(
		InputIterator first, InputIterator last, const mapped_type& value) {
	node_pointer pnode;
	bool taken_place = base::insert(first, last, value, pnode);
	return std::make_pair(
			iterator(pnode->parent, pnode->parent->children.find(pnode->route)),
			taken_place);
}

template <typename Character, typename T>
template <typename InputIterator>
typename TrieMap<Character, T>::iterator TrieMap<Character, T>::find(
		InputIterator first, InputIterator last) {
	node_pointer pnode = base::root_pointer();
	if (internal::find(first, last, pnode) && pnode->has_value()) {
            return iterator(pnode->parent,
                    pnode->parent->children.find(pnode->route));
	} else {
		return end();
	}
}

template <typename Character, typename T>
template <typename InputIterator>
typename TrieMap<Character, T>::const_iterator TrieMap<Character, T>::find(
		InputIterator first, InputIterator last) const {
	const_node_pointer pnode = base::root_pointer();
	if (internal::find(first, last, pnode) && pnode->has_value()) {
		return const_iterator(pnode->parent,
				pnode->parent->children.find(pnode->route));
	} else {
		return end();
	}
}

template <typename Character, typename T>
template <typename InputIterator>
typename TrieMap<Character, T>::mapped_type&
		TrieMap<Character, T>::operator [] (
		const std::pair<InputIterator, InputIterator>& range) {
	node_pointer p = NULL;
	base::insert(range.first, range.second, mapped_type(), p);
	return p->value();
}

// subsequence iterator
// =============================================================================
template <typename BidirectionalIterator, typename Mapped, typename Character>
inline SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		SubsequenceIterator(sequence_iterator first, sequence_iterator last,
		subsequence_set& subset, bool overlap): begin_(first), end_(last),
		psubset_(&subset), overlap_(overlap), fail_(), fail_node_(),
		cursor_(subset.root_pointer()), found_(true), match_() {
	find_match();
}

template <typename BidirectionalIterator, typename Mapped, typename Character>
inline SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		SubsequenceIterator(): begin_(), end_(), psubset_(NULL), overlap_(true),
		fail_(), fail_node_(), cursor_(NULL), found_(false), match_() {}

template <typename BidirectionalIterator, typename Mapped, typename Character>
inline bool SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		operator == (const SubsequenceIterator& rhs) const {
    if (end_of_sequence() && rhs.end_of_sequence())  // both end-of-sequence
        return true;
    else if (end_of_sequence() || rhs.end_of_sequence())  // not both end-of-seq
        return false;
    else {  // both not end-of-sequence
        return begin_==rhs.begin_ && end_==rhs.end_ &&
            psubset_==rhs.psubset_ && match_==rhs.match_;  // field compare
    }
}

template <typename BidirectionalIterator, typename Mapped, typename Character>
inline SubsequenceIterator<BidirectionalIterator, Mapped, Character>&
    SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		operator ++ () {
	find_match();
    return *this;
}

template <typename BidirectionalIterator, typename Mapped, typename Character>
inline SubsequenceIterator<BidirectionalIterator, Mapped, Character>
    SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		operator ++ (int) {
    SubsequenceIterator iter(*this);
    ++(*this);
    return iter;
}

template <typename BidirectionalIterator, typename Mapped, typename Character>
inline void SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		find_match() {
    if (end_of_sequence()) return;
	found_ = psubset_->match_first(begin_, end_, cursor_, fail_, fail_node_);

	if (found_) {
		match_.first = match_.second = fail_.second;
		std::advance(match_.first, -fail_node_.second->height);
		match_ = value_type(match_.first, match_.second, fail_node_.second);
		if (overlap_) {
			begin_ = fail_.first;
		} else {  // overlap match not allowed, restart from match_.second;
			begin_ = fail_.second;
			cursor_ = psubset_->root_pointer();
		}
	} else {
		match_.first = match_.second = end_;
	}
}

template <typename BidirectionalIterator, typename Mapped, typename Character>
inline bool SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		end_of_sequence() const {
	return NULL==psubset_ || !found_; 
}

}  // namespace ac

#endif  // AC_INL_H_
