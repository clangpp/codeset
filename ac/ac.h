// ac.h
// an implement of Aho-Corasick automation algorithm.
// simulates std::map's design concept

#ifndef AC_H_
#define AC_H_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ac {

// node to form trie
// =============================================================================
template <typename Character, typename T>
struct TrieNode {
 public:
  typedef Character character_type;
  typedef T value_type;
  typedef TrieNode self;
  typedef std::unordered_map<character_type, self*> child_table;

 public:
  self* parent = nullptr;
  child_table children;
  character_type route = {};
  std::ptrdiff_t height = 0;
  self* fail_pointer = nullptr;

 public:
  TrieNode() = default;
  ~TrieNode() {
    erase_value();
    clear_children();
  }

  void set_value(const value_type& value) {
    if (has_value()) {
      *pvalue_ = value;
    } else {
      pvalue_ = new value_type(value);
    }
  }

  bool has_value() const { return pvalue_ != NULL; }
  value_type& value() {
    if (!has_value()) {
      throw std::runtime_error("TrieNode::value(): no value error");
    }
    return *pvalue_;
  }

  const value_type& value() const {
    if (!has_value()) {
      throw std::runtime_error("TrieNode::value() const: no value error");
    }
    return *pvalue_;
  }

  void erase_value() {
    if (!has_value()) {
      return;
    }
    delete pvalue_;
    pvalue_ = NULL;
  }

  bool eos() const { return has_value(); }

  void clear_children() {
    for (typename child_table::iterator iter = children.begin();
         iter != children.end(); ++iter) {
      delete iter->second;
    }
    children.clear();
  }

 private:
  value_type* pvalue_ = nullptr;
};

// node pointer traits
template <typename NodePointer>
struct node_pointer_traits;
template <typename TrieNodeT>
struct node_pointer_traits<TrieNodeT*> {
  typedef typename TrieNodeT::child_table::iterator child_iterator;
};
template <typename TrieNodeT>
struct node_pointer_traits<const TrieNodeT*> {
  typedef typename TrieNodeT::child_table::const_iterator child_iterator;
};

// namespace free functions
// =============================================================================
namespace internal {
// look for subsequence [first, last) in trie tree denoted by /cursor/
// return whether found or not. if found, /cursor/ denotes the position
template <typename InputIterator, typename NodePointer>
bool find(InputIterator first, InputIterator last, NodePointer& cursor) {
  typedef
      typename node_pointer_traits<NodePointer>::child_iterator child_iterator;

  // check whether route exists in trie tree
  for (InputIterator curr = first; curr != last; ++curr) {
    child_iterator child = cursor->children.find(*curr);
    if (cursor->children.end() == child) {  // not found, do nothing
      return false;
    }
    cursor = child->second;
  }
  return cursor->has_value();
}

// try to find match in trie tree, cursor denotes the begin searching position.
template <typename InputIterator, typename NodePointer>
void match(InputIterator first, InputIterator last, NodePointer& cursor,
           std::pair<InputIterator, InputIterator>& mismatch,
           std::pair<NodePointer, NodePointer>& mismatch_node) {
  mismatch = std::make_pair(first, first);
  mismatch_node = std::make_pair(cursor, cursor);
  for (InputIterator curr = first; curr != last;) {
    typename node_pointer_traits<NodePointer>::child_iterator iter =
        cursor->children.find(*curr);
    if (cursor->children.end() == iter) break;  // no more match

    cursor = iter->second;  // cursor move to next value
    ++curr;                 // the off-by-one matter

    mismatch.first = curr;
    mismatch_node.first = cursor;
    if (cursor->eos()) {
      mismatch.second = curr;
      mismatch_node.second = cursor;
    }
  }
}

}  // namespace internal

// AC automation
// =============================================================================
template <typename Character, typename T>
class Automation {
 public:
  typedef TrieNode<Character, T> node;
  typedef Character character_type;
  typedef T mapped_type;
  typedef node* node_pointer;
  typedef const node* const_node_pointer;
  typedef std::size_t size_type;

 public:
  Automation() = default;

  // insert a subsequence into automation
  // return whether operation taken place
  template <typename InputIterator>
  bool insert(InputIterator first, InputIterator last, const mapped_type& value,
              node_pointer& eos_node) {
    eos_node = &root_;
    for (; first != last; ++first) {
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

  // remove a subsequence from automation
  // return whether operation taken place
  template <typename InputIterator>
  bool erase(InputIterator first, InputIterator last) {
    typedef typename node_pointer_traits<node_pointer>::child_iterator
        child_iterator;

    // check whether route exists in trie tree
    node_pointer p = &root_;
    for (InputIterator curr = first; curr != last; ++curr) {
      child_iterator child = p->children.find(*curr);
      if (p->children.end() == child) return false;  // not found, do nothing
      p = child->second;
    }

    // remove related nodes
    p->erase_value();  // clear node mapped value (eos flag)
    while (p != &root_ && !p->eos() &&
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

  // // find a subsequence in automation
  // // return whether found or not
  // // post-condition: if returned true, eos_node denotes the result.
  // template <typename InputIterator, typename NodePointer>
  // bool find(InputIterator first, InputIterator last,
  //     NodePointer& eos_node) const;

  // empty automation
  void clear() {
    root_.erase_value();
    root_.clear_children();
    fail_pointers_updated_ = false;
    counter_ = 0;
  }

  bool empty() const { return root_.children.empty() && !root_.has_value(); }

  size_type size() const { return counter_; }

  // find first mismatch position between [first, last) and *this
  // with returned pair /r/:
  //  [first, r.first) is the longest unspecified match
  //  [first, r.second) is the longest subsequence match
  template <typename ForwardIterator>
  std::pair<ForwardIterator, ForwardIterator> mismatch(
      ForwardIterator first, ForwardIterator last) const {
    std::pair<ForwardIterator, ForwardIterator> mismatch;
    std::pair<const_node_pointer, const_node_pointer> mismatch_node;
    const_node_pointer cursor = &root_;
    internal::match(first, last, cursor, mismatch, mismatch_node);
    return mismatch;
  }

  // match first subsequence in [first, last), start from /cursor/
  // return whether a match found
  // if returns true, then with the first match position /sub/:
  //  [sub, fail.first) is the longest unspecified match,
  //    fail_node.first is pointer to the mismatch code
  //  [sub, fail.second) is the longest subsequence match
  //    fail_node.second is pointer to the mismatch code
  // pre-condition: /cursor/ must be in Automation
  // post-condition: cursor == fail_node.first
  template <typename ForwardIterator, typename NodePointer>
  bool match_first(ForwardIterator first, ForwardIterator last,
                   NodePointer& cursor,
                   std::pair<ForwardIterator, ForwardIterator>& fail,
                   std::pair<NodePointer, NodePointer>& fail_node) {
    if (!fail_pointers_updated_) {  // confirm fail pointers
      update_fail_pointers();
    }
    for (ForwardIterator curr = first; curr != last;) {
      bool search_from_root = (&root_ == cursor);

      // find the first mismatch
      internal::match(curr, last, cursor, fail, fail_node);

      // prepare for next search
      cursor = cursor->fail_pointer;  // jump to next node
      if (fail.second != curr) {      // one subsequence match found
        return true;
      } else if (fail.first != curr) {  // matched some characters
        curr = fail.first;
      } else if (!search_from_root) {  // search not start from root
        continue;
      } else {  // no even one character match
        ++curr;
      }
    }
    return false;
  }

  node_pointer root_pointer() { return &root_; }
  const_node_pointer root_pointer() const { return &root_; }

 private:
  void update_fail_pointers() {
    typedef typename node_pointer_traits<node_pointer>::child_iterator
        child_iterator;

    // fail pointer of root_ is root_ itself (for unity)
    root_.fail_pointer = &root_;

    // push children of root_ into queue
    std::queue<node_pointer> node_queue;
    for (child_iterator iter = root_.children.begin();
         iter != root_.children.end(); ++iter) {
      node_pointer p = iter->second;
      p->fail_pointer = &root_;  // fail pointer set to root_
      node_queue.push(p);
    }

    // BFS, to calculate fail pointers
    for (; !node_queue.empty(); node_queue.pop()) {
      node_pointer p = node_queue.front();

      // for each child of the current node
      for (child_iterator child = p->children.begin();
           child != p->children.end(); ++child) {
        const character_type& route = child->first;
        node_pointer pchild = child->second;
        node_queue.push(pchild);  // push the child into queue

        // look for the same key to fail pointer's children
        pchild->fail_pointer = find_fail_pointer(p->fail_pointer, route);
      }
    }

    // set updated flag
    fail_pointers_updated_ = true;
  }

  node_pointer find_fail_pointer(node_pointer parent_fail_pointer,
                                 const character_type& route) {
    typedef typename node_pointer_traits<node_pointer>::child_iterator
        child_iterator;

    child_iterator fail_child = parent_fail_pointer->children.find(route);
    if (fail_child != parent_fail_pointer->children.end()) {
      return fail_child->second;
    } else if (parent_fail_pointer == &root_) {
      return &root_;
    } else {
      return find_fail_pointer(parent_fail_pointer->fail_pointer, route);
    }
  }

 private:
  node root_;
  bool fail_pointers_updated_ = false;
  size_type counter_ = 0;
};

// find first subsequence in /subset/ in [first, last)
// return subsequence range in [first, last) if found, [last, last) otherwise
template <typename BidirectionalIterator, typename Character, typename Mapped>
std::pair<BidirectionalIterator, BidirectionalIterator> find_first_of(
    BidirectionalIterator first, BidirectionalIterator last,
    Automation<Character, Mapped>& subset) {
  typedef
      typename Automation<Character, Mapped>::const_node_pointer node_pointer;

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

// check whether any subsequence in /subset/ can be found in [first, last)
template <typename BidirectionalIterator, typename Character, typename Mapped>
bool search(BidirectionalIterator first, BidirectionalIterator last,
            Automation<Character, Mapped>& subset) {
  return find_first_of(first, last, subset).first != last;
}

// how many subsequence in /subset/ can be found in [first, last)
template <typename ForwardIterator, typename Character, typename Mapped>
std::size_t count(ForwardIterator first, ForwardIterator last,
                  Automation<Character, Mapped>& subset) {
  typedef
      typename Automation<Character, Mapped>::const_node_pointer node_pointer;
  std::pair<ForwardIterator, ForwardIterator> fail;
  std::pair<node_pointer, node_pointer> fail_node;
  node_pointer cursor = subset.root_pointer();
  ForwardIterator curr = first;
  std::size_t amount = 0;
  while (subset.match_first(curr, last, cursor, fail, fail_node)) {
    ++amount;
    curr = fail.first;
  }
  return amount;
}

// trie map
// perform std::map interface or like
// =============================================================================
template <typename Character, typename T>
class TrieMap : public Automation<Character, T> {
 public:
  typedef Automation<Character, T> base;
  typedef typename base::character_type character_type;
  typedef typename base::mapped_type mapped_type;
  typedef typename base::node node;
  typedef typename base::node_pointer node_pointer;
  typedef typename base::const_node_pointer const_node_pointer;
  typedef std::size_t size_type;

  template <typename NodePointer>
  class basic_iterator;
  typedef basic_iterator<node_pointer> iterator;
  typedef basic_iterator<const_node_pointer> const_iterator;

  // TBD: to be implemented.
  template <typename NodePointer>
  class basic_iterator {
   public:
    typedef NodePointer node_pointer;
    typedef typename node_pointer_traits<node_pointer>::child_iterator
        child_iterator;
    typedef std::vector<typename TrieMap::character_type> key_type;
    typedef typename TrieMap::mapped_type value_type;
    // template <typename NodePointer2> friend class basic_iterator;
    // // compile error in vs2005
    friend class basic_iterator<typename TrieMap::node_pointer>;
    friend class basic_iterator<typename TrieMap::const_node_pointer>;

   public:
    basic_iterator(node_pointer p, child_iterator pos)
        : parent_(p), position_(pos) {}
    basic_iterator() : parent_(NULL), position_() {}
    basic_iterator(const iterator& other)
        : parent_(other.parent_),
          position_(other.position_),
          path_(other.path_) {}
    value_type& value() { return position_->second->value(); }
    const value_type& value() const { return position_->second->value(); }
    const key_type& key() const {
      if (path_.empty()) {  // calculate once
        node_pointer p = parent_;
        child_iterator pos = position_;
        path_.push_back(pos->first);  // nearest route
        while (p->parent != NULL) {   // back search
          path_.push_back(p->route);  // record route
          pos = p->parent->children.find(p->route);
          p = p->parent;
        }
        std::reverse(path_.begin(),
                     path_.end());  // natural order
      }
      return path_;
    }
    template <typename NodePointer2>
    bool operator==(const basic_iterator<NodePointer2>& other) const {
      return parent_ == other.parent_ && position_ == other.position_;
    }
    template <typename NodePointer2>
    bool operator!=(const basic_iterator<NodePointer2>& other) const {
      return !(*this == other);
    }

   private:
    bool next();      // move to next node  // TBD.
    bool next_eos();  // move to next eos node  // TBD.
   private:
    node_pointer parent_;
    child_iterator position_;
    mutable key_type path_;  // it is const_iterator who push me to
                             // use 'mutable'!
  };

 public:  // iterator observers
  iterator begin() {
    return iterator(base::root_pointer(),
                    base::root_pointer()->children.begin());
  }
  iterator end() {
    return iterator(base::root_pointer(), base::root_pointer()->children.end());
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
  std::pair<iterator, bool> insert(InputIterator first, InputIterator last,
                                   const mapped_type& value) {
    node_pointer pnode;
    bool taken_place = base::insert(first, last, value, pnode);
    return std::make_pair(
        iterator(pnode->parent, pnode->parent->children.find(pnode->route)),
        taken_place);
  }
  template <typename InputIterator>
  std::pair<iterator, bool> insert(
      const std::pair<InputIterator, InputIterator>& range,
      const mapped_type& value) {
    return insert(range.first, range.second, value);
  }
  template <typename Container>
  std::pair<iterator, bool> insert(const Container& sequence,
                                   const mapped_type& value) {
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
  iterator find(InputIterator first, InputIterator last) {
    node_pointer pnode = base::root_pointer();
    if (internal::find(first, last, pnode)) {
      return iterator(pnode->parent,
                      pnode->parent->children.find(pnode->route));
    } else {
      return end();
    }
  }
  template <typename InputIterator>
  iterator find(const std::pair<InputIterator, InputIterator>& range) {
    return find(range.begin(), range.end());
  }
  template <typename Container>
  iterator find(const Container& sequence) {
    return find(sequence.begin(), sequence.end());
  }
  template <typename InputIterator>
  const_iterator find(InputIterator first, InputIterator last) const {
    const_node_pointer pnode = base::root_pointer();
    if (internal::find(first, last, pnode)) {
      return const_iterator(pnode->parent,
                            pnode->parent->children.find(pnode->route));
    } else {
      return end();
    }
  }
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
  mapped_type& operator[](
      const std::pair<InputIterator, InputIterator>& range) {
    node_pointer p = NULL;
    base::insert(range.first, range.second, mapped_type(), p);
    return p->value();
  }
  template <typename Container>
  mapped_type& operator[](const Container& c) {
    return (*this)[std::make_pair(c.begin(), c.end())];
  }

  // observers
  template <typename InputIterator>
  size_type count(InputIterator first, InputIterator last) const {
    return find(first, last) != end() ? 1 : 0;
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
  using base::erase;
  using base::insert;
  using base::match_first;
  using base::mismatch;
};

// subsequence set
// =============================================================================
template <typename Character, typename Mapped = bool>
class SubsequenceSet : public TrieMap<Character, Mapped> {
 public:
  typedef TrieMap<Character, Mapped> base;
  typedef typename base::mapped_type mapped_type;
  typedef typename base::node_pointer node_pointer;
  typedef typename base::iterator iterator;
  typedef typename base::const_iterator const_iterator;

  template <typename BidirectionalIterator>
  struct match_result
      : public std::pair<BidirectionalIterator, BidirectionalIterator> {
   public:
    typedef std::pair<BidirectionalIterator, BidirectionalIterator> base;
    match_result(BidirectionalIterator first, BidirectionalIterator last,
                 node_pointer pnode = NULL)
        : base(first, last), pnode_(pnode) {}
    match_result() : base(), pnode_(NULL) {}
    mapped_type& mapped() { return pnode_->value(); }

   private:
    node_pointer pnode_;
  };

  template <typename BidirectionalIterator, typename Character1,
            typename Mapped1>
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
  typedef typename subsequence_set::template match_result<BidirectionalIterator>
      value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
  typedef std::ptrdiff_t difference_type;
  typedef std::forward_iterator_tag iterator_category;

 public:
  // construct iterator with sequence [first, last) and subsequence set
  // /subset/. /overlap/ denotes whether matched words can overlap with
  // each other or not.
  SubsequenceIterator(sequence_iterator first, sequence_iterator last,
                      subsequence_set& subset, bool overlap = true)
      : begin_(first),
        end_(last),
        psubset_(&subset),
        overlap_(overlap),
        fail_(),
        fail_node_(),
        cursor_(subset.root_pointer()),
        found_(true),
        match_() {
    find_match();
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
             psubset_ == rhs.psubset_ && match_ == rhs.match_;  // field compare
    }
  }
  bool operator!=(const SubsequenceIterator& rhs) const {
    return !(*this == rhs);
  }

  // standard observers of iterator
  // return a range (std::pair) donates matched range in sequence
  reference operator*() { return match_; }
  pointer operator->() { return &match_; }

  // standard increment operator
  SubsequenceIterator& operator++() {
    find_match();
    return *this;
  }
  SubsequenceIterator operator++(int) {
    SubsequenceIterator iter(*this);
    ++(*this);
    return iter;
  }

 private:
  // find next match result
  // pre-condition: !end_of_sequence();
  void find_match() {
    if (end_of_sequence()) {
      return;
    }
    found_ = psubset_->match_first(begin_, end_, cursor_, fail_, fail_node_);
    if (!found_) {
      match_.first = match_.second = end_;
    }

    match_.first = match_.second = fail_.second;
    std::advance(match_.first, -fail_node_.second->height);
    match_ = value_type(match_.first, match_.second, fail_node_.second);
    if (overlap_) {
      begin_ = fail_.first;
    } else {  // overlap match not allowed, restart from match_.second;
      begin_ = fail_.second;
      cursor_ = psubset_->root_pointer();
    }
  }

  // check whether this is end-of-sequence iterator
  bool end_of_sequence() const { return NULL == psubset_ || !found_; }

 private:
  sequence_iterator begin_;
  sequence_iterator end_;
  subsequence_set* psubset_ = nullptr;
  bool overlap_ = true;

  std::pair<sequence_iterator, sequence_iterator> fail_;
  std::pair<node_pointer, node_pointer> fail_node_;
  node_pointer cursor_ = nullptr;
  bool found_ = false;

  value_type match_;
};  // class SubsequenceIterator

}  // namespace ac

#endif  // AC_H_
