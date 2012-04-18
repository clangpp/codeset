// sparsematrix-inl.h
#ifndef SPARSEMATRIX_INL_H_
#define SPARSEMATRIX_INL_H_

#include "sparsematrix.h"

#include <cassert>

template <typename T> 
SparseMatrix<T>::SparseMatrix(
        size_type row_count, size_type column_count,
        const value_type& default_value):
    base(row_count, column_count, default_value) {
	sparse(1, 1);
}

template <typename T>
SparseMatrix<T>::SparseMatrix(const SparseMatrix& other) {
	sparse(1, 1);
	*this = other;
}

template <typename T>
SparseMatrix<T>& SparseMatrix<T>::operator = (const SparseMatrix& rhs) {
	if (&rhs==this) return *this;
	base::clear();
	std::pair<size_type, size_type> sp=rhs.sparse();
	sparse(sp.first,sp.second);  // PA: sparse(row,col) must be called before '=', to make up table for elements to attach
	base::operator = (rhs);
	return *this;
}

template <typename T>
bool SparseMatrix<T>::operator == (const SparseMatrix& rhs) const {
	return base::operator == (rhs);
}

template <typename T>
void SparseMatrix<T>::sparse(
        size_type sparse_row_count, size_type sparse_column_count) {
	if (0==sparse_row_count || 0==sparse_column_count) {
		throw std::invalid_argument(
                "SparseMatrix<T>::sparse(size_type,size_type): zero size input.");
    }

	// same size as before, no need to reform
	if (table_.size()==sparse_row_count &&
            table_.front().size()==sparse_column_count) {
		return;
    }

	// clear all node_pointer recorded
	typedef typename table_type::value_type column_type;
	for (typename table_type::iterator
            row_iter=table_.begin(); row_iter!=table_.end(); ++row_iter) {
		for (typename column_type::iterator
                iter=row_iter->begin(); iter!=row_iter->end(); ++iter) {
			iter->clear();
		}
	}

	// resize table
	table_.resize(sparse_row_count);
	for (typename table_type::iterator
            row_iter=table_.begin(); row_iter!=table_.end(); ++row_iter) {
		row_iter->resize(sparse_column_count);
	}

	// make up table
	for (typename base::iterator
            node_iter= base::begin(), node_end=base::end();
			node_iter!=node_end; ++node_iter) {
		attach_node_into_table(node_of_iterator(node_iter));
	}
}

template <typename T>
std::pair<typename SparseMatrix<T>::size_type,
		typename SparseMatrix<T>::size_type> SparseMatrix<T>::sparse() const {
	assert(!table_.empty());
	return std::make_pair(table_.size(),table_.front().size());
}

template <typename T>
void SparseMatrix<T>::transpose() {
    std::pair<size_type, size_type> sp = sparse();
	base::transpose();
	sparse(sp.second, sp.first);  // reform table
}

template <typename T>
bool SparseMatrix<T>::ierase(size_type row_index, size_type col_index) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index))
		throw std::out_of_range("SparseMatrix<T>::rerase(size_type,size_type): row_index or col_index illegal");
	node* p=ilocate(row_index,col_index);
	if (NULL==p) return false;
	erase_node(p);
	return true;
}

template <typename T>
typename SparseMatrix<T>::reference
    SparseMatrix<T>::iat(size_type row_index, size_type col_index) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index))
		throw std::out_of_range("SparseMatrix<T>::iat(size_type,size_type): row_index or col_index illegal");
		
	node* p=ilocate(row_index,col_index);
	if (p) {  // if there is a node there
		return p->value;  // return it
    } else {  // else
		return at(row_index,col_index);  // return at(row_index,col_index)
    }
}

template <typename T>
typename SparseMatrix<T>::const_reference
    SparseMatrix<T>::iat(size_type row_index, size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "SparseMatrix<T>::iat(size_type,size_type) const: row_index or col_index illegal");
    }
		
	node* p=ilocate(row_index,col_index);
	if (p) {  // if there is a node there
		return p->value;  // return it
    } else {  // else, report error
		throw std::runtime_error(
                "SparseMatrix<T>::iat(size_type,size_type) const: no value there");
    }
}

template <typename T>
typename SparseMatrix<T>::value_type
    SparseMatrix<T>::iget(size_type row_index, size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "SparseMatrix<T>::iget(size_type,size_type) const: row_index or col_index illegal");
    }

	node* p=ilocate(row_index,col_index);
	return p ? p->value : base::default_value_;
}

template <typename T>
void SparseMatrix<T>::iset(
        size_type row_index, size_type col_index, const_reference value) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "SparseMatrix<T>::iset(size_type,size_type,const_reference): row_index or col_index illegal");
    }

	node* p=ilocate(row_index,col_index);
	if (p) {
		p->value = value;
    } else {
		set(row_index,col_index,value);
    }
}

template <typename T>
bool SparseMatrix<T>::iexist(size_type row_index,size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "SparseMatrix<T>::iexist(size_type,size_type) const: row_index or col_index illegal");
    }
	return ilocate(row_index,col_index)!=NULL;
}

// precondition: !table_.empty()
template <typename T> 
typename SparseMatrix<T>::node*
	SparseMatrix<T>::ilocate(size_type row_index, size_type col_index) const {
	size_type row_mod=row_index % table_.size();
	size_type col_mod=col_index % table_.front().size();
	std::list<node*>& nodes=table_[row_mod][col_mod];
	for(typename std::list<node*>::iterator
            iter=nodes.begin(); iter!=nodes.end(); ++iter) {
		if (row_index==(*iter)->row && col_index==(*iter)->column)
			return *iter;
	}
	return NULL;
}

template <typename T>
void SparseMatrix<T>::insert_node_before(
        node* new_node, const std::pair<node*,node*>& ptrs) {
	base::insert_node_before(new_node,ptrs);
	attach_node_into_table(new_node);
}

template <typename T>
void SparseMatrix<T>::insert_node_after(
        node* new_node, const std::pair<node*,node*>& ptrs) {
	base::insert_node_after(new_node,ptrs);
	attach_node_into_table(new_node);
}

template <typename T>
void SparseMatrix<T>::erase_node(node* p) {
	detach_node_from_table(p);
	base::erase_node(p);
}

template <typename T>
void SparseMatrix<T>::attach_node_into_table(node* p) {
	size_type row_mod=p->row % table_.size();
	size_type col_mod=p->column % table_.front().size();
	table_[row_mod][col_mod].push_back(p);
}

template <typename T>
void SparseMatrix<T>::detach_node_from_table(node* p) {
	size_type row_mod=p->row % table_.size();
	size_type col_mod=p->column % table_.front().size();
	table_[row_mod][col_mod].remove(p);
}

#endif  // SPARSEMATRIX_INL_H_
