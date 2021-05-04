// crosslist.h
#ifndef CROSSLIST_H_
#define CROSSLIST_H_

#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

// crosslist related utilities
namespace crosslist {

typedef std::size_t size_type;

// node that contains cross list values
template <typename T>
struct Node {
  typedef T value_type;
  value_type value;
  size_type row, column;
  Node *left, *right, *up, *down;

  Node(value_type v = value_type(),
       size_type r = std::numeric_limits<size_type>::max(),
       size_type c = std::numeric_limits<size_type>::max())
      : value(v),
        row(r),
        column(c),
        left(NULL),
        right(NULL),
        up(NULL),
        down(NULL) {}
};

// node pointer traits
template <typename NodePtrT>
struct node_pointer_traits;

// specialization for NodeT*
template <typename NodeT>
struct node_pointer_traits<NodeT*> {
  typedef typename NodeT::value_type value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
};

// specialization for const NodeT*
template <typename NodeT>
struct node_pointer_traits<const NodeT*> {
  typedef typename NodeT::value_type value_type;
  typedef const value_type& reference;
  typedef const value_type* pointer;
};

}  // namespace crosslist

// template class CrossList<T>
template <typename T>
class CrossList {
 public:  // interface basic types
  typedef T value_type;
  typedef crosslist::size_type size_type;
  typedef std::ptrdiff_t difference_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;

 protected:  // internal basic types
  typedef crosslist::Node<value_type> node;
  typedef std::vector<node*> headers_type;
  typedef std::vector<size_type> records_type;
  typedef typename headers_type::iterator header_iterator;

 protected:  // internal iterator types
  // basic iterator template
  template <typename NodePtrT>
  class basic_iterator {
   public:  // basic types
    typedef typename CrossList::size_type size_type;
    typedef NodePtrT node_pointer;
    friend class CrossList;

   public:  // iterator traits
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename CrossList::value_type value_type;
    typedef typename CrossList::difference_type difference_type;
    typedef
        typename crosslist::node_pointer_traits<node_pointer>::pointer pointer;
    typedef typename crosslist::node_pointer_traits<node_pointer>::reference
        reference;

   public:  // interface operations
    explicit basic_iterator(node_pointer p = NULL) : p_(p) {}
    operator basic_iterator<const node*>() const {
      return basic_iterator<const node*>(p_);
    }
    reference operator*() { return p_->value; }
    pointer operator->() const { return &(p_->value); }
    bool operator==(const basic_iterator& r) const { return p_ == r.p_; }
    bool operator!=(const basic_iterator& r) const { return !(*this == r); }
    void step_up() { p_ = p_->up; }
    void step_down() { p_ = p_->down; }
    void step_left() { p_ = p_->left; }
    void step_right() { p_ = p_->right; }
    size_type row() const { return p_->row; }
    size_type column() const { return p_->column; }

    template <typename NodePtrT2>  // compare position in matrix logic
    bool pos_less(const basic_iterator<NodePtrT2>& r) const {
      return row() == r.row() ? column() < r.column() : row() < r.row();
    }

   protected:
    node_pointer p_;  // current node
  };

  // get node of iterator
  template <typename NodePtrT>
  static typename basic_iterator<NodePtrT>::node_pointer& node_of_iterator(
      basic_iterator<NodePtrT>& iter) {
    return iter.p_;
  }

  // basic iterator template to iterate a row
  template <typename NodePtrT>
  class basic_row_iterator : public basic_iterator<NodePtrT> {
   protected:
    typedef basic_iterator<NodePtrT> base;

   public:
    typedef typename base::node_pointer node_pointer;

   public:  // defines ++ and -- operations of iterator
    explicit basic_row_iterator(node_pointer p = NULL) : base(p) {}
    operator basic_row_iterator<const node*>() const {
      return basic_row_iterator<const node*>(base::p_);
    }
    basic_row_iterator& operator++() {
      base::step_right();
      return *this;
    }
    basic_row_iterator& operator--() {
      base::step_left();
      return *this;
    }
    basic_row_iterator operator++(int) {
      basic_row_iterator t(*this);
      ++(*this);
      return t;
    }
    basic_row_iterator operator--(int) {
      basic_row_iterator t(*this);
      --(*this);
      return t;
    }
  };

  // basic iterator template to iterate a column
  template <typename NodePtrT>
  class basic_column_iterator : public basic_iterator<NodePtrT> {
   protected:
    typedef basic_iterator<NodePtrT> base;

   public:
    typedef typename base::node_pointer node_pointer;

   public:  // defines ++ and -- operations of iterator
    explicit basic_column_iterator(node_pointer p = NULL) : base(p) {}
    operator basic_column_iterator<const node*>() const {
      return basic_column_iterator<const node*>(base::p_);
    }
    basic_column_iterator& operator++() {
      base::step_down();
      return *this;
    }
    basic_column_iterator& operator--() {
      base::step_up();
      return *this;
    }
    basic_column_iterator operator++(int) {
      basic_column_iterator t(*this);
      ++(*this);
      return t;
    }
    basic_column_iterator operator--(int) {
      basic_column_iterator t(*this);
      --(*this);
      return t;
    }
  };

  // basic iterator template to iterate entire cross list
  friend class basic_cursor_iterator;
  template <typename NodePtrT>
  class basic_cursor_iterator : public basic_iterator<NodePtrT> {
   protected:
    typedef basic_iterator<NodePtrT> base;
    using base::p_;

   public:
    typedef typename base::node_pointer node_pointer;

   public:  // defines ++ and -- operations of iterator
    explicit basic_cursor_iterator(node_pointer p, size_type r,
                                   const CrossList* c)
        : base(p), row_(r), c_(c) {
      forward_adjust();
    }
    basic_cursor_iterator() : base(NULL), row_(-1), c_(NULL) {}
    operator basic_cursor_iterator<const node*>() const {
      return basic_cursor_iterator<const node*>(p_, row_, c_);
    }
    basic_cursor_iterator& operator++() {
      base::step_right();
      forward_adjust();
      return *this;
    }
    basic_cursor_iterator& operator--() {
      base::step_left();
      backword_adjust();
      return *this;
    }
    basic_cursor_iterator operator++(int) {
      basic_cursor_iterator t(*this);
      ++(*this);
      return t;
    }
    basic_cursor_iterator operator--(int) {
      basic_cursor_iterator t(*this);
      --(*this);
      return t;
    }

   protected:
    void forward_adjust() {
      // when p_ is current row_'s tailer and p_ not reach last row_
      while (row_ < c_->last_internal_row() && p_ == c_->headers_[row_]) {
        // p_ move to next row_'s first node
        p_ = c_->headers_[++row_]->right;
      }
    }
    void backword_adjust() {
      // when p_ is current row_'s header and p_ not reach first row_
      while (row_ > 0 && p_ == c_->headers_[row_]) {
        // p_ move to previous row_'s last node
        p_ = c_->headers_[--row_]->left;
      }
    }

   protected:
    size_type row_;
    const CrossList* c_;
  };

  // basic iterator template to reverse a iterator
  template <typename Iterator>
  class basic_reverse_iterator : public Iterator {
   protected:
    typedef Iterator base;

   public:  // defines ++ and -- operations of iterator
    explicit basic_reverse_iterator(const base& iter) : base(iter) {
      --dynamic_cast<base&>(*this);
    }
    template <typename Iterator2>
    operator basic_reverse_iterator<Iterator2>() const {
      basic_reverse_iterator iter(*this);  // temporary iterator
      ++dynamic_cast<base&>(iter);         // restore base iterator
      return basic_reverse_iterator<Iterator2>(dynamic_cast<base&>(iter));
    }
    basic_reverse_iterator& operator++() {
      --dynamic_cast<base&>(*this);
      return *this;
    }
    basic_reverse_iterator& operator--() {
      ++dynamic_cast<base&>(*this);
      return *this;
    }
    basic_reverse_iterator operator++(int) {
      basic_reverse_iterator t(*this);
      ++(*this);
      return t;
    }
    basic_reverse_iterator operator--(int) {
      basic_reverse_iterator t(*this);
      --(*this);
      return t;
    }
  };

 public:  // interface iterator types
  typedef basic_cursor_iterator<node*> iterator;
  typedef basic_cursor_iterator<const node*> const_iterator;
  typedef basic_row_iterator<node*> row_iterator;
  typedef basic_row_iterator<const node*> const_row_iterator;
  typedef basic_column_iterator<node*> column_iterator;
  typedef basic_column_iterator<const node*> const_column_iterator;

  typedef basic_reverse_iterator<iterator> reverse_iterator;
  typedef basic_reverse_iterator<const_iterator> const_reverse_iterator;
  typedef basic_reverse_iterator<row_iterator> reverse_row_iterator;
  typedef basic_reverse_iterator<const_row_iterator> const_reverse_row_iterator;
  typedef basic_reverse_iterator<column_iterator> reverse_column_iterator;
  typedef basic_reverse_iterator<const_column_iterator>
      const_reverse_column_iterator;

 public:  // iterator observers
  iterator begin() { return iterator(headers_[0]->right, 0, this); }
  const_iterator begin() const {
    return const_iterator(headers_[0]->right, 0, this);
  }
  iterator end() {
    size_type last_row = last_internal_row();
    return iterator(headers_[last_row], last_row, this);
  }
  const_iterator end() const {
    size_type last_row = last_internal_row();
    return const_iterator(headers_[last_row], last_row, this);
  }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  row_iterator row_begin(size_type row_index) {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range(
          "CrossList<T>::row_begin(size_type): row_index illegal");
    }
    return row_iterator(headers_[row_index]->right);
  }
  const_row_iterator row_begin(size_type row_index) const {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range(
          "CrossList<T>::row_begin(size_type) const: row_index illegal");
    }
    return const_row_iterator(headers_[row_index]->right);
  }
  row_iterator row_end(size_type row_index) {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range(
          "CrossList<T>::row_end(size_type): row_index illegal");
    }
    return row_iterator(headers_[row_index]);
  }
  const_row_iterator row_end(size_type row_index) const {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range(
          "CrossList<T>::row_end(size_type) const: row_index illegal");
    }
    return const_row_iterator(headers_[row_index]);
  }
  reverse_row_iterator row_rbegin(size_type row_index) {
    return reverse_row_iterator(row_end(row_index));
  }
  const_reverse_row_iterator row_rbegin(size_type row_index) const {
    return const_reverse_row_iterator(row_end(row_index));
  }
  reverse_row_iterator row_rend(size_type row_index) {
    return reverse_row_iterator(row_begin(row_index));
  }
  const_reverse_row_iterator row_rend(size_type row_index) const {
    return const_reverse_row_iterator(row_begin(row_index));
  }

  column_iterator column_begin(size_type col_index) {
    if (!is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::column_begin(size_type): col_index illegal");
    }
    return column_iterator(headers_[col_index]->down);
  }
  const_column_iterator column_begin(size_type col_index) const {
    if (!is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::column_begin(size_type) const: col_index illegal");
    }
    return const_column_iterator(headers_[col_index]->down);
  }
  column_iterator column_end(size_type col_index) {
    if (!is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::column_end(size_type): col_index illegal");
    }
    return column_iterator(headers_[col_index]);
  }
  const_column_iterator column_end(size_type col_index) const {
    if (!is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::column_end(size_type) const: col_index illegal");
    }
    return const_column_iterator(headers_[col_index]);
  }
  reverse_column_iterator column_rbegin(size_type col_index) {
    return reverse_column_iterator(column_end(col_index));
  }
  const_reverse_column_iterator column_rbegin(size_type col_index) const {
    return const_reverse_column_iterator(column_end(col_index));
  }
  reverse_column_iterator column_rend(size_type col_index) {
    return reverse_column_iterator(column_begin(col_index));
  }
  const_reverse_column_iterator column_rend(size_type col_index) const {
    return const_reverse_column_iterator(column_begin(col_index));
  }

 public:  // operation interface
  explicit CrossList(size_type row_count = 0, size_type column_count = 0,
                     const value_type& default_value = value_type())
      : headers_(std::max(row_count, column_count)),
        row_sizes_(row_count),
        column_sizes_(column_count),
        default_value_(default_value) {
    if (headers_.empty()) {
      headers_.resize(1);
    }
    fill_headers(headers_.begin(), headers_.end());
  }
  CrossList(const CrossList& other)
      : headers_(1), default_value_(other.default_value_) {
    fill_headers(headers_.begin(), headers_.end());
    *this = other;
  }
  virtual ~CrossList() {
    clear();
    empty_headers(headers_.begin(), headers_.end());
  }
  CrossList& operator=(const CrossList& rhs) {
    if (&rhs == this) {
      return *this;
    }

    // reset members
    clear();
    default_value_ = rhs.default_value_;
    if (headers_.size() < rhs.headers_.size()) {
      size_type old_size = headers_.size();
      headers_.resize(rhs.headers_.size(), NULL);
      fill_headers(headers_.begin() + old_size, headers_.end());
    } else {
      size_type new_size = rhs.headers_.size();
      empty_headers(headers_.begin() + new_size, headers_.end());
      headers_.resize(new_size);
      reset_headers(headers_.begin(), headers_.end());
    }
    row_sizes_.assign(rhs.row_sizes_.size(), 0);
    column_sizes_.assign(rhs.column_sizes_.size(), 0);
    entire_size_ = 0;

    // insert nodes
    typename CrossList::const_iterator iter = rhs.begin();
    typename CrossList::const_iterator end = rhs.end();
    for (; iter != end; ++iter) {
      size_type row = iter.row(), column = iter.column();
      node* p = new_node(*iter, row, column);
      std::pair<node*, node*> ptrs =
          std::make_pair(headers_[row], headers_[column]);
      insert_node_before(p, ptrs);
    }
    return *this;
  }
  bool operator==(const CrossList& rhs) const {
    if (row_count() != rhs.row_count() ||
        column_count() != rhs.column_count()) {
      return false;
    }
    if (size() != rhs.size()) {  // compare node amount
      return false;
    }
    const_iterator l_iter = begin(), l_end = end();
    const_iterator r_iter = rhs.begin(), r_end = rhs.end();
    for (; l_iter != l_end && r_iter != r_end; ++l_iter, ++r_iter) {
      if (l_iter.row() != r_iter.row() || l_iter.column() != r_iter.column() ||
          *l_iter != *r_iter) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(const CrossList& rhs) const { return !(*this == rhs); }
  void transpose() {                       // transpose as matrix
    std::swap(row_sizes_, column_sizes_);  // swap records

    // swap fields inside node
    for (iterator node_iter = begin(), node_end = end();
         node_iter != node_end;) {
      node* p = node_of_iterator(node_iter);
      ++node_iter;  // must increase before p's fields changes
      std::swap(p->row, p->column);
      std::swap(p->left, p->up);
      std::swap(p->right, p->down);
    }

    // swap header fields
    header_iterator header_iter = headers_.begin();
    for (; header_iter != headers_.end(); ++header_iter) {
      node* p = *header_iter;
      std::swap(p->left, p->up);
      std::swap(p->right, p->down);
    }
  }

  // insert node at coordinate (row_index,col_index)
  // true if insert successfully, false if node already exist
  // (look for node from left to right, from up to down) !!!faster for head
  // insertion!!!
  bool insert(size_type row_index, size_type col_index, const_reference value) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::insert(size_type,size_type,const_reference): "
          "row_index or col_index illegal");
    }

    // find place to insert node
    std::pair<node*, node*> ptrs = locate(row_index, col_index);
    if (has_node_at(ptrs, row_index, col_index)) {
      return false;
    }

    // insert a new allocated node
    node* p = new_node(value, row_index, col_index);
    insert_node_before(p, ptrs);
    return true;
  }

  // insert node at coordinate (row_index,col_index)
  // true if insert successfully, false if node already exist
  // (look for node from right to left, from down to up) !!!faster for tail
  // insertion!!!
  bool rinsert(size_type row_index, size_type col_index,
               const_reference value) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::rinsert(size_type,size_type,const_reference): "
          "row_index or col_index illegal");
    }

    // find place to insert node
    std::pair<node*, node*> ptrs = rlocate(row_index, col_index);
    if (has_node_at(ptrs, row_index, col_index)) return false;

    // insert a new allocated node
    node* p = new_node(value, row_index, col_index);
    insert_node_after(p, ptrs);
    return true;
  }

  // erase node of coordinate (row_index,col_index)
  // true if erase successfully, false if node not exist
  // (look for node from left to right, from up to down) !!!faster for head
  // erase!!!
  bool erase(size_type row_index, size_type col_index) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::erase(size_type,size_type): row_index or col_index "
          "illegal");
    }

    std::pair<node*, node*> ptrs = locate(row_index, col_index);
    if (!has_node_at(ptrs, row_index, col_index)) return false;

    erase_node(ptrs.first);
    return true;
  }

  // erase node of coordinate (row_index,col_index)
  // true if erase successfully, false if node not exist
  // (look for node from right to left, from down to up) !!!faster for tail
  // erase!!!
  bool rerase(size_type row_index, size_type col_index) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::rerase(size_type,size_type): row_index or col_index "
          "illegal");
    }

    std::pair<node*, node*> ptrs = rlocate(row_index, col_index);
    if (!has_node_at(ptrs, row_index, col_index)) return false;

    erase_node(ptrs.first);
    return true;
  }

  // Iterator can be iterator, row_iterator or column_iterator
  // Iterator cannot be reverse_iterator, reverse_row_iterator or
  // reverse_column_iterator
  template <typename Iterator>
  void erase(Iterator iter) {
    node* p = node_of_iterator(iter);
    if (NULL == p || !is_valid_row(p->row) || !is_valid_column(p->column)) {
      throw std::invalid_argument(
          "CrossList<T>::erase(Iterator): illegal iterator");
    }
    erase_node(p);
  }

  // note: name erase_range to avoid confict with erase(size_type, size_type)
  // Iterator can be iterator, row_iterator or column_iterator
  // Iterator cannot be reverse_iterator, reverse_row_iterator or
  // reverse_column_iterator
  template <typename Iterator>
  void erase_range(Iterator first, Iterator last) {
    while (first != last) {
      node* p = node_of_iterator(first);
      ++first;
      if (NULL == p || !is_valid_row(p->row) || !is_valid_column(p->column)) {
        throw std::invalid_argument(
            "CrossList<T>::erase(Iterator,Iterator): range contains illegal "
            "iterator");
      }
      erase_node(p);
    }
  }

  // get value reference of coordinate (row_index,col_index), if not exist,
  // create one (look for node from left to right, from up to down) !!!faster
  // for head search!!!
  reference at(size_type row_index, size_type col_index) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::at(size_type,size_type): row_index or col_index "
          "illegal");
    }

    std::pair<node*, node*> ptrs = locate(row_index, col_index);
    if (has_node_at(ptrs, row_index, col_index)) return ptrs.first->value;

    // create one, and return it
    node* p = new_node(default_value_, row_index, col_index);
    insert_node_before(p, ptrs);
    return p->value;
  }

  // get value reference of coordinate (row_index,col_index), if not exist,
  // throw an exception (look for node from left to right, from up to down)
  // !!!faster for head search!!!
  const_reference at(size_type row_index, size_type col_index) const {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::at(size_type,size_type) const: row_index or col_index "
          "illegal");
    }

    std::pair<node*, node*> ptrs = locate(row_index, col_index);
    if (!has_node_at(ptrs, row_index, col_index)) {
      throw std::runtime_error(
          "CrossList<T>::at(size_type,size_type) const: no value there");
    }
    return ptrs.first->value;
  }

  // get value reference of coordinate (row_index,col_index), if not exist,
  // create one (look for node from right to left, from down to up) !!!faster
  // for head search!!!
  reference rat(size_type row_index, size_type col_index) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::rat(size_type,size_type): row_index or col_index "
          "illegal");
    }

    std::pair<node*, node*> ptrs = rlocate(row_index, col_index);
    if (has_node_at(ptrs, row_index, col_index)) return ptrs.first->value;

    // create one, and return it
    node* p = new_node(default_value_, row_index, col_index);
    insert_node_after(p, ptrs);
    return p->value;
  }

  // get value reference of coordinate (row_index,col_index), if not exist,
  // throw an exception (look for node from right to left, from down to up)
  // !!!faster for head search!!!
  const_reference rat(size_type row_index, size_type col_index) const {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::rat(size_type,size_type) const: row_index or "
          "col_index illegal");
    }

    std::pair<node*, node*> ptrs = rlocate(row_index, col_index);
    if (!has_node_at(ptrs, row_index, col_index)) {
      throw std::runtime_error(
          "CrossList<T>::rat(size_type,size_type) const: no value there");
    }
    return ptrs.first->value;
  }

  // get value reference of coordinate (row_index,col_index), if not exist,
  // create one (look for node from left to right, from up to down) !!!faster
  // for head search!!! virtual interface for optimization of derived class
  virtual reference operator()(size_type row_index, size_type col_index) {
    return at(row_index, col_index);
  }

  // get value reference of coordinate (row_index,col_index), if not exist,
  // throw an exception (look for node from left to right, from up to down)
  // !!!faster for head search!!! virtual interface for optimization of derived
  // class
  virtual const_reference operator()(size_type row_index,
                                     size_type col_index) const {
    return at(row_index, col_index);
  }

  // get value of coordinate (row_index,col_index), if not exist, return default
  // value (look for node from left to right, from up to down) !!!faster for
  // head search!!!
  value_type get(size_type row_index, size_type col_index) const {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::get(size_type,size_type) const: row_index or "
          "col_index illegal");
    }

    std::pair<node*, node*> ptrs = locate(row_index, col_index);
    if (has_node_at(ptrs, row_index, col_index)) return ptrs.first->value;
    return default_value_;
  }

  // get value of coordinate (row_index,col_index), if not exist, return default
  // value (look for node from right to left, from down to up) !!!faster for
  // tail search!!!
  value_type rget(size_type row_index, size_type col_index) const {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::rget(size_type,size_type) const: row_index or "
          "col_index illegal");
    }

    std::pair<node*, node*> ptrs = rlocate(row_index, col_index);
    if (has_node_at(ptrs, row_index, col_index)) return ptrs.first->value;
    return default_value_;
  }

  // set value of coordinate (row_index,col_index) to value, if not exist,
  // create one (look for node from left to right, from up to down) !!!faster
  // for head insertion!!!
  void set(size_type row_index, size_type col_index, const_reference value) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::set(size_type,size_type,const_reference): row_index "
          "or col_index illegal");
    }

    // find node or place to insert node
    std::pair<node*, node*> ptrs = locate(row_index, col_index);

    // operation
    if (has_node_at(ptrs, row_index, col_index)) {
      ptrs.first->value = value;
    } else {  // insert a new allocated node
      node* p = new_node(value, row_index, col_index);
      insert_node_before(p, ptrs);
    }
  }

  // set value of coordinate (row_index,col_index) to value, if not exist,
  // create one (look for node from right to left, from down to up) !!!faster
  // for tail insertion!!!
  void rset(size_type row_index, size_type col_index, const_reference value) {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::rset(size_type,size_type,const_reference): row_index "
          "or col_index illegal");
    }

    // find node or place to insert node
    std::pair<node*, node*> ptrs = rlocate(row_index, col_index);

    // operation
    if (has_node_at(ptrs, row_index, col_index)) {
      ptrs.first->value = value;
    } else {  // insert a new allocated node
      node* p = new_node(value, row_index, col_index);
      insert_node_after(p, ptrs);
    }
  }

  // whether a node of coordinate (row_index,col_index) exist
  // (look for node from left to right, from up to down) !!!faster for head
  // search!!!
  bool exist(size_type row_index, size_type col_index) const {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::exist(size_type,size_type) const: row_index or "
          "col_index illegal");
    }
    std::pair<node*, node*> ptrs = locate(row_index, col_index);
    return has_node_at(ptrs, row_index, col_index);
  }

  // whether a node of coordinate (row_index,col_index) exist
  // (look for node from right to left, from down to up) !!!faster for tail
  // search!!!
  bool rexist(size_type row_index, size_type col_index) const {
    if (!is_valid_row(row_index) || !is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::rexist(size_type,size_type) const: row_index or "
          "col_index illegal");
    }
    std::pair<node*, node*> ptrs = rlocate(row_index, col_index);
    return has_node_at(ptrs, row_index, col_index);
  }

  bool empty() const { return size() == 0; }
  void clear() { erase_range(begin(), end()); }  // empty the cross list

  // reset maximum nodes that cross list can contains
  void reserve(size_type row_count, size_type column_count) {
    row_reserve(row_count);
    column_reserve(column_count);
  }

  // reset maximum rows that cross list can contains
  void row_reserve(size_type new_count) {
    size_type old_count = row_count();
    if (old_count < new_count) {
      // resize containers
      row_sizes_.resize(new_count);
      if (headers_.size() < new_count) {
        size_type headers_old_count = headers_.size();
        headers_.resize(new_count, NULL);
        fill_headers(headers_.begin() + headers_old_count, headers_.end());
      }
    } else if (old_count > new_count) {
      // erase nodes of rows [new_count,old_count)
      for (size_type r = new_count; r < old_count; ++r)
        erase_range(row_begin(r), row_end(r));

      // resize containers
      row_sizes_.resize(new_count);
      size_type max_dim = std::max(row_count(), column_count());
      max_dim = max_dim > 0 ? max_dim : 1;
      empty_headers(headers_.begin() + max_dim, headers_.end());
      headers_.resize(max_dim);
    }
  }

  // reset maximum columns that cross list can contains
  void column_reserve(size_type new_count) {
    size_type old_count = column_count();
    if (old_count < new_count) {
      // resize containers
      column_sizes_.resize(new_count, 0);
      if (headers_.size() < new_count) {
        size_type headers_old_count = headers_.size();
        headers_.resize(new_count, NULL);
        fill_headers(headers_.begin() + headers_old_count, headers_.end());
      }
    } else if (old_count > new_count) {
      // erase nodes of columns [new_count,old_count)
      for (size_type c = new_count; c < old_count; ++c)
        erase_range(column_begin(c), column_end(c));

      // resize containers
      column_sizes_.resize(new_count);
      size_type max_dim = std::max(row_count(), column_count());
      max_dim = max_dim > 0 ? max_dim : 1;
      empty_headers(headers_.begin() + max_dim, headers_.end());
      headers_.resize(max_dim);
    }
  }

  // how many nodes in cross list
  size_type size() const { return entire_size_; }

  // how many rows in cross list
  size_type row_count() const { return row_sizes_.size(); }

  // how many columns in cross list
  size_type column_count() const { return column_sizes_.size(); }

  // how many nodes in row
  size_type row_size(size_type row_index) const {
    if (!is_valid_row(row_index)) {
      throw std::out_of_range(
          "CrossList<T>::row_size(size_type) const: row_index illegal");
    }
    return row_sizes_[row_index];
  }

  // how many nodes in column
  size_type column_size(size_type col_index) const {
    if (!is_valid_column(col_index)) {
      throw std::out_of_range(
          "CrossList<T>::column_size(size_type) const: col_index illegal");
    }
    return column_sizes_[col_index];
  }

 protected:  // internal operations (level -1)
  // fill null pointer range [first,last) with new allocated default header
  void fill_headers(header_iterator first, header_iterator last) {
    for (; first != last; ++first) *first = new_header();
  }

  // set non-null pointer range [firs,last) to null pointers, delete original
  // nodes
  void empty_headers(header_iterator first, header_iterator last) {
    for (; first != last; ++first) {
      delete_node(*first);
      *first = NULL;
    }
  }

  // set fields of nodes in range [first,last) to header's default state
  void reset_headers(header_iterator first, header_iterator last) {
    for (; first != last; ++first) reset_header(*first);
  }

  // check if a row index is in legal range
  bool is_valid_row(size_type row_index) const {
    return row_index < row_count();
  }

  // check if a column index is in legal range
  bool is_valid_column(size_type col_index) const {
    return col_index < column_count();
  }

  // get last internal row index of headers_
  // precondition: assume !headers_.empty()==true
  size_type last_internal_row() const {
    return row_count() > 0 ? row_count() - 1 : 0;
  }

  // look up node (row_index,col_index), if not exist, return right and down
  // side nodes precondition: row_index and col_index are both legal
  std::pair<node*, node*> locate(size_type row_index,
                                 size_type col_index) const {
    std::pair<node*, node*> ptrs =
        std::make_pair(headers_[row_index]->right, headers_[col_index]->down);
    while (ptrs.first != headers_[row_index] &&
           ptrs.first->column < col_index) {
      ptrs.first = ptrs.first->right;
    }
    while (ptrs.second != headers_[col_index] && ptrs.second->row < row_index) {
      ptrs.second = ptrs.second->down;
    }
    return ptrs;
  }

  // look up node (row_index,col_index), if not exist, return left and up side
  // nodes precondition: row_index and col_index are both legal
  std::pair<node*, node*> rlocate(size_type row_index,
                                  size_type col_index) const {
    std::pair<node*, node*> ptrs =
        std::make_pair(headers_[row_index]->left, headers_[col_index]->up);
    while (ptrs.first != headers_[row_index] &&
           ptrs.first->column > col_index) {
      ptrs.first = ptrs.first->left;
    }
    while (ptrs.second != headers_[col_index] && ptrs.second->row > row_index) {
      ptrs.second = ptrs.second->up;
    }
    return ptrs;
  }

  // check if ptrs denote a node at (row_index,col_index)
  bool has_node_at(const std::pair<node*, node*>& ptrs, size_type row_index,
                   size_type col_index) const {
    return ptrs.first == ptrs.second && ptrs.first->column == col_index &&
           ptrs.second->row == row_index;
  }

  // allocate a node, with pointer fields uninitialized (depend on node's
  // constructor)
  virtual node* new_node(const_reference value, size_type row_index,
                         size_type col_index) {
    return new node(value, row_index, col_index);
  }

  // insert node to left side of ptrs->first and up side of ptrs->second
  virtual void insert_node_before(node* new_node,
                                  const std::pair<node*, node*>& ptrs) {
    attach_node_left_of(new_node, ptrs.first);
    attach_node_up_of(new_node, ptrs.second);
    increase_record(new_node);
  }

  // insert node to right side of ptrs->first and down side of ptrs->second
  virtual void insert_node_after(node* new_node,
                                 const std::pair<node*, node*>& ptrs) {
    attach_node_right_of(new_node, ptrs.first);
    attach_node_down_of(new_node, ptrs.second);
    increase_record(new_node);
  }

  // precondition: node is in cross list and not headers
  virtual void erase_node(node* p) {
    decrease_record(p);
    detach_node(p);
    delete_node(p);
  }

 protected:  // internal operations (level -2)
  // allocate a header node, fill its fields with header's default value
  node* new_header() {
    size_type no_val =
        std::numeric_limits<size_type>::max();  // for has_ptr_at()
    node* ptr = new_node(default_value_, no_val, no_val);
    ptr->left = ptr->right = ptr->up = ptr->down = ptr;
    return ptr;
  }

  // reset header node's fields to header's default value
  void reset_header(node* p) {
    size_type no_val =
        std::numeric_limits<size_type>::max();  // for has_ptr_at()
    p->left = p->right = p->up = p->down = p;
    p->row = p->column = no_val;
    p->value = default_value_;
  }

  // deallocate a node, release memory space
  virtual void delete_node(node* p) { delete p; }

  // attach node 'new_node' on left side of node 'pos'
  void attach_node_left_of(node* new_node, node* pos) {
    new_node->left = pos->left;
    new_node->right = pos;
    new_node->left->right = new_node;
    new_node->right->left = new_node;
  }

  // attach node 'new_node' on right side of node 'pos'
  void attach_node_right_of(node* new_node, node* pos) {
    new_node->right = pos->right;
    new_node->left = pos;
    new_node->left->right = new_node;
    new_node->right->left = new_node;
  }

  // attach node 'new_node' on up side of node 'pos'
  void attach_node_up_of(node* new_node, node* pos) {
    new_node->up = pos->up;
    new_node->down = pos;
    new_node->up->down = new_node;
    new_node->down->up = new_node;
  }

  // attach node 'new_node' on down side of node 'pos'
  void attach_node_down_of(node* new_node, node* pos) {
    new_node->down = pos->down;
    new_node->up = pos;
    new_node->up->down = new_node;
    new_node->down->up = new_node;
  }

  // detach node from its neighbours
  void detach_node(node* p) {
    p->left->right = p->right;
    p->right->left = p->left;
    p->up->down = p->down;
    p->down->up = p->up;
  }

  // increase records of node by 1
  void increase_record(node* p) {
    ++row_sizes_[p->row];
    ++column_sizes_[p->column];
    ++entire_size_;
  }

  // decrease records of node by 1
  void decrease_record(node* p) {
    --row_sizes_[p->row];
    --column_sizes_[p->column];
    --entire_size_;
  }

 protected:                    // data members
  headers_type headers_;       // headers
  records_type row_sizes_;     // record node amount of every row
  records_type column_sizes_;  // record node amount of every column
  size_type entire_size_ = 0;  // record node amount of entire cross list
  value_type default_value_;   // default value of 'no node there'
};

#endif  // CROSSLIST_H_
