// sparsematrix.h
#ifndef SPARSEMATRIX_H_
#define SPARSEMATRIX_H_

#include "../crosslist/crosslist.h"

#include <list>
#include <utility>
#include <vector>

template <typename T>
class SparseMatrix: public CrossList<T> {
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
    explicit SparseMatrix(
            size_type row_count=0, size_type column_count=0,
            const value_type& default_value=value_type());

    SparseMatrix(const SparseMatrix& other);

    SparseMatrix& operator = (const SparseMatrix& rhs);

    bool operator == (const SparseMatrix& rhs) const;

    void sparse(size_type sparse_row_count, size_type sparse_column_count);

    std::pair<size_type,size_type> sparse() const;

    void transpose();

    // erase node of coordinate (row_index,col_index)
    // true if erase successfully, false if node not exist
    // (look for node in index table) !!!faster for random access erase!!!
    bool ierase(size_type row_index, size_type col_index);

    // get value reference of coordinate (row_index,col_index), if not exist, create one
    // (look for node in index table) !!!faster for random access!!!
    // but if node not in matrix, then work as at(size_type,size_type).
    reference iat(size_type row_index,size_type col_index);

    // get value reference of coordinate (row_index,col_index),
    //  if not exist, throw an exception
    // (look for node in index table) !!!faster for random access!!!
    const_reference iat(size_type row_index,size_type col_index) const;

    // get value reference of coordinate (row_index,col_index), if not exist, create one
    virtual reference operator () (size_type row_index,size_type col_index) {
        return iat(row_index,col_index);
    }

    // get value reference of coordinate (row_index,col_index),
    //  if not exist, throw an exception
    virtual const_reference operator () (
            size_type row_index, size_type col_index) const {
        return iat(row_index, col_index);
    }

    // get value of coordinate (row_index,col_index),
    //  if not exist, return default value
    // (look for node in index table) !!!faster for random search!!!
    value_type iget(size_type row_index, size_type col_index) const;

    // set value of coordinate (row_index,col_index) to value, if not exist, create one
    // (look for node in index table) !!!faster for random access!!!
    // but if node not in matrix, then work as set(size_type,size_type,const_reference).
    void iset(size_type row_index, size_type col_index, const_reference value);

    // whether a node of coordinate (row_index,col_index) exist
    // (look for node in index table) !!!faster for random access!!!
    bool iexist(size_type row_index, size_type col_index) const;
    
protected:  // internal operations (level -1)
    // look up node (row_index,col_index),
    //  if not exist, return left and up side nodes
    // precondition: row_index and col_index are both legal
    // return: return a node if found, NULL otherwise
    node* ilocate(size_type row_index, size_type col_index) const;

    // insert node to left side of ptrs->first and up side of ptrs->second
    virtual void insert_node_before(
            node* new_node, const std::pair<node*,node*>& ptrs);

    // insert node to right side of ptrs->first and down side of ptrs->second
    virtual void insert_node_after(
            node* new_node, const std::pair<node*,node*>& ptrs);

    virtual void erase_node(node* p);

protected:  // internal operations (level -2)
    void attach_node_into_table(node* p);
    void detach_node_from_table(node* p);

protected:  // data member(s)
    mutable table_type table_;  // index table
};

#include "sparsematrix-inl.h"

#endif  // SPARSEMATRIX_H_
