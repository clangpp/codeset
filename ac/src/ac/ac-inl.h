// ac-inl.h
#ifndef AC_INL_H_
#define AC_INL_H_

#include "ac.h"

#include <queue>

namespace ac {

// struct TrieNode<Character, Mapped>
// =============================================================================
template <typename Character, typename Mapped>
void TrieNode<Character, Mapped>::set_mapped(const mapped_type& mapped) {
	if (has_mapped())
		*mapped_ = mapped;
	else
		mapped_ = new mapped_type(mapped);
}

template <typename Character, typename Mapped>
typename TrieNode<Character, Mapped>::mapped_type&
	TrieNode<Character, Mapped>::mapped() {
	if (!has_mapped())
		throw std::runtime_error("TrieNode::mapped(): no value error");
	return *mapped_;
}

template <typename Character, typename Mapped>
const typename TrieNode<Character, Mapped>::mapped_type&
	TrieNode<Character, Mapped>::mapped() const {
	if (!has_mapped())
		throw std::runtime_error("TrieNode::mapped() const: no value error");
	return *mapped_;
}

template <typename Character, typename Mapped>
void TrieNode<Character, Mapped>::erase_mapped() {
	if (!has_mapped()) return;
	delete mapped_;
	mapped_ = NULL;
}


// class Automation<T, AutomationNodeT>
// =============================================================================
template <typename Character, typename Mapped>
template <typename ForwardIterator>
bool Automation<Character, Mapped>::insert(
		ForwardIterator first, ForwardIterator last, const mapped_type& mapped) {
	node_pointer p = NULL;
	return insert(first, last, mapped, p);
}

template <typename Character, typename Mapped>
template <typename ForwardIterator>
bool Automation<Character, Mapped>::erase(
		ForwardIterator first, ForwardIterator last) {
	typedef typename node::child_iterator child_iterator;

	// check whether route exists in trie tree
	node_pointer p = &root_;
	for (ForwardIterator curr=first; curr!=last; ++curr) {
		child_iterator child = p->children.find(*curr);
		if (p->children.end() == child) return false;  // not found, do nothing
		p = &(child->second);
	}

	// remove related nodes
	p->erase_mapped();  // clear node mapped value (eos flag)
	while (p!=&root_ && !p->eos() &&
			p->children.empty()) {  // no further path, remove this path
		node_pointer s = p->parent;
		s->children.erase(p->route);
		p = s;
	}
	fail_pointers_updated_ = false;
	return true;
}

template <typename Character, typename Mapped>
void Automation<Character, Mapped>::clear() {
	root_.erase_mapped();
	root_.children.clear();
	fail_pointers_updated_ = false;
}

template <typename Character, typename Mapped>
bool Automation<Character, Mapped>::empty() const {
	return root_.children.empty() && !root_.has_mapped();
}

template <typename Character, typename Mapped>
template <typename ForwardIterator>
inline std::pair<ForwardIterator, ForwardIterator>
		Automation<Character, Mapped>::mismatch(
		ForwardIterator first, ForwardIterator last) const {
	std::pair<ForwardIterator, ForwardIterator> mismatch;
	std::pair<const_node_pointer, const_node_pointer> mismatch_node;
	const_node_pointer cursor = &root_;
	match(first, last, cursor, mismatch, mismatch_node);
	return mismatch;
}

template <typename Character, typename Mapped>
template <typename BidirectionalIterator, typename NodePointer>
bool Automation<Character, Mapped>::match_first(
		BidirectionalIterator first, BidirectionalIterator last,
		NodePointer& cursor,
		std::pair<BidirectionalIterator, BidirectionalIterator>& fail,
		std::pair<NodePointer, NodePointer>& fail_node) {
	if (!fail_pointers_updated_)  // confirm fail pointers
		update_fail_pointers();
	for (BidirectionalIterator curr=first; curr!=last;) {

		// find the first mismatch
		match(curr, last, cursor, fail, fail_node);

		// prepare for next search
		cursor = cursor->fail_pointer;  // jump to next node
		if (fail.second != curr) {  // one subsequence match found
			return true;
		} else if (fail.first != curr) {  // matched some characters
			curr = fail.first;
		} else {  // no even one character match
			++ curr;
		}
	}
	return false;
}

template <typename Character, typename Mapped>
void Automation<Character, Mapped>::update_fail_pointers() {
	typedef typename node::child_iterator child_iterator;

	// fail pointer of root_ is root_ itself (for unity)
	root_.fail_pointer = &root_;

	// push children of root_ into queue
	std::queue<node_pointer> node_queue;
	for (child_iterator iter = root_.children.begin();
			iter!=root_.children.end(); ++iter) {
		node_pointer p = &(iter->second);
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
			node_pointer pchild = &(child->second);
			node_queue.push(pchild);  // push the child into queue

			// look for the same key to fail pointer's children
			child_iterator fail_child = pfail->children.find(child->first);
			pchild->fail_pointer = fail_child!=pfail->children.end() ?
				&(fail_child->second) : &root_;
		}
	}

	// set updated flag
	fail_pointers_updated_ = true;
}

template <typename Character, typename Mapped>
template <typename ForwardIterator>
bool Automation<Character, Mapped>::insert(
		ForwardIterator first, ForwardIterator last,
		const mapped_type& mapped, node_pointer& eos_node) {
	eos_node = &root_;
	for (; first!=last; ++first) {
		node_pointer s = &(eos_node->children[*first]);
		s->parent = eos_node;
		s->route = *first;
		s->height = eos_node->height + 1;
		eos_node = s;
	}
	bool already_exist = eos_node->eos();
	if (!already_exist) {
		eos_node->set_mapped(mapped);
		fail_pointers_updated_ = false;
	}
	return !already_exist;
}

// namespace free functions
// =============================================================================
template <typename ForwardIterator, typename NodePointer>
void match(
		ForwardIterator first, ForwardIterator last, NodePointer& cursor,
		std::pair<ForwardIterator, ForwardIterator>& mismatch,
		std::pair<NodePointer, NodePointer>& mismatch_node) {
	mismatch = std::make_pair(first, first);
	mismatch_node = std::make_pair(cursor, cursor);
	for (ForwardIterator curr=first; curr!=last;) {
		typename node_pointer_traits<NodePointer>::child_iterator iter = 
			cursor->children.find(*curr);
		if (cursor->children.end() == iter) break;  // no more match

		cursor = &(iter->second);  // cursor move to next value
		++ curr;  // the off-by-one matter

		mismatch.first = curr;
		mismatch_node.first = cursor;
		if (cursor->eos()) {
			mismatch.second = curr;
			mismatch_node.second = cursor;
		}
	}
}

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

template <typename BidirectionalIterator, typename Character, typename Mapped>
std::size_t count(BidirectionalIterator first, BidirectionalIterator last,
		Automation<Character, Mapped>& subset) {
	typedef typename
		Automation<Character, Mapped>::const_node_pointer node_pointer;
	std::pair<BidirectionalIterator, BidirectionalIterator> fail;
	std::pair<node_pointer, node_pointer> fail_node;
	node_pointer cursor = subset.root_pointer();
	BidirectionalIterator curr = first;
	std::size_t amount = 0;
	while (subset.match_first(curr, last, cursor, fail, fail_node)) {
		++ amount;
		curr = fail.first;
	}
	return amount;
}

// subsequence set
// =============================================================================
template <typename Character, typename Mapped>
template <typename ForwardIterator>
typename SubsequenceSet<Character, Mapped>::mapped_type& 
		SubsequenceSet<Character, Mapped>::operator [] (
		const std::pair<ForwardIterator, ForwardIterator>& range) {
	node_pointer p = NULL;
	base::insert(range.first, range.second, mapped_type(), p);
	return p->mapped();
}

// subsequence iterator
// =============================================================================
template <typename BidirectionalIterator, typename Mapped, typename Character>
inline SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		SubsequenceIterator(sequence_iterator first, sequence_iterator last,
		subsequence_set& subset): begin_(first), end_(last), psubset_(&subset),
		fail_(), fail_node_(), cursor_(&(subset.root_)), found_(true), match_() {
	find_match();
}

template <typename BidirectionalIterator, typename Mapped, typename Character>
inline SubsequenceIterator<BidirectionalIterator, Mapped, Character>::
		SubsequenceIterator(): begin_(), end_(), psubset_(NULL), fail_(),
		fail_node_(), cursor_(NULL), found_(false), match_() {}

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
	begin_ = fail_.first;
	if (found_) {
		match_.first = match_.second = fail_.second;
		std::advance(match_.first, -fail_node_.second->height);
		match_ = value_type(match_.first, match_.second, fail_node_.second);
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
