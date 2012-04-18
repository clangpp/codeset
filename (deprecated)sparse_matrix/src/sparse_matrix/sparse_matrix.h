//sparse_matrix.h
/**************************************************************************
module: sparse_matrix
file(s): cross_list.h, sparse_matrix.h
function: class template sparse_matrix, implements an STL style sparse matrix,
	which can be used to store sparse matrix with very less space cost, and 
	perform a good time complexity when searching for values.
STL compatibility:
	<specifier>iterator is provided to iterate sparse_matrix (row previous);
	<specifier>row_iterator is provided to iterate a specific row of sparse_matrix;
	<specifier>col_iterator is provided to iterate a specific column of sparse_matrix;
	(<speicifer> can be none, const_, reverse_, const_reverse_).
special:
	1. provide reverse directional operations couple with original operations,
		with a prefix 'r' to original name. they can perform faster search of
		right side and down side nodes in sparse matrix.
	2. sparse_matrix = cross_list + index_table, provide a member function:
		sparse(sparse_row_size,sparse_col_size) to build a 'sparse_row_size * sparse_col_size'
		index table to perform random access operations.
	3. provide random access operations couple with original operations,
		with a prefix 'i' to original name. by searching in index table,
		they can provide O(1) search time complexity.
thanks-to:
	1. Thanks to associate professor Bo Xiao, Pattern Recoginition and Itelligent System Lab., BUPT.
		Mr. Xiao told me that hashing index can be a good strategy for static data searching.
author: Yongtian Zhang, yongtianzhang@gmail.com
finish-time: 2011.03.29
last-modified: 2011.06.02

license:
	This code is not under any license, anyone can get it, use it, modified it, redistribute it without notice me. 
	I don't think there is no bug in it, so before you use it, please check and test carefully, I don't promise its quality. ^_^
	If you find any bug, please report it to me, I'll be very grateful to you. Discussion is also welcome.

**************************************************************************/
#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include "../cross_list/cross_list.h"
#include <deque>
#include <list>
#include <utility>

template <typename T>
class sparse_matrix: public cross_list<T>
{
protected:
	typedef cross_list<T> base_type;
	typedef typename base_type::node node;
	typedef std::deque<std::deque<std::list<node*> > > table_type;

public:
	typedef typename base_type::value_type value_type;
	typedef typename base_type::size_type size_type;
	typedef typename base_type::difference_type difference_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::const_pointer const_pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;

public:
	sparse_matrix(size_type row_size=0, size_type col_size=0,
			value_type default_value=value_type());

	sparse_matrix(const sparse_matrix& other);

	~sparse_matrix();

	sparse_matrix& operator = (const sparse_matrix& rhs);

	bool operator == (const sparse_matrix& rhs) const;

	//output to std::ostream
	//requirement: U must meet what os << cross_list<U> needs
	template <typename U>
	friend std::ostream& operator << (std::ostream& os, const sparse_matrix<U>& mat);

	//input from std::istream
	//requirement: U must meet what is >> cross_list<U> needs
	template <typename U>
	friend std::istream& operator >> (std::istream& is, sparse_matrix<U>& mat);

	void sparse(size_type sparse_row_size, size_type sparse_col_size);

	std::pair<size_type,size_type> sparse() const;

	void transpose();

	//erase node of coordinate (row_index,col_index)
	//true if erase successfully, false if node not exist
	//(look for node in index table) !!!faster for random access erase!!!
	bool ierase(size_type row_index, size_type col_index);

	//get value reference of coordinate (row_index,col_index), if not exist, create one
	//(look for node in index table) !!!faster for random access!!!
	//but if node not in matrix, then work as at(size_type,size_type).
	reference iat(size_type row_index,size_type col_index);

	//get value reference of coordinate (row_index,col_index), if not exist, throw an exception
	//(look for node in index table) !!!faster for random access!!!
	const_reference iat(size_type row_index,size_type col_index) const;

	//get value reference of coordinate (row_index,col_index), if not exist, create one
	virtual reference operator () (size_type row_index,size_type col_index)
	{
		return iat(row_index,col_index);
	}

	//get value reference of coordinate (row_index,col_index), if not exist, throw an exception
	virtual const_reference operator () (size_type row_index,size_type col_index) const
	{
		return iat(row_index,col_index);
	}

	//get value of coordinate (row_index,col_index), if not exist, return default value
	//(look for node in index table) !!!faster for random search!!!
	value_type iget(size_type row_index, size_type col_index) const;

	//set value of coordinate (row_index,col_index) to value, if not exist, create one
	//(look for node in index table) !!!faster for random access!!!
	//but if node not in matrix, then work as set(size_type,size_type,const_reference).
	void iset(size_type row_index, size_type col_index, const_reference value);

	//whether a node of coordinate (row_index,col_index) exist
	//(look for node in index table) !!!faster for random access!!!
	bool iexist(size_type row_index,size_type col_index) const;
	
protected:	//internal operations (level -1)
	//look up node (row_index,col_index), if not exist, return left and up side nodes
	//precondition: row_index and col_index are both legal
	//return: return a node if found, NULL otherwise
	node* ilocate(size_type row_index,size_type col_index) const;

	//insert node to left side of ptrs->first and up side of ptrs->second
	virtual void insert_node_before(node* new_node, const std::pair<node*,node*>& ptrs);

	//insert node to right side of ptrs->first and down side of ptrs->second
	virtual void insert_node_after(node* new_node, const std::pair<node*,node*>& ptrs);

	virtual void erase_node(node* p);

protected:	//internal operations (level -2)
	void attach_node_into_table(node* p);

	void detach_node_from_table(node* p);

protected:	//data member(s)
	mutable table_type m_table;	//index table
};

template <typename T> 
sparse_matrix<T>::sparse_matrix(size_type row_num, size_type col_num,
		value_type default_value): base_type(row_num,col_num,default_value)
{
	sparse(1,1);
}

template <typename T>
sparse_matrix<T>::sparse_matrix(const sparse_matrix& other)
{
	sparse(1,1);
	*this = other;
}

template <typename T>
sparse_matrix<T>::~sparse_matrix()
{
	//base_type::~cross_list();
}

template <typename T>
sparse_matrix<T>& sparse_matrix<T>::operator = (const sparse_matrix& rhs)
{
	if (&rhs==this)
		return *this;
	base_type::clear();
	std::pair<size_type,size_type> sp=rhs.sparse();
	sparse(sp.first,sp.second);					//PA: sparse(row,col) must be called before '=', to make up table for elements to attach
	base_type::operator = (rhs);
	return *this;
}

template <typename T>
bool sparse_matrix<T>::operator == (const sparse_matrix& rhs) const
{
	return base_type::operator == (rhs);
}

template <typename T>
std::ostream& operator << (std::ostream& os, const sparse_matrix<T>& mat)
{
	typedef typename sparse_matrix<T>::size_type size_type;
	typedef typename sparse_matrix<T>::base_type base_type;
	std::pair<size_type,size_type> sp=mat.sparse();//output sparse information
	os << "sparse_row_size=" << sp.first << " sparse_col_size=" << sp.second << std::endl;
	os << dynamic_cast<const base_type&>(mat);	//call base class
	return os;
}

template <typename T>
std::istream& operator >> (std::istream& is, sparse_matrix<T>& mat)
{
	typedef typename sparse_matrix<T>::size_type size_type;
	typedef typename sparse_matrix<T>::base_type base_type;
	mat.clear();
	size_type sparse_row_size=0,sparse_col_size=0;
	std::string text;
	getline(is,text,'='); is >> sparse_row_size;//get row sparse
	getline(is,text,'='); is >> sparse_col_size;//get col sparse
	getline(is,text);							//skip '\n'
	mat.sparse(sparse_row_size,sparse_col_size);
	is >> dynamic_cast<base_type&>(mat);		//call base class
	return is;
}

template <typename T>
void sparse_matrix<T>::sparse(size_type sparse_row_size, size_type sparse_col_size)
{
	if (0==sparse_row_size || 0==sparse_col_size)
		throw std::invalid_argument("sparse_matrix<T>::sparse(size_type,size_type): zero size input.");

	//same size as before, no need to reform
	if (m_table.size()==sparse_row_size && m_table.front().size()==sparse_col_size)
		return;

	//clear all node_pointer recorded
	typedef typename table_type::value_type column_type;
	for (typename table_type::iterator row_iter=m_table.begin();
			row_iter!=m_table.end(); ++row_iter)
	{
		for (typename column_type::iterator iter=row_iter->begin();
				iter!=row_iter->end(); ++iter)
		{
			iter->clear();
		}
	}

	//resize table
	m_table.resize(sparse_row_size);
	for (typename table_type::iterator row_iter=m_table.begin();
			row_iter!=m_table.end(); ++row_iter)
	{
		row_iter->resize(sparse_col_size);
	}

	//make up table
	for (typename base_type::iterator node_iter=
			base_type::begin(), node_end=base_type::end();
			node_iter!=node_end; ++node_iter)
	{
		attach_node_into_table(node_of_iterator(node_iter));
	}
}

template <typename T>
std::pair<typename sparse_matrix<T>::size_type,
		typename sparse_matrix<T>::size_type>
	sparse_matrix<T>::sparse() const
{
	assert(!m_table.empty());
	return std::make_pair(m_table.size(),m_table.front().size());
}

template <typename T>
void sparse_matrix<T>::transpose()
{
	base_type::transpose();
	sparse(base_type::col_size(),base_type::row_size());//reform table
}

template <typename T>
bool sparse_matrix<T>::ierase(size_type row_index,size_type col_index)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("sparse_matrix<T>::rerase(size_type,size_type): row_index or col_index illegal");
	node* p=ilocate(row_index,col_index);
	if (NULL==p)
		return false;
	erase_node(p);
	return true;
}

template <typename T>
typename sparse_matrix<T>::reference sparse_matrix<T>::iat(size_type row_index, size_type col_index)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("sparse_matrix<T>::iat(size_type,size_type): row_index or col_index illegal");
		
	node* p=ilocate(row_index,col_index);
	if (p)										//if there is a node there
		return p->value;						//return it
	else										//else
		return at(row_index,col_index);			//return at(row_index,col_index)
}

template <typename T>
typename sparse_matrix<T>::const_reference sparse_matrix<T>::iat(size_type row_index, size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("sparse_matrix<T>::iat(size_type,size_type) const: row_index or col_index illegal");
		
	node* p=ilocate(row_index,col_index);
	if (p)										//if there is a node there
		return p->value;						//return it
	else										//else, report error
		throw std::runtime_error("sparse_matrix<T>::iat(size_type,size_type) const: no value there");
}

template <typename T>
typename sparse_matrix<T>::value_type sparse_matrix<T>::iget(size_type row_index, size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("sparse_matrix<T>::iget(size_type,size_type) const: row_index or col_index illegal");

	node* p=ilocate(row_index,col_index);
	return p ? p->value : base_type::m_dft_val;
}

template <typename T>
void sparse_matrix<T>::iset(size_type row_index, size_type col_index, const_reference value)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("sparse_matrix<T>::iset(size_type,size_type,const_reference): row_index or col_index illegal");

	node* p=ilocate(row_index,col_index);
	if (p)
		p->value = value;
	else
		set(row_index,col_index,value);
}

template <typename T>
bool sparse_matrix<T>::iexist(size_type row_index,size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("sparse_matrix<T>::iexist(size_type,size_type) const: row_index or col_index illegal");
	return ilocate(row_index,col_index)!=NULL;
}

template <typename T> 
typename sparse_matrix<T>::node*
	sparse_matrix<T>::ilocate(size_type row_index, size_type col_index) const
{
	//precondition: !m_table.empty()
	size_type row_mod=row_index % m_table.size();
	size_type col_mod=col_index % m_table.front().size();
	std::list<node*>& nodes=m_table[row_mod][col_mod];
	for(typename std::list<node*>::iterator iter=nodes.begin();
			iter!=nodes.end(); ++iter)
	{
		if (row_index==(*iter)->row && col_index==(*iter)->col)
			return *iter;
	}
	return NULL;
}

template <typename T>
void sparse_matrix<T>::insert_node_before(node* new_node, const std::pair<node*,node*>& ptrs)
{
	base_type::insert_node_before(new_node,ptrs);
	attach_node_into_table(new_node);
}

template <typename T>
void sparse_matrix<T>::insert_node_after(node* new_node, const std::pair<node*,node*>& ptrs)
{
	base_type::insert_node_after(new_node,ptrs);
	attach_node_into_table(new_node);
}

template <typename T>
void sparse_matrix<T>::erase_node(node* p)
{
	detach_node_from_table(p);
	base_type::erase_node(p);
}

template <typename T>
void sparse_matrix<T>::attach_node_into_table(node* p)
{
	size_type row_mod=p->row % m_table.size();
	size_type col_mod=p->col % m_table.front().size();
	m_table[row_mod][col_mod].push_back(p);
}

template <typename T>
void sparse_matrix<T>::detach_node_from_table(node* p)
{
	size_type row_mod=p->row % m_table.size();
	size_type col_mod=p->col % m_table.front().size();
	m_table[row_mod][col_mod].remove(p);
}

#endif //SPARSE_MATRIX_H
