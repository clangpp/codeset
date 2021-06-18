#ifndef SPARSEMATRIX_H_
#define SPARSEMATRIX_H_

#include <cassert>
#include <list>
#include <utility>
#include <vector>

#include "crosslist/crosslist.h"

template <typename T>
class SparseMatrix : public CrossList<T> {
 protected:
  typedef CrossList<T> base;
  typedef typename base::node node;
  typedef std::vector<std::vector<std::list<node*> > > table_type;

 public:  // container typedefs
  typedef typename base::value_type value_type;
  typedef typename base::size_type size_type;
  typedef typename base::difference_type difference_type;
  typedef typename base::pointer pointer;
  typedef typename base::const_pointer const_pointer;
  typedef typename base::reference reference;
  typedef typename base::const_reference const_reference;

 public:
  explicit SparseMatrix(size_type row_count = 0, size_type column_count = 0,
                        const value_type& default_value = value_type())
      : base(row_count, column_count, default_value) {
    sparse(1, 1);
  }

  SparseMatrix(const SparseMatrix& other) {
    sparse(1, 1);
    *this = other;
  }

  SparseMatrix& operator=(const SparseMatrix& rhs) {
    if (&rhs == this) {
      return *this;
    }
    base::clear();
    std::pair<size_type, size_type> sp = rhs.sparse();
    // PA: sparse(row,col) must be called before
    // '=', to make up table for elements to attach
    sparse(sp.first, sp.second);
    base::operator=(rhs);
    return *this;
  }

  bool operator==(const SparseMatrix& rhs) const {
    return base::operator==(rhs);
  }

  void sparse(size_type sparse_row_count, size_type sparse_column_count) {
    if (0 == sparse_row_count || 0 == sparse_column_count) {
      throw std::invalid_argument(
          "SparseMatrix<T>::sparse(size_type,size_type): zero size input.");
    }

    // same size as before, no need to reform
    if (table_.size() == sparse_row_count &&
        table_.front().size() == sparse_column_count) {
      return;
    }

    // clear all node_pointer recorded
    typedef typename table_type::value_type column_type;
    for (typename table_type::iterator row_iter = table_.begin();
         row_iter != table_.end(); ++row_iter) {
      for (typename column_type::iterator iter = row_iter->begin();
           iter != row_iter->end(); ++iter) {
        iter->clear();
      }
    }

    // resize table
    table_.resize(sparse_row_count);
    for (typename table_type::iterator row_iter = table_.begin();
         row_iter != table_.end(); ++row_iter) {
      row_iter->resize(sparse_column_count);
    }

    // make up table
    for (typename base::iterator node_iter = base::begin(),
                                 node_end = base::end();
         node_iter != node_end; ++node_iter) {
      attach_node_into_table(this->node_of_iterator(node_iter));
    }
  }

  std::pair<size_type, size_type> sparse() const {
    assert(!table_.empty());
    return std::make_pair(table_.size(), table_.front().size());
  }

  void transpose() {
    const auto& sp = sparse();
    base::transpose();
    sparse(sp.second, sp.first);  // reform table
  }

  // erase node of coordinate (row_index,col_index)
  // true if erase successfully, false if node not exist
  // (look for node in index table) !!!faster for random access erase!!!
  bool ierase(size_type row_index, size_type col_index) {
    if (!this->is_valid_row(row_index) || !this->is_valid_column(col_index)) {
      throw std::out_of_range(
          "SparseMatrix<T>::rerase(size_type,size_type): row_index or "
          "col_index illegal");
    }
    node* p = ilocate(row_index, col_index);
    if (nullptr == p) {
      return false;
    }
    erase_node(p);
    return true;
  }

  // get value reference of coordinate (row_index,col_index), if not exist,
  // create one (look for node in index table) !!!faster for random access!!!
  // but if node not in matrix, then work as at(size_type,size_type).
  reference iat(size_type row_index, size_type col_index) {
    if (!this->is_valid_row(row_index) || !this->is_valid_column(col_index)) {
      throw std::out_of_range(
          "SparseMatrix<T>::iat(size_type,size_type): row_index or col_index "
          "illegal");
    }

    node* p = ilocate(row_index, col_index);
    return p ? p->value : this->at(row_index, col_index);
  }

  // get value reference of coordinate (row_index,col_index),
  //  if not exist, throw an exception
  // (look for node in index table) !!!faster for random access!!!
  const_reference iat(size_type row_index, size_type col_index) const {
    if (!this->is_valid_row(row_index) || !this->is_valid_column(col_index)) {
      throw std::out_of_range(
          "SparseMatrix<T>::iat(size_type,size_type) const: row_index or "
          "col_index illegal");
    }

    if (node* p = ilocate(row_index, col_index); p != nullptr) {
      return p->value;  // if there is a node there, return it
    }
    throw std::runtime_error(
        "SparseMatrix<T>::iat(size_type,size_type) const: no value there");
  }

  // get value reference of coordinate (row_index,col_index),
  //  if not exist, create one
  virtual reference operator()(size_type row_index, size_type col_index) {
    return iat(row_index, col_index);
  }

  // get value reference of coordinate (row_index,col_index),
  //  if not exist, throw an exception
  virtual const_reference operator()(size_type row_index,
                                     size_type col_index) const {
    return iat(row_index, col_index);
  }

  // get value of coordinate (row_index,col_index),
  //  if not exist, return default value
  // (look for node in index table) !!!faster for random search!!!
  value_type iget(size_type row_index, size_type col_index) const {
    if (!this->is_valid_row(row_index) || !this->is_valid_column(col_index)) {
      throw std::out_of_range(
          "SparseMatrix<T>::iget(size_type,size_type) const: row_index or "
          "col_index illegal");
    }

    node* p = ilocate(row_index, col_index);
    return p ? p->value : base::default_value_;
  }

  // set value of coordinate (row_index,col_index) to value, if not exist,
  // create one (look for node in index table) !!!faster for random access!!!
  // but if node not in matrix, then work as
  // set(size_type,size_type,const_reference).
  void iset(size_type row_index, size_type col_index, const_reference value) {
    if (!this->is_valid_row(row_index) || !this->is_valid_column(col_index)) {
      throw std::out_of_range(
          "SparseMatrix<T>::iset(size_type,size_type,const_reference): "
          "row_index "
          "or col_index illegal");
    }

    if (node* p = ilocate(row_index, col_index); p != nullptr) {
      p->value = value;
      return;
    }
    this->set(row_index, col_index, value);
  }

  // whether a node of coordinate (row_index,col_index) exist
  // (look for node in index table) !!!faster for random access!!!
  bool iexist(size_type row_index, size_type col_index) const {
    if (!this->is_valid_row(row_index) || !this->is_valid_column(col_index)) {
      throw std::out_of_range(
          "SparseMatrix<T>::iexist(size_type,size_type) const: row_index or "
          "col_index illegal");
    }
    return ilocate(row_index, col_index) != nullptr;
  }

 protected:  // internal operations (level -1)
  // look up node (row_index,col_index),
  //  if not exist, return left and up side nodes
  // precondition: !table_.empty()
  // precondition: row_index and col_index are both legal
  // return: return a node if found, nullptr otherwise
  node* ilocate(size_type row_index, size_type col_index) const {
    size_type row_mod = row_index % table_.size();
    size_type col_mod = col_index % table_.front().size();
    std::list<node*>& nodes = table_[row_mod][col_mod];
    for (typename std::list<node*>::iterator iter = nodes.begin();
         iter != nodes.end(); ++iter) {
      if (row_index == (*iter)->row && col_index == (*iter)->column) {
        return *iter;
      }
    }
    return nullptr;
  }

  // insert node to left side of ptrs->first and up side of ptrs->second
  virtual void insert_node_before(node* new_node,
                                  const std::pair<node*, node*>& ptrs) {
    base::insert_node_before(new_node, ptrs);
    attach_node_into_table(new_node);
  }

  // insert node to right side of ptrs->first and down side of ptrs->second
  virtual void insert_node_after(node* new_node,
                                 const std::pair<node*, node*>& ptrs) {
    base::insert_node_after(new_node, ptrs);
    attach_node_into_table(new_node);
  }

  virtual void erase_node(node* p) {
    detach_node_from_table(p);
    base::erase_node(p);
  }

 protected:  // internal operations (level -2)
  void attach_node_into_table(node* p) {
    size_type row_mod = p->row % table_.size();
    size_type col_mod = p->column % table_.front().size();
    table_[row_mod][col_mod].push_back(p);
  }
  void detach_node_from_table(node* p) {
    size_type row_mod = p->row % table_.size();
    size_type col_mod = p->column % table_.front().size();
    table_[row_mod][col_mod].remove(p);
  }

 protected:                   // data member(s)
  mutable table_type table_;  // index table
};

#endif  // SPARSEMATRIX_H_
