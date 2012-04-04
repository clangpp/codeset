// crosslist-inl.h
#ifndef CROSSLIST_INL_H_
#define CROSSLIST_INL_H_

#include "crosslist.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

// CrossList iterator observers
// -----------------------------------------------------------------------------

template <typename T>
typename CrossList<T>::iterator CrossList<T>::begin() {
	return iterator(headers_[0]->right,0,this);
}

template <typename T>
typename CrossList<T>::const_iterator CrossList<T>::begin() const {
	return const_iterator(headers_[0]->right,0,this);
}

template <typename T>
typename CrossList<T>::iterator CrossList<T>::end() {
	size_type last_row=last_internal_row();
	return iterator(headers_[last_row],last_row,this);
}

template <typename T>
typename CrossList<T>::const_iterator CrossList<T>::end() const {
	size_type last_row=last_internal_row();
	return const_iterator(headers_[last_row],last_row,this);
}

template <typename T>
typename CrossList<T>::row_iterator
    CrossList<T>::row_begin(size_type row_index) {
	if (!is_legal_row(row_index)) {
		throw std::out_of_range(
                "CrossList<T>::row_begin(size_type): row_index illegal");
    }
	return row_iterator(headers_[row_index]->right);
}

template <typename T>
typename CrossList<T>::const_row_iterator
    CrossList<T>::row_begin(size_type row_index) const {
	if (!is_legal_row(row_index)) {
		throw std::out_of_range(
                "CrossList<T>::row_begin(size_type) const: row_index illegal");
    }
	return const_row_iterator(headers_[row_index]->right);
}

template <typename T>
typename CrossList<T>::row_iterator
    CrossList<T>::row_end(size_type row_index) {
	if (!is_legal_row(row_index)) {
		throw std::out_of_range(
                "CrossList<T>::row_end(size_type): row_index illegal");
    }
	return row_iterator(headers_[row_index]);
}

template <typename T>
typename CrossList<T>::const_row_iterator
    CrossList<T>::row_end(size_type row_index) const {
	if (!is_legal_row(row_index)) {
		throw std::out_of_range(
                "CrossList<T>::row_end(size_type) const: row_index illegal");
    }
	return const_row_iterator(headers_[row_index]);
}

template <typename T>
typename CrossList<T>::column_iterator
    CrossList<T>::column_begin(size_type col_index) {
	if (!is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::column_begin(size_type): col_index illegal");
    }
	return column_iterator(headers_[col_index]->down);
}

template <typename T>
typename CrossList<T>::const_column_iterator
    CrossList<T>::column_begin(size_type col_index) const {
	if (!is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::column_begin(size_type) const: col_index illegal");
    }
	return const_column_iterator(headers_[col_index]->down);
}

template <typename T>
typename CrossList<T>::column_iterator
    CrossList<T>::column_end(size_type col_index) {
	if (!is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::column_end(size_type): col_index illegal");
    }
	return column_iterator(headers_[col_index]);
}

template <typename T>
typename CrossList<T>::const_column_iterator
    CrossList<T>::column_end(size_type col_index) const {
	if (!is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::column_end(size_type) const: col_index illegal");
    }
	return const_column_iterator(headers_[col_index]);
}

// CrossList member functions
// -----------------------------------------------------------------------------
template <typename T> 
CrossList<T>::CrossList(size_type row_count,
        size_type column_count, const value_type& default_value):
	headers_(std::max(row_count,column_count)), row_sizes_(row_count),
    column_sizes_(column_count), entire_size_(0), default_value_(default_value) {
	if (headers_.empty()) headers_.resize(1);
	fill_headers(headers_.begin(),headers_.end());
}

template <typename T>
CrossList<T>::CrossList(const CrossList& other):
    headers_(1), entire_size_(0), default_value_(value_type()) {
	fill_headers(headers_.begin(),headers_.end());
	*this=other;
}

template <typename T>
CrossList<T>::~CrossList() {
	clear();
	empty_headers(headers_.begin(),headers_.end());
}

template <typename T>
CrossList<T>& CrossList<T>::operator = (const CrossList& rhs) {
	if (&rhs==this) return *this;

	// reset members
	clear();
	default_value_ = rhs.default_value_;
	if (headers_.size()<rhs.headers_.size()) {
		size_type old_size=headers_.size();
		headers_.resize(rhs.headers_.size(),NULL);
		fill_headers(headers_.begin()+old_size,headers_.end());
	} else {
		size_type new_size=rhs.headers_.size();
		empty_headers(headers_.begin()+new_size,headers_.end());
		headers_.resize(new_size);
		reset_headers(headers_.begin(),headers_.end());
	}
    row_sizes_.assign(rhs.row_sizes_.size(), 0);
    column_sizes_.assign(rhs.column_sizes_.size(), 0);
	entire_size_ = 0;

	// insert nodes
	typename CrossList::const_iterator iter=rhs.begin();
	typename CrossList::const_iterator end=rhs.end();
	for (; iter!=end; ++iter) {
		size_type row=iter.row(), column=iter.column();
		node* p = new_node(*iter, row, column);
		std::pair<node*,node*> ptrs=
			std::make_pair(headers_[row],headers_[column]);
		insert_node_before(p, ptrs);
	}
	return *this;
}

template <typename T>
bool CrossList<T>::operator == (const CrossList& rhs) const {
	if (row_count()!=rhs.row_count() || column_count()!=rhs.column_count())
		return false;
	if (size()!=rhs.size()) return false;  // compare node amount
	const_iterator l_iter=begin(),l_end=end();
	const_iterator r_iter=rhs.begin(),r_end=rhs.end();
	for (; l_iter!=l_end && r_iter!=r_end; ++l_iter,++r_iter) {
		if (l_iter.row()!=r_iter.row()) return false;
		if (l_iter.column()!=r_iter.column()) return false;
		if (*l_iter!=*r_iter) return false;
	}
	return true;
}

template <typename T>
void CrossList<T>::transpose() {
	std::swap(row_sizes_,column_sizes_);  // swap records

	// swap fields inside node
	for (iterator node_iter=begin(), node_end=end(); node_iter!=node_end;) {
		node* p=node_of_iterator(node_iter);
		++node_iter; // must increase before p's fields changes
		std::swap(p->row,p->column);
		std::swap(p->left,p->up);
		std::swap(p->right,p->down);
	}

	// swap header fields
	header_iterator header_iter=headers_.begin();
	for (; header_iter!=headers_.end(); ++header_iter) {
		node* p=*header_iter;
		std::swap(p->left,p->up);
		std::swap(p->right,p->down);
	}
}

// this function ensure no node be placed wrong, as well is its assumption.
template <typename T>
bool CrossList<T>::insert(
        size_type row_index,size_type col_index,const_reference value) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::insert(size_type,size_type,const_reference): row_index or col_index illegal");
    }
	
	// find place to insert node
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index)) return false;

    // insert a new allocated node
    node* p=new_node(value,row_index,col_index);
    insert_node_before(p,ptrs);
    return true;
}

// this function ensure no node be placed wrong, what as well is its assumption.
template <typename T>
bool CrossList<T>::rinsert(
        size_type row_index,size_type col_index,const_reference value) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::rinsert(size_type,size_type,const_reference): row_index or col_index illegal");
    }
	
	// find place to insert node
	std::pair<node*,node*> ptrs = rlocate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index)) return false;

    // insert a new allocated node
    node* p = new_node(value,row_index,col_index);
    insert_node_after(p,ptrs);
    return true;
}

template <typename T>
bool CrossList<T>::erase(size_type row_index,size_type col_index) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::erase(size_type,size_type): row_index or col_index illegal");
    }
	
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (!has_node_at(ptrs,row_index,col_index)) return false;

    erase_node(ptrs.first);
    return true;
}

template <typename T>
bool CrossList<T>::rerase(size_type row_index,size_type col_index) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::rerase(size_type,size_type): row_index or col_index illegal");
    }
	
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	if (!has_node_at(ptrs,row_index,col_index)) return false;

    erase_node(ptrs.first);
    return true;
}

template <typename T>
template <typename Iterator>
void CrossList<T>::erase(Iterator iter) {
	node* p=node_of_iterator(iter);
	if (NULL==p || !is_legal_row(p->row) || !is_legal_column(p->column)) {
		throw std::invalid_argument(
                "CrossList<T>::erase(Iterator): illegal iterator");
    }
	erase_node(p);
}

template <typename T>
template <typename Iterator>
void CrossList<T>::erase_range(Iterator first, Iterator last) {
	while (first!=last) {
		node* p=node_of_iterator(first);
		++first;
		if (NULL==p || !is_legal_row(p->row) || !is_legal_column(p->column)) {
			throw std::invalid_argument(
                    "CrossList<T>::erase(Iterator,Iterator): range contains illegal iterator");
        }
		erase_node(p);
	}
}

template <typename T>
typename CrossList<T>::reference CrossList<T>::at(
        size_type row_index,size_type col_index) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::at(size_type,size_type): row_index or col_index illegal");
    }
		
	std::pair<node*,node*> ptrs = locate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index)) return ptrs.first->value;

    // create one, and return it
    node* p = new_node(default_value_,row_index,col_index);
    insert_node_before(p,ptrs);
    return p->value;
}

template <typename T>
typename CrossList<T>::const_reference CrossList<T>::at(
        size_type row_index, size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::at(size_type,size_type) const: row_index or col_index illegal");
    }
		
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (!has_node_at(ptrs,row_index,col_index)) {
        throw std::runtime_error(
                "CrossList<T>::at(size_type,size_type) const: no value there");
    }
    return ptrs.first->value;
}

template <typename T>
typename CrossList<T>::reference CrossList<T>::rat(
        size_type row_index,size_type col_index) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::rat(size_type,size_type): row_index or col_index illegal");
    }
		
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index)) return ptrs.first->value;

	// create one, and return it
    node* p = new_node(default_value_,row_index,col_index);
    insert_node_after(p,ptrs);
    return p->value;
}

template <typename T>
typename CrossList<T>::const_reference CrossList<T>::rat(
        size_type row_index, size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::rat(size_type,size_type) const: row_index or col_index illegal");
    }
		
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	if (!has_node_at(ptrs,row_index,col_index)) {
		throw std::runtime_error(
                "CrossList<T>::rat(size_type,size_type) const: no value there");
    }
    return ptrs.first->value;
}

template <typename T>
typename CrossList<T>::value_type CrossList<T>::get(
        size_type row_index, size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::get(size_type,size_type) const: row_index or col_index illegal");
    }
		
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index)) return ptrs.first->value;
    return default_value_;
}

template <typename T>
typename CrossList<T>::value_type CrossList<T>::rget(
        size_type row_index, size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::rget(size_type,size_type) const: row_index or col_index illegal");
    }
		
	std::pair<node*,node*> ptrs = rlocate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index)) return ptrs.first->value;
    return default_value_;
}

template <typename T>
void CrossList<T>::set(
        size_type row_index, size_type col_index, const_reference value) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::set(size_type,size_type,const_reference): row_index or col_index illegal");
    }
	
	// find node or place to insert node
	std::pair<node*,node*> ptrs = locate(row_index,col_index);
	
	// operation
	if (has_node_at(ptrs,row_index,col_index)) {
		ptrs.first->value = value;
    } else {  // insert a new allocated node
		node* p = new_node(value,row_index,col_index);
		insert_node_before(p,ptrs);
	}
}

template <typename T>
void CrossList<T>::rset(
        size_type row_index, size_type col_index, const_reference value) {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::rset(size_type,size_type,const_reference): row_index or col_index illegal");
    }
	
	// find node or place to insert node
	std::pair<node*, node*> ptrs = rlocate(row_index, col_index);
	
	// operation
	if (has_node_at(ptrs, row_index, col_index)) {
		ptrs.first->value = value;
    } else {  // insert a new allocated node
		node* p=new_node(value, row_index, col_index);
		insert_node_after(p, ptrs);
	}
}

template <typename T>
bool CrossList<T>::exist(size_type row_index,size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::exist(size_type,size_type) const: row_index or col_index illegal");
    }
	std::pair<node*, node*> ptrs = locate(row_index, col_index);
	return has_node_at(ptrs, row_index, col_index);
}

template <typename T>
bool CrossList<T>::rexist(size_type row_index,size_type col_index) const {
	if (!is_legal_row(row_index) || !is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::rexist(size_type,size_type) const: row_index or col_index illegal");
    }
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	return has_node_at(ptrs,row_index,col_index);
}

template <typename T>
void CrossList<T>::reserve(size_type row_count, size_type column_count) {
	row_reserve(row_count);
	column_reserve(column_count);
}

template <typename T>
void CrossList<T>::row_reserve(size_type new_count)
{
	size_type old_count=row_count();
	if (old_count<new_count) {
		// resize containers
		row_sizes_.resize(new_count);
		if (headers_.size() < new_count) {
			size_type headers_old_count=headers_.size();
			headers_.resize(new_count,NULL);
			fill_headers(headers_.begin()+headers_old_count,headers_.end());
		}
	} else if (old_count>new_count) {
		// erase nodes of rows [new_count,old_count)
		for (size_type r=new_count; r<old_count; ++r)
			erase_range(row_begin(r),row_end(r));

		// resize containers
		row_sizes_.resize(new_count);
		size_type max_dim=std::max(row_count(),column_count());
		max_dim = max_dim>0 ? max_dim : 1;
		empty_headers(headers_.begin()+max_dim,headers_.end());
		headers_.resize(max_dim);
	}
}

template <typename T>
void CrossList<T>::column_reserve(size_type new_count)
{
	size_type old_count=column_count();
	if (old_count<new_count) {
		// resize containers
		column_sizes_.resize(new_count,0);
		if (headers_.size() < new_count) {
			size_type headers_old_count=headers_.size();
			headers_.resize(new_count,NULL);
			fill_headers(headers_.begin()+headers_old_count,headers_.end());
		}
	} else if (old_count>new_count) {
		// erase nodes of columns [new_count,old_count)
		for (size_type c=new_count; c<old_count; ++c)
			erase_range(column_begin(c),column_end(c));

		// resize containers
		column_sizes_.resize(new_count);
		size_type max_dim=std::max(row_count(),column_count());
		max_dim = max_dim>0 ? max_dim : 1;
		empty_headers(headers_.begin()+max_dim,headers_.end());
		headers_.resize(max_dim);
	}
}

template <typename T>
typename CrossList<T>::size_type
    CrossList<T>::row_size(size_type row_index) const {
	if (!is_legal_row(row_index)) {
		throw std::out_of_range(
                "CrossList<T>::row_size(size_type) const: row_index illegal");
    }
	return row_sizes_[row_index];
}

template <typename T>
typename CrossList<T>::size_type
    CrossList<T>::column_size(size_type col_index) const {
	if (!is_legal_column(col_index)) {
		throw std::out_of_range(
                "CrossList<T>::column_size(size_type) const: col_index illegal");
    }
	return column_sizes_[col_index];
}

// internal operations (level -1)
// -----------------------------------------------------------------------------
template <typename T>
void CrossList<T>::fill_headers(header_iterator first, header_iterator last) {
	for (; first!=last; ++first) *first = new_header();
}

template <typename T>
void CrossList<T>::empty_headers(header_iterator first, header_iterator last) {
	for (; first!=last; ++first) {
		delete_node(*first);
		*first = NULL;
	}
}

template <typename T>
void CrossList<T>::reset_headers(header_iterator first, header_iterator last) {
	for (; first!=last; ++first) reset_header(*first);
}

template <typename T> 
std::pair<typename CrossList<T>::node*, typename CrossList<T>::node*>
	CrossList<T>::locate(size_type row_index, size_type col_index) const {
	std::pair<node*,node*> ptrs=
		std::make_pair(headers_[row_index]->right,headers_[col_index]->down);
	while (ptrs.first!=headers_[row_index] && ptrs.first->column<col_index)
		ptrs.first = ptrs.first->right;
	while (ptrs.second!=headers_[col_index] && ptrs.second->row<row_index)
		ptrs.second = ptrs.second->down;
	return ptrs;
}

template <typename T> 
std::pair<typename CrossList<T>::node*, typename CrossList<T>::node*>
	CrossList<T>::rlocate(size_type row_index, size_type col_index) const {
	std::pair<node*,node*> ptrs=
        std::make_pair(headers_[row_index]->left,headers_[col_index]->up);
	while (ptrs.first!=headers_[row_index] && ptrs.first->column>col_index)
		ptrs.first = ptrs.first->left;
	while (ptrs.second!=headers_[col_index] && ptrs.second->row>row_index)
		ptrs.second = ptrs.second->up;
	return ptrs;
}

template <typename T>
void CrossList<T>::insert_node_before(
        node* new_node, const std::pair<node*,node*>& ptrs) {
	attach_node_left_of(new_node,ptrs.first);
	attach_node_up_of(new_node,ptrs.second);
	increase_record(new_node);
}

template <typename T>
void CrossList<T>::insert_node_after(
        node* new_node, const std::pair<node*,node*>& ptrs) {
	attach_node_right_of(new_node,ptrs.first);
	attach_node_down_of(new_node,ptrs.second);
	increase_record(new_node);
}

template <typename T>
void CrossList<T>::erase_node(node* p) {
	decrease_record(p);
	detach_node(p);
	delete_node(p);
}

// internal operations (level -2)
// -----------------------------------------------------------------------------
template <typename T> 
typename CrossList<T>::node* CrossList<T>::new_header() {
	size_type no_val=std::numeric_limits<size_type>::max(); // for has_ptr_at()
	node* ptr=new_node(default_value_,no_val,no_val);
	ptr->left = ptr->right = ptr->up = ptr->down = ptr;
	return ptr;
}

template <typename T> 
void CrossList<T>::reset_header(node* p) {
	size_type no_val=std::numeric_limits<size_type>::max(); // for has_ptr_at()
	p->left = p->right = p->up = p->down = p;
	p->row = p->column = no_val;
	p->value = default_value_;
}

template <typename T>
void CrossList<T>::attach_node_left_of(node* new_node, node* pos) {
	new_node->left = pos->left;
	new_node->right = pos;
	new_node->left->right = new_node;
	new_node->right->left = new_node;
}

template <typename T>
void CrossList<T>::attach_node_right_of(node* new_node, node* pos) {
	new_node->right = pos->right;
	new_node->left = pos;
	new_node->left->right = new_node;
	new_node->right->left = new_node;
}

template <typename T>
void CrossList<T>::attach_node_up_of(node* new_node, node* pos) {
	new_node->up = pos->up;
	new_node->down = pos;
	new_node->up->down = new_node;
	new_node->down->up = new_node;
}

template <typename T>
void CrossList<T>::attach_node_down_of(node* new_node, node* pos) {
	new_node->down = pos->down;
	new_node->up = pos;
	new_node->up->down = new_node;
	new_node->down->up = new_node;
}

template <typename T>
void CrossList<T>::detach_node(node* p) {
	p->left->right = p->right;
	p->right->left = p->left;
	p->up->down = p->down;
	p->down->up = p->up;
}

template <typename T>
void CrossList<T>::increase_record(node* p) {
	++row_sizes_[p->row];
	++column_sizes_[p->column];
	++entire_size_;
}

template <typename T>
void CrossList<T>::decrease_record(node* p) {
	--row_sizes_[p->row];
	--column_sizes_[p->column];
	--entire_size_;
}

#endif  // CROSSLIST_INL_Hb
