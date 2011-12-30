//cross_list.h
/**************************************************************************
module: cross_list
file(s): cross_list.h
function: class template cross_list, implements an STL style cross list,
	which can be used to store sparse matrix with very less space cost.
STL compatibility:
	<specifier>iterator is provided to iterate cross_list (row previous);
	<specifier>row_iterator is provided to iterate a specific row of cross_list;
	<specifier>col_iterator is provided to iterate a specific column of cross_list;
	(<speicifer> can be none, const_, reverse_, const_reverse_).
special:
	1. provide reverse directional operations couple with original operations,
		with a prefix 'r' to original name. they can perform faster search of
		right side and down side nodes in cross list.
author: Yongtian Zhang, yongtianzhang@gmail.com
finish-time: 2011.03.27
last-modified: 2011.12.29

license:
	This code is not under any license, anyone can get it, use it, modified it, redistribute it without notice me. 
	I don't think there is no bug in it, so before you use it, please check and test carefully, I don't promise its quality. ^_^
	If you find any bug, please report it to me, I'll be very grateful to you. Discussion is also welcome.

**************************************************************************/
#ifndef CROSS_LIST_H
#define CROSS_LIST_H

#include <cstddef>
#include <iterator>
#include <deque>
#include <stdexcept>
#include <utility>
#include <limits>
#include <algorithm>
#include <vector>
#include <cassert>
#include <iostream>
#include <string>
#include <cstring>

#include "../serialization/serialization.h"

namespace cross_list_internal {
	//node that contains cross list values
	template <typename ValueT, typename SizeT=std::size_t>
	struct node
	{
		//basic types
		typedef ValueT value_type;
		typedef SizeT size_type;

		//fields
		value_type value;
		size_type row, col;
		node *left, *right, *up, *down;

		//constructor
		node(value_type v=value_type(),
				size_type row_index=std::numeric_limits<size_type>::max(),
				size_type col_index=std::numeric_limits<size_type>::max()):
			value(v),row(row_index),col(col_index),
			left(NULL),right(NULL),up(NULL),down(NULL)
		{
		};
	};

	//node pointer traits
	template <typename NodePtrT>
	struct node_pointer_traits;

	//node pointer traits specialization for NodeT*
	template <typename NodeT>
	struct node_pointer_traits<NodeT*>
	{
		typedef typename NodeT::value_type value_type;
		typedef value_type& reference;
		typedef value_type* pointer;
	};

	//node pointer traits specialization for const NodeT*
	template <typename NodeT>
	struct node_pointer_traits<const NodeT*>
	{
		typedef typename NodeT::value_type value_type;
		typedef const value_type& reference;
		typedef const value_type* pointer;
	};
}

template <typename T>
class cross_list
{
public:		//interface basic types
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;

protected:	//internal basic types
	typedef cross_list_internal::node<value_type,size_type> node;
	typedef std::deque<node*> headers_type;
	typedef std::deque<size_type> records_type;

protected:	//internal iterator types
	//basic iterator template
	template <typename NodePtrT>
	class basic_iterator
	{
	public:	//basic types
		typedef typename cross_list::size_type size_type;
		typedef NodePtrT node_pointer;
		friend class cross_list;
	public:	//iterator traits
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef typename cross_list::value_type value_type;
		typedef typename cross_list::difference_type difference_type;
		typedef typename cross_list_internal::
			node_pointer_traits<node_pointer>::pointer pointer;
		typedef typename cross_list_internal::
			node_pointer_traits<node_pointer>::reference reference;
	public:	//interface operations
		explicit basic_iterator(node_pointer p=NULL): ptr(p) {}
		operator basic_iterator<const node*>() const
		{
			return basic_iterator<const node*>(ptr);
		}
		reference operator * () {return ptr->value;}
		pointer operator -> () const {return &(ptr->value);}
		//virtual basic_iterator& operator ++ ()
		//virtual basic_iterator& operator -- ();
		bool operator == (const basic_iterator& r) const {return ptr==r.ptr;}
		bool operator != (const basic_iterator& r) const {return ptr!=r.ptr;}
		void step_up() {ptr = ptr->up;}
		void step_down() {ptr = ptr->down;}
		void step_left() {ptr = ptr->left;}
		void step_right() {ptr = ptr->right;}
		size_type row_index() const {return ptr->row;}
		size_type col_index() const {return ptr->col;}

		template <typename NodePtrT2>			//compare position in matrix logic
		bool pos_less(const basic_iterator<NodePtrT2>& r) const
		{
			if (row_index()!=r.row_index())
				return row_index()<r.row_index();
			else //row_index()==r.row_index()
				return col_index()<r.col_index();
		}
	protected:	//current node contains value
		node_pointer ptr;
	}; 
	
	//get node of iterator
	template <typename NodePtrT>
	static typename basic_iterator<NodePtrT>::node_pointer&
		node_of_iterator(basic_iterator<NodePtrT>& iter)
	{
		return iter.ptr;
	}

	//basic iterator template to iterate a row
	template <typename NodePtrT>
	class basic_row_iterator: public basic_iterator<NodePtrT>
	{
	protected:
		typedef basic_iterator<NodePtrT> iter_base;
	public:
		typedef typename iter_base::node_pointer node_pointer;
		//friend class cross_list;
	public:	//defines ++ and -- operations of iterator
		explicit basic_row_iterator(node_pointer p=NULL): iter_base(p) {}
		operator basic_row_iterator<const node*>() const
		{
			return basic_row_iterator<const node*>(iter_base::ptr);
		}
		basic_row_iterator& operator ++ () {iter_base::step_right(); return *this;}
		basic_row_iterator& operator -- () {iter_base::step_left(); return *this;}
		basic_row_iterator operator ++ (int)
		{
			basic_row_iterator t(*this);
			++(*this); return t;
		}
		basic_row_iterator operator -- (int)
		{
			basic_row_iterator t(*this);
			--(*this); return t;
		}
	};

	//basic iterator template to iterate a column
	template <typename NodePtrT>
	class basic_col_iterator: public basic_iterator<NodePtrT>
	{
	protected:
		typedef basic_iterator<NodePtrT> iter_base;
	public:
		typedef typename iter_base::node_pointer node_pointer;
	public:	//defines ++ and -- operations of iterator
		explicit basic_col_iterator(node_pointer p=NULL): iter_base(p) {}
		operator basic_col_iterator<const node*> () const
		{
			return basic_col_iterator<const node*>(iter_base::ptr);
		}
		basic_col_iterator& operator ++ () {iter_base::step_down(); return *this;}
		basic_col_iterator& operator -- () {iter_base::step_up(); return *this;}
		basic_col_iterator operator ++ (int)
		{
			basic_col_iterator t(*this);
			++(*this); return t;
		}
		basic_col_iterator operator -- (int)
		{
			basic_col_iterator t(*this);
			--(*this); return t;
		}
	};

	//basic iterator template to iterate entire cross list
	friend class basic_cursor_iterator;
	template <typename NodePtrT>
	class basic_cursor_iterator: public basic_iterator<NodePtrT>
	{
	protected:
		typedef basic_iterator<NodePtrT> iter_base;
		using iter_base::ptr;
	public:
		typedef typename iter_base::node_pointer node_pointer;
		//friend class cross_list;
	public:	//defines ++ and -- operations of iterator
		explicit basic_cursor_iterator(node_pointer p,
				size_type row_index, const cross_list* p_cl):
			iter_base(p), row(row_index), pcl(p_cl)
		{
			forward_adjust();
		}
		operator basic_cursor_iterator<const node*> () const //beautiful
		{
			return basic_cursor_iterator<const node*>(ptr,row,pcl);
		}
		basic_cursor_iterator& operator ++ ()
		{
			iter_base::step_right(); forward_adjust(); return *this;
		}
		basic_cursor_iterator& operator -- ()
		{
			iter_base::step_left(); backword_adjust(); return *this;
		}
		basic_cursor_iterator operator ++ (int)
		{
			basic_cursor_iterator t(*this);
			++(*this); return t;
		}
		basic_cursor_iterator operator -- (int)
		{
			basic_cursor_iterator t(*this);
			--(*this); return t;
		}
	protected:
		void forward_adjust()
		{
			//when ptr point to current row's tailer and ptr not reach last row
			while (row<pcl->last_internal_row() && ptr==pcl->m_headers[row])
				ptr = pcl->m_headers[++row]->right;//ptr move to next row's first node
		}
		void backword_adjust()
		{
			//when ptr point to current row's header and ptr not reach first row
			while (row>0 && ptr==pcl->m_headers[row])
				ptr = pcl->m_headers[--row]->left;//ptr move to previous row's last node
		}
	protected:
		size_type row;
		const cross_list* pcl;
	};

	//basic iterator template to reverse a iterator
	template <typename Iterator>
	class basic_reverse_iterator: public Iterator
	{
	protected:
		typedef Iterator base_type;
	public:	//defines ++ and -- operations of iterator
		explicit basic_reverse_iterator(const base_type& iter): base_type(iter)
		{
			--dynamic_cast<base_type&>(*this);
		}

		template <typename Iterator2>
		operator basic_reverse_iterator<Iterator2> () const
		{
			basic_reverse_iterator iter(*this);	//temporary iterator
			++dynamic_cast<base_type&>(iter);	//restore base iterator
			return basic_reverse_iterator<Iterator2>(
					dynamic_cast<base_type&>(iter));//construct
		}
		basic_reverse_iterator& operator ++ ()
		{
			--dynamic_cast<base_type&>(*this); return *this;
		}
		basic_reverse_iterator& operator -- ()
		{
			++dynamic_cast<base_type&>(*this); return *this;
		}
		basic_reverse_iterator operator ++ (int)
		{
			basic_reverse_iterator t(*this);
			++(*this); return t;
		}
		basic_reverse_iterator operator -- (int)
		{
			basic_reverse_iterator t(*this);
			--(*this); return t;
		}
	};

public:	//interface iterator types
	typedef basic_cursor_iterator<node*> iterator;
	typedef basic_cursor_iterator<const node*> const_iterator;
	typedef basic_row_iterator<node*> row_iterator;
	typedef basic_row_iterator<const node*> const_row_iterator;
	typedef basic_col_iterator<node*> col_iterator;
	typedef basic_col_iterator<const node*> const_col_iterator;

	typedef basic_reverse_iterator<iterator> reverse_iterator;
	typedef basic_reverse_iterator<const_iterator> const_reverse_iterator;
	typedef basic_reverse_iterator<row_iterator> reverse_row_iterator;
	typedef basic_reverse_iterator<const_row_iterator> const_reverse_row_iterator;
	typedef basic_reverse_iterator<col_iterator> reverse_col_iterator;
	typedef basic_reverse_iterator<const_col_iterator> const_reverse_col_iterator;

public:	//iterator creaters
	iterator begin();
	iterator end();
	row_iterator begin_of_row(size_type row_index);
	row_iterator end_of_row(size_type row_index);
	col_iterator begin_of_col(size_type col_index);
	col_iterator end_of_col(size_type col_index);

	const_iterator begin() const {return cbegin();}
	const_iterator end() const {return cend();}
	const_row_iterator begin_of_row(size_type row_index) const
	{
		return cbegin_of_row(row_index);
	}

	const_row_iterator end_of_row(size_type row_index) const
	{
		return cend_of_row(row_index);
	}

	const_col_iterator begin_of_col(size_type col_index) const
	{
		return cbegin_of_col(col_index);
	}

	const_col_iterator end_of_col(size_type col_index) const
	{
		return cend_of_col(col_index);
	}


	const_iterator cbegin() const;
	const_iterator cend() const;
	const_row_iterator cbegin_of_row(size_type row_index) const;
	const_row_iterator cend_of_row(size_type row_index) const;
	const_col_iterator cbegin_of_col(size_type col_index) const;
	const_col_iterator cend_of_col(size_type col_index) const;

	reverse_iterator rbegin() {return reverse_iterator(end());}
	reverse_iterator rend() {return reverse_iterator(begin());}
	reverse_row_iterator rbegin_of_row(size_type row_index)
	{
		return reverse_row_iterator(end_of_row(row_index));
	}
	reverse_row_iterator rend_of_row(size_type row_index)
	{
		return reverse_row_iterator(begin_of_row(row_index));
	}
	reverse_col_iterator rbegin_of_col(size_type col_index)
	{
		return reverse_col_iterator(end_of_col(col_index));
	}
	reverse_col_iterator rend_of_col(size_type col_index)
	{
		return reverse_col_iterator(begin_of_col(col_index));
	}

	const_reverse_iterator rbegin() const {return crbegin();}
	const_reverse_iterator rend() const {return crend();}
	const_reverse_row_iterator rbegin_of_row(size_type row_index) const
	{
		return crbegin_of_row(row_index);
	}
	const_reverse_row_iterator rend_of_row(size_type row_index) const
	{
		return crend_of_row(row_index);
	}
	const_reverse_col_iterator rbegin_of_col(size_type col_index) const
	{
		return crbegin_of_col(col_index);
	}
	const_reverse_col_iterator rend_of_col(size_type col_index) const
	{
		return crend_of_col(col_index);
	}

	const_reverse_iterator crbegin() const {return const_reverse_iterator(cend());}
	const_reverse_iterator crend() const {return const_reverse_iterator(cbegin());}
	const_reverse_row_iterator crbegin_of_row(size_type row_index) const
	{
		return const_reverse_row_iterator(cend_of_row(row_index));
	}
	const_reverse_row_iterator crend_of_row(size_type row_index) const
	{
		return const_reverse_row_iterator(cbegin_of_row(row_index));
	}
	const_reverse_col_iterator crbegin_of_col(size_type col_index) const
	{
		return const_reverse_col_iterator(cend_of_col(col_index));
	}
	const_reverse_col_iterator crend_of_col(size_type col_index) const
	{
		return const_reverse_col_iterator(cbegin_of_col(col_index));
	}

public:	//interface operations
	cross_list(size_type row_size=0, size_type col_size=0,
			value_type default_value=value_type());

	cross_list(const cross_list& other);

	virtual ~cross_list();

	cross_list& operator = (const cross_list& rhs);

	bool operator == (const cross_list& rhs) const;

	//transpose as matrix
	void transpose();

	//insert node at coordinate (row_index,col_index)
	//true if insert successfully, false if node already exist
	//(look for node from left to right, from up to down) !!!faster for head insertion!!!
	bool insert(size_type row_index, size_type col_index, const_reference value);

	//insert node at coordinate (row_index,col_index)
	//true if insert successfully, false if node already exist
	//(look for node from right to left, from down to up) !!!faster for tail insertion!!!
	bool rinsert(size_type row_index, size_type col_index, const_reference value);

	//erase node of coordinate (row_index,col_index)
	//true if erase successfully, false if node not exist
	//(look for node from left to right, from up to down) !!!faster for head erase!!!
	bool erase(size_type row_index, size_type col_index);

	//erase node of coordinate (row_index,col_index)
	//true if erase successfully, false if node not exist
	//(look for node from right to left, from down to up) !!!faster for tail erase!!!
	bool rerase(size_type row_index, size_type col_index);

	template <typename Iterator> //Iterator can be iterator, row_iterator or col_iterator
	void erase(Iterator iter);

	template <typename Iterator> //Iterator can be iterator, row_iterator or col_iterator
	void erase(Iterator first,Iterator last);

	//get value reference of coordinate (row_index,col_index), if not exist, create one
	//(look for node from left to right, from up to down) !!!faster for head search!!!
	reference at(size_type row_index,size_type col_index);

	//get value reference of coordinate (row_index,col_index), if not exist, throw an exception
	//(look for node from left to right, from up to down) !!!faster for head search!!!
	const_reference at(size_type row_index,size_type col_index) const;

	//get value reference of coordinate (row_index,col_index), if not exist, create one
	//(look for node from right to left, from down to up) !!!faster for head search!!!
	reference rat(size_type row_index,size_type col_index);

	//get value reference of coordinate (row_index,col_index), if not exist, throw an exception
	//(look for node from right to left, from down to up) !!!faster for head search!!!
	const_reference rat(size_type row_index,size_type col_index) const;

	//get value reference of coordinate (row_index,col_index), if not exist, create one
	//(look for node from left to right, from up to down) !!!faster for head search!!!
	//virtual interface for optimization of derived class
	virtual reference operator () (size_type row_index,size_type col_index)
	{
		return at(row_index,col_index);
	}

	//get value reference of coordinate (row_index,col_index), if not exist, throw an exception
	//(look for node from left to right, from up to down) !!!faster for head search!!!
	//virtual interface for optimization of derived class
	virtual const_reference operator () (size_type row_index,size_type col_index) const
	{
		return at(row_index,col_index);
	}

	//get value of coordinate (row_index,col_index), if not exist, return default value
	//(look for node from left to right, from up to down) !!!faster for head search!!!
	value_type get(size_type row_index, size_type col_index) const;

	//get value of coordinate (row_index,col_index), if not exist, return default value
	//(look for node from right to left, from down to up) !!!faster for tail search!!!
	value_type rget(size_type row_index, size_type col_index) const;

	//set value of coordinate (row_index,col_index) to value, if not exist, create one
	//(look for node from left to right, from up to down) !!!faster for head insertion!!!
	void set(size_type row_index, size_type col_index, const_reference value);

	//set value of coordinate (row_index,col_index) to value, if not exist, create one
	//(look for node from right to left, from down to up) !!!faster for tail insertion!!!
	void rset(size_type row_index, size_type col_index, const_reference value);

	//whether a node of coordinate (row_index,col_index) exist
	//(look for node from left to right, from up to down) !!!faster for head search!!!
	bool exist(size_type row_index,size_type col_index) const;

	//whether a node of coordinate (row_index,col_index) exist
	//(look for node from right to left, from down to up) !!!faster for tail search!!!
	bool rexist(size_type row_index,size_type col_index) const;

	bool empty() const {return size()==0;}

	//empty the cross list
	void clear() {erase(begin(),end());}

	//reset maximum nodes that cross list can contains
	void resize(size_type row_new_size, size_type col_new_size);

	//reset maximum rows that cross list can contains
	void row_resize(size_type new_size);

	//reset maximum columns that cross list can contains
	void col_resize(size_type new_size);

	//how many nodes in cross list
	size_type size() const {return m_size;}

	//how many rows in cross list
	size_type row_size() const {return m_r_rec.size();}

	//how many columns in cross list
	size_type col_size() const {return m_c_rec.size();}

	//how many nodes in row
	size_type size_of_row(size_type row_index) const;

	//how many nodes in column
	size_type size_of_col(size_type col_index) const;
	
protected:	//internal operations (level -1)
	//fill null pointer range [first,last) with new allocated default header
	void fill_headers(typename headers_type::iterator first, typename headers_type::iterator last);

	//set non-null pointer range [firs,last) to null pointers, delete original nodes
	void empty_headers(typename headers_type::iterator first, typename headers_type::iterator last);

	//set fields of nodes in range [first,last) to header's default state
	void reset_headers(typename headers_type::iterator first, typename headers_type::iterator last);

	//check if a row index is in legal range
	bool is_legal_row(size_type row_index) const {return row_index<row_size();}

	//check if a column index is in legal range
	bool is_legal_col(size_type col_index) const {return col_index<col_size();}

	//get last internal row index of m_headers
	//precondition: assume !m_headers.empty()==true
	size_type last_internal_row() const {return row_size()>0 ? row_size()-1 : 0;}

	//look up node (row_index,col_index), if not exist, return right and down side nodes
	//precondition: row_index and col_index are both legal
	std::pair<node*,node*> locate(size_type row_index,size_type col_index) const;

	//look up node (row_index,col_index), if not exist, return left and up side nodes
	//precondition: row_index and col_index are both legal
	std::pair<node*,node*> rlocate(size_type row_index,size_type col_index) const;

	//check if ptrs denote a node at (row_index,col_index)
	bool has_node_at(const std::pair<node*,node*>& ptrs,
			size_type row_index, size_type col_index) const
	{
		return ptrs.first==ptrs.second &&
			ptrs.first->col==col_index && ptrs.second->row==row_index;
	}

	//allocate a node, with pointer fields uninitialized (depend on node's constructor)
	virtual node* new_node(const_reference value, size_type row_index, size_type col_index)
	{
		return new node(value, row_index, col_index);
	}

	//insert node to left side of ptrs->first and up side of ptrs->second
	virtual void insert_node_before(node* new_node, const std::pair<node*,node*>& ptrs);

	//insert node to right side of ptrs->first and down side of ptrs->second
	virtual void insert_node_after(node* new_node, const std::pair<node*,node*>& ptrs);

	//precondition: node is in cross list and not headers
	virtual void erase_node(node* p);

protected:	//internal operations (level -2)
	//allocate a header node, fill its fields with header's default value
	node* new_header();

	//reset header node's fields to header's default value
	void reset_header(node* p);

	//deallocate a node, release memory space
	virtual void delete_node(node* p) {delete p;}

	//attach node 'new_node' on left side of node 'pos'
	void attach_node_left_of(node* new_node, node* pos);

	//attach node 'new_node' on right side of node 'pos'
	void attach_node_right_of(node* new_node, node* pos);

	//attach node 'new_node' on up side of node 'pos'
	void attach_node_up_of(node* new_node, node* pos);

	//attach node 'new_node' on down side of node 'pos'
	void attach_node_down_of(node* new_node, node* pos);

	//detach node from its neighbours
	void detach_node(node* p);

	//increase records of node by 1
	void increase_record(node* p);

	//decrease records of node by 1
	void decrease_record(node* p);

protected:	//data members
	headers_type m_headers;	//headers
	records_type m_r_rec;	//record node amount of each row
	records_type m_c_rec;	//record node amount of each column
	size_type m_size;		//record node amount of entire cross list
	value_type m_dft_val;	//default value of 'no node there'
};

//---------------------------------free functions----------------------------------------

//precondition: each cross_list in [cross_list_first,cross_list_last) have there
//	same dimension with result
template <typename InputIterator, typename T>
void accumulate_to(
		InputIterator cross_list_first, InputIterator cross_list_last,
		cross_list<T>& result)
{
	typedef cross_list<T> cross_list_type;
	typedef typename cross_list_type::value_type element_type;
	typedef typename cross_list_type::size_type size_type;
	typedef typename cross_list_type::const_iterator const_iterator;
	typedef std::vector<const_iterator> const_iterator_vector;
	result.clear();

	if (cross_list_first==cross_list_last)
		return;

	std::vector<InputIterator> cross_lists;
	const_iterator_vector iters;
	for (; cross_list_first!=cross_list_last; ++cross_list_first)
	{
		if (cross_list_first->row_size()!=result.row_size() ||
				cross_list_first->col_size()!=result.col_size())
			throw std::invalid_argument("accumulate_to(InputIterator,InputIterator,cross_list<T>&): different row_size or col_size of cross lists");
		cross_lists.push_back(cross_list_first);
		iters.push_back(cross_list_first->cbegin());
	}
	
	while (true)
	{
		//find minimum coordinate of current iterators
		//precondition: !iters.empty()
		size_type max_num=std::numeric_limits<size_type>::max();
		size_type min_row=max_num, min_col=max_num;
		for (typename const_iterator_vector::size_type i=0; i<iters.size(); ++i)
		{
			if (iters[i]==cross_lists[i]->cend())	//skip end iterator
				continue;

			if (iters[i].row_index()<min_row)		//find latest coordinate
			{
				min_row = iters[i].row_index();
				min_col = iters[i].col_index();
			}
			else if (iters[i].row_index()==min_row
					&& iters[i].col_index()<min_col)
			{
				min_col = iters[i].col_index();
			}
		}
		if (max_num==min_row || max_num==min_col)	//all iterator skipped
			break;

		//sum of minimum coordinate of each cross list
		//precondition: at least one element exist
		element_type sum=element_type();
		typename const_iterator_vector::size_type i=0;
		for (; i<iters.size(); ++i)					//find first value to assign
		{
			if (iters[i].row_index()==min_row && iters[i].col_index()==min_col)
			{
				sum = *(iters[i]++);
				break;
			}
		}
		for (; i<iters.size(); ++i)					//other value to accumulate
		{
			if (iters[i].row_index()==min_row && iters[i].col_index()==min_col)
				sum += *(iters[i]++);
		}
		result.rset(min_row,min_col,sum);
	}
}

//output to ostream
//requirement: os << T must be well defined.
template <typename T>
std::ostream& operator << (std::ostream& os, const cross_list<T>& mat)
{
    typedef serialization::sparse_matrix::Cell<T> cell_type;
    typedef serialization::sparse_matrix::Dimension dimension_type;
	for (typename cross_list<T>::size_type r=0; r<mat.row_size(); ++r)
	{
		if (mat.size_of_row(r)==0)
			continue;
		typename cross_list<T>::const_row_iterator row_iter=mat.cbegin_of_row(r);
		typename cross_list<T>::const_row_iterator row_end=mat.cend_of_row(r);
		for (; row_iter!=row_end; ++row_iter)	// write every node
            os << cell_type(row_iter.row_index(), row_iter.col_index(), *row_iter);
		os << std::endl;
	}
	os << dimension_type(mat.row_size(), mat.col_size()) << std::endl;
	return os;
}

//input from istream
//requirement: is >> T must be well defined, compatible with space ending
template <typename T>
std::istream& operator >> (std::istream& is, cross_list<T>& mat)
{
	typedef typename cross_list<T>::size_type size_type;
	typedef typename cross_list<T>::value_type value_type;
    typedef serialization::sparse_matrix::Cell<T> cell_type;
    typedef serialization::sparse_matrix::Dimension dimension_type;
	mat.clear();
    cell_type cell;
    dimension_type dimension;
    char beg_ch=0;
    while (is >> beg_ch)  // for each line
    {
        is.putback(beg_ch);
        switch (beg_ch) {
        case '(':  // cell unit
            is >> cell;
            if (cell.row >= mat.row_size())
                mat.row_resize(cell.row + 1);
            if (cell.column >= mat.col_size())
                mat.col_resize(cell.column + 1);
            mat.rinsert(cell.row, cell.column, cell.value);
            break;
        case '[':  // dimension
            is >> dimension;
            mat.resize(dimension.row, dimension.column);
            break;
        default:
            break;
        }
    }
	return is;
}

//-----------------------------class member functions------------------------------------
template <typename T> 
cross_list<T>::cross_list(size_type row_num, size_type col_num, value_type default_value):
	m_headers(std::max(row_num,col_num)),
	m_r_rec(row_num,0),m_c_rec(col_num,0),m_size(0),m_dft_val(default_value)
{
	if (m_headers.empty())
		m_headers.resize(1);
	fill_headers(m_headers.begin(),m_headers.end());
}

template <typename T>
cross_list<T>::cross_list(const cross_list& other): m_headers(1),m_size(0),m_dft_val(value_type())
{
	fill_headers(m_headers.begin(),m_headers.end());
	*this=other;
}

template <typename T>
cross_list<T>::~cross_list()
{
	clear();
	empty_headers(m_headers.begin(),m_headers.end());
}

template <typename T>
cross_list<T>& cross_list<T>::operator = (const cross_list& rhs)
{
	if (&rhs==this)
		return *this;

	//reset members
	clear();
	m_dft_val = rhs.m_dft_val;
	if (m_headers.size()<rhs.m_headers.size())
	{
		size_type old_size=m_headers.size();
		m_headers.resize(rhs.m_headers.size(),NULL);
		fill_headers(m_headers.begin()+old_size,m_headers.end());
	}
	else
	{
		size_type new_size=rhs.m_headers.size();
		empty_headers(m_headers.begin()+new_size,m_headers.end());
		m_headers.resize(new_size);
		reset_headers(m_headers.begin(),m_headers.end());
	}
	m_r_rec.clear(); m_r_rec.resize(rhs.m_r_rec.size(),0);
	m_c_rec.clear(); m_c_rec.resize(rhs.m_c_rec.size(),0);
	m_size = 0;

	//insert nodes
	typename cross_list::const_iterator iter=rhs.cbegin();
	typename cross_list::const_iterator end=rhs.cend();
	for (; iter!=end; ++iter)
	{
		size_type row=iter.row_index(), col=iter.col_index();
		node* p=new_node(*iter,row,col);
		std::pair<node*,node*> ptrs=
			std::make_pair(m_headers[row],m_headers[col]);
		insert_node_before(p,ptrs);
	}

	return *this;
}

template <typename T>
bool cross_list<T>::operator == (const cross_list& rhs) const
{
	if (row_size()!=rhs.row_size() || col_size()!=rhs.col_size())//compare structure
		return false;

	if (size()!=rhs.size())							//compare node amount
		return false;

	const_iterator l_iter=cbegin(),l_end=cend();
	const_iterator r_iter=rhs.cbegin(),r_end=rhs.cend();
	for (; l_iter!=l_end && r_iter!=r_end; ++l_iter,++r_iter)//compare node members
	{
		if (l_iter.row_index()!=r_iter.row_index())
			return false;
		if (l_iter.col_index()!=r_iter.col_index())
			return false;
		if (*l_iter!=*r_iter)
			return false;
	}
	//if (l_iter!=l_end || r_iter!=r_end)			//compare node amount
	//	return false;

	return true;
}

template <typename T>
void cross_list<T>::transpose()
{
	//swap records
	std::swap(m_r_rec,m_c_rec);

	//swap fields inside node
	for (iterator node_iter=begin(), node_end=end();
			node_iter!=node_end;)
	{
		node* p=node_of_iterator(node_iter);
		++node_iter; //must increase before ptr's fields changes
		std::swap(p->row,p->col);
		std::swap(p->left,p->up);
		std::swap(p->right,p->down);
	}

	//swap header fields
	for (typename headers_type::iterator
			header_iter=m_headers.begin();
			header_iter!=m_headers.end(); ++header_iter)
	{
		node* p=*header_iter;
		std::swap(p->left,p->up);
		std::swap(p->right,p->down);
	}
}

template <typename T>
bool cross_list<T>::insert(size_type row_index,size_type col_index,const_reference value)
{//this function ensure no node be placed wrong, what as well is its assumption.
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::insert(size_type,size_type,const_reference): row_index or col_index illegal");
	
	//find place to insert node
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	
	//operation
	if (has_node_at(ptrs,row_index,col_index))//if there is a node before
		return false;
	else	//no node there before
	{
		//insert a new allocated node
		node* p=new_node(value,row_index,col_index);
		insert_node_before(p,ptrs);
		return true;
	}
}

template <typename T>
bool cross_list<T>::rinsert(size_type row_index,size_type col_index,const_reference value)
{//this function ensure no node be placed wrong, what as well is its assumption.
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::rinsert(size_type,size_type,const_reference): row_index or col_index illegal");
	
	//find place to insert node
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	
	//operation
	if (has_node_at(ptrs,row_index,col_index))//if there is a node before
		return false;
	else	//no node there before
	{
		//insert a new allocated node
		node* p=new_node(value,row_index,col_index);
		insert_node_after(p,ptrs);
		return true;
	}
}

template <typename T>
bool cross_list<T>::erase(size_type row_index,size_type col_index)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::erase(size_type,size_type): row_index or col_index illegal");
	
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))
	{
		erase_node(ptrs.first);
		return true;
	}
	else
		return false;
}

template <typename T>
bool cross_list<T>::rerase(size_type row_index,size_type col_index)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::rerase(size_type,size_type): row_index or col_index illegal");
	
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))
	{
		erase_node(ptrs.first);
		return true;
	}
	else
		return false;
}

template <typename T>
template <typename Iterator>
void cross_list<T>::erase(Iterator iter)
{
	node* p=node_of_iterator(iter);
	if (NULL==p || !is_legal_row(p->row) || !is_legal_col(p->col))
		throw std::invalid_argument("cross_list<T>::erase(Iterator): illegal iterator");
	erase_node(p);
}

template <typename T>
template <typename Iterator>
void cross_list<T>::erase(Iterator first, Iterator last)
{
	while (first!=last)
	{
		node* p=node_of_iterator(first);
		++first;
		if (NULL==p || !is_legal_row(p->row) || !is_legal_col(p->col))
			throw std::invalid_argument("cross_list<T>::erase(Iterator,Iterator): range contains illegal iterator");
		erase_node(p);
	}
}

template <typename T>
typename cross_list<T>::reference cross_list<T>::at(size_type row_index,size_type col_index)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::at(size_type,size_type): row_index or col_index illegal");
		
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node there
		return ptrs.first->value;				//return it
	else										//else, create one, and return it
	{
		node* p=new_node(m_dft_val,row_index,col_index);
		insert_node_before(p,ptrs);
		return p->value;
	}
}

template <typename T>
typename cross_list<T>::const_reference cross_list<T>::at(size_type row_index, size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::at(size_type,size_type) const: row_index or col_index illegal");
		
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node there
		return ptrs.first->value;				//return it
	else										//else, report error
		throw std::runtime_error("cross_list<T>::at(size_type,size_type) const: no value there");
}

template <typename T>
typename cross_list<T>::reference cross_list<T>::rat(size_type row_index,size_type col_index)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::rat(size_type,size_type): row_index or col_index illegal");
		
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node there
		return ptrs.first->value;				//return it
	else										//else, create one, and return it
	{
		node* p=new_node(m_dft_val,row_index,col_index);
		insert_node_after(p,ptrs);
		return p->value;
	}
}

template <typename T>
typename cross_list<T>::const_reference cross_list<T>::rat(size_type row_index, size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::rat(size_type,size_type) const: row_index or col_index illegal");
		
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node there
		return ptrs.first->value;				//return it
	else										//else, report error
		throw std::runtime_error("cross_list<T>::rat(size_type,size_type) const: no value there");
}

template <typename T>
typename cross_list<T>::value_type cross_list<T>::get(size_type row_index, size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::get(size_type,size_type) const: row_index or col_index illegal");
		
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node there
		return ptrs.first->value;				//return its value field
	else										//no node there
		return m_dft_val;						//return default value
}

template <typename T>
typename cross_list<T>::value_type cross_list<T>::rget(size_type row_index, size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::rget(size_type,size_type) const: row_index or col_index illegal");
		
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node there
		return ptrs.first->value;				//return its value field
	else										//no node there
		return m_dft_val;						//return default value
}

template <typename T>
void cross_list<T>::set(size_type row_index, size_type col_index, const_reference value)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::set(size_type,size_type,const_reference): row_index or col_index illegal");
	
	//find node or place to insert node
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	
	//operation
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node before
		ptrs.first->value = value;
	else										//no node there before
	{
		//insert a new allocated node
		node* p=new_node(value,row_index,col_index);
		insert_node_before(p,ptrs);
	}
}

template <typename T>
void cross_list<T>::rset(size_type row_index, size_type col_index, const_reference value)
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::rset(size_type,size_type,const_reference): row_index or col_index illegal");
	
	//find node or place to insert node
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	
	//operation
	if (has_node_at(ptrs,row_index,col_index))	//if there is a node before
		ptrs.first->value = value;
	else										//no node there before
	{
		//insert a new allocated node
		node* p=new_node(value,row_index,col_index);
		insert_node_after(p,ptrs);
	}
}

template <typename T>
bool cross_list<T>::exist(size_type row_index,size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::exist(size_type,size_type) const: row_index or col_index illegal");
	std::pair<node*,node*> ptrs=locate(row_index,col_index);
	return has_node_at(ptrs,row_index,col_index);
}

template <typename T>
bool cross_list<T>::rexist(size_type row_index,size_type col_index) const
{
	if (!is_legal_row(row_index) || !is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::rexist(size_type,size_type) const: row_index or col_index illegal");
	std::pair<node*,node*> ptrs=rlocate(row_index,col_index);
	return has_node_at(ptrs,row_index,col_index);
}

template <typename T>
void cross_list<T>::resize(size_type row_new_size, size_type col_new_size)
{
	row_resize(row_new_size);
	col_resize(col_new_size);
}

template <typename T>
void cross_list<T>::row_resize(size_type new_size)
{
	size_type old_size=row_size();
	if (old_size<new_size)
	{
		//resize containers
		m_r_rec.resize(new_size,0);
		if (m_headers.size() < new_size)
		{
			size_type headers_old_size=m_headers.size();
			m_headers.resize(new_size,NULL);
			fill_headers(m_headers.begin()+headers_old_size,m_headers.end());
		}
	}
	else if (old_size>new_size)
	{
		//erase nodes of rows [new_size,old_size)
		for (size_type r=new_size; r<old_size; ++r)
			erase(begin_of_row(r),end_of_row(r));

		//resize containers
		m_r_rec.resize(new_size);
		size_type max_dim=std::max(row_size(),col_size());
		max_dim = max_dim>0 ? max_dim : 1;
		empty_headers(m_headers.begin()+max_dim,m_headers.end());
		m_headers.resize(max_dim);
	}
}

template <typename T>
void cross_list<T>::col_resize(size_type new_size)
{
	size_type old_size=col_size();
	if (old_size<new_size)
	{
		//resize containers
		m_c_rec.resize(new_size,0);
		if (m_headers.size() < new_size)
		{
			size_type headers_old_size=m_headers.size();
			m_headers.resize(new_size,NULL);
			fill_headers(m_headers.begin()+headers_old_size,m_headers.end());
		}
	}
	else if (old_size>new_size)
	{
		//erase nodes of columns [new_size,old_size)
		for (size_type c=new_size; c<old_size; ++c)
			erase(begin_of_col(c),end_of_col(c));

		//resize containers
		m_c_rec.resize(new_size);
		size_type max_dim=std::max(row_size(),col_size());
		max_dim = max_dim>0 ? max_dim : 1;
		empty_headers(m_headers.begin()+max_dim,m_headers.end());
		m_headers.resize(max_dim);
	}
}

template <typename T>
typename cross_list<T>::size_type cross_list<T>::size_of_row(size_type row_index) const
{
	if (!is_legal_row(row_index))
		throw std::out_of_range("cross_list<T>::size_of_row(size_type) const: row_index illegal");
	return m_r_rec[row_index];
}

template <typename T>
typename cross_list<T>::size_type cross_list<T>::size_of_col(size_type col_index) const
{
	if (!is_legal_row(col_index))
		throw std::out_of_range("cross_list<T>::size_of_col(size_type) const: col_index illegal");
	return m_c_rec[col_index];
}

template <typename T>
typename cross_list<T>::iterator cross_list<T>::begin()
{
	//if (row_size()==0 || col_size()==0)
	//	throw std::out_of_range("cross_list<T>::begin(): zero size cross list");
	assert(!m_headers.empty());
	return iterator(m_headers[0]->right,0,this);
}

template <typename T>
typename cross_list<T>::iterator cross_list<T>::end()
{
	//if (row_size()==0 || col_size()==0)
	//	throw std::out_of_range("cross_list<T>::end(): zero size cross list");
	assert(!m_headers.empty());
	size_type last_row=last_internal_row();
	return iterator(m_headers[last_row],last_row,this);
}

template <typename T>
typename cross_list<T>::const_iterator cross_list<T>::cbegin() const
{
	//if (row_size()==0 || col_size()==0)
	//	throw std::out_of_range("cross_list<T>::cbegin(): zero size cross list");
	assert(!m_headers.empty());
	return const_iterator(m_headers[0]->right,0,this);
}

template <typename T>
typename cross_list<T>::const_iterator cross_list<T>::cend() const
{
	//if (row_size()==0 || col_size()==0)
	//	throw std::out_of_range("cross_list<T>::cend(): zero size cross list");
	assert(!m_headers.empty());
	size_type last_row=last_internal_row();
	return const_iterator(m_headers[last_row],last_row,this);
}

template <typename T>
typename cross_list<T>::row_iterator cross_list<T>::begin_of_row(size_type row_index)
{
	if (!is_legal_row(row_index))
		throw std::out_of_range("cross_list<T>::begin_of_row(size_type): row_index illegal");
	return row_iterator(m_headers[row_index]->right);
}

template <typename T>
typename cross_list<T>::col_iterator cross_list<T>::begin_of_col(size_type col_index)
{
	if (!is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::begin_of_col(size_type): col_index illegal");
	return col_iterator(m_headers[col_index]->down);
}

template <typename T>
typename cross_list<T>::row_iterator cross_list<T>::end_of_row(size_type row_index)
{
	if (!is_legal_row(row_index))
		throw std::out_of_range("cross_list<T>::end_of_row(size_type): row_index illegal");
	return row_iterator(m_headers[row_index]);
}

template <typename T>
typename cross_list<T>::col_iterator cross_list<T>::end_of_col(size_type col_index)
{
	if (!is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::end_of_col(size_type): col_index illegal");
	return col_iterator(m_headers[col_index]);
}

template <typename T>
typename cross_list<T>::const_row_iterator cross_list<T>::cbegin_of_row(size_type row_index) const
{
	if (!is_legal_row(row_index))
		throw std::out_of_range("cross_list<T>::cbegin_of_row(size_type) const: row_index illegal");
	return const_row_iterator(m_headers[row_index]->right);
}

template <typename T>
typename cross_list<T>::const_col_iterator cross_list<T>::cbegin_of_col(size_type col_index) const
{
	if (!is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::cbegin_of_col(size_type) const: col_index illegal");
	return const_col_iterator(m_headers[col_index]->down);
}

template <typename T>
typename cross_list<T>::const_row_iterator cross_list<T>::cend_of_row(size_type row_index) const
{
	if (!is_legal_row(row_index))
		throw std::out_of_range("cross_list<T>::cend_of_row(size_type) const: row_index illegal");
	return const_row_iterator(m_headers[row_index]);
}

template <typename T>
typename cross_list<T>::const_col_iterator cross_list<T>::cend_of_col(size_type col_index) const
{
	if (!is_legal_col(col_index))
		throw std::out_of_range("cross_list<T>::cend_of_col(size_type) const: col_index illegal");
	return const_col_iterator(m_headers[col_index]);
}

template <typename T>
void cross_list<T>::fill_headers(typename headers_type::iterator first,
		typename headers_type::iterator last)
{
	for (; first!=last; ++first)
		*first = new_header();
}

template <typename T>
void cross_list<T>::empty_headers(typename headers_type::iterator first,
		typename headers_type::iterator last)
{
	for (; first!=last; ++first)
	{
		delete_node(*first);
		*first = NULL;
	}
}

template <typename T>
void cross_list<T>::reset_headers(typename headers_type::iterator first,
		typename headers_type::iterator last)
{
	for (; first!=last; ++first)
		reset_header(*first);
}

template <typename T> 
std::pair<typename cross_list<T>::node*, typename cross_list<T>::node*>
	cross_list<T>::locate(size_type row_index, size_type col_index) const
{
	std::pair<node*,node*> ptrs=
		std::make_pair(m_headers[row_index]->right,m_headers[col_index]->down);
	while (ptrs.first!=m_headers[row_index] && ptrs.first->col<col_index)
		ptrs.first = ptrs.first->right;
	while (ptrs.second!=m_headers[col_index] && ptrs.second->row<row_index)
		ptrs.second = ptrs.second->down;
	return ptrs;
}

template <typename T> 
std::pair<typename cross_list<T>::node*, typename cross_list<T>::node*>
	cross_list<T>::rlocate(size_type row_index, size_type col_index) const
{
	std::pair<node*,node*> ptrs=std::make_pair(m_headers[row_index]->left,m_headers[col_index]->up);
	while (ptrs.first!=m_headers[row_index] && ptrs.first->col>col_index)
		ptrs.first = ptrs.first->left;
	while (ptrs.second!=m_headers[col_index] && ptrs.second->row>row_index)
		ptrs.second = ptrs.second->up;
	return ptrs;
}

template <typename T>
void cross_list<T>::insert_node_before(node* new_node, const std::pair<node*,node*>& ptrs)
{
	attach_node_left_of(new_node,ptrs.first);
	attach_node_up_of(new_node,ptrs.second);
	increase_record(new_node);
}

template <typename T>
void cross_list<T>::insert_node_after(node* new_node, const std::pair<node*,node*>& ptrs)
{
	attach_node_right_of(new_node,ptrs.first);
	attach_node_down_of(new_node,ptrs.second);
	increase_record(new_node);
}

template <typename T>
void cross_list<T>::erase_node(node* p)
{
	decrease_record(p);
	detach_node(p);
	delete_node(p);
}

template <typename T> 
typename cross_list<T>::node* cross_list<T>::new_header()
{
	size_type no_val=std::numeric_limits<size_type>::max();//has_ptr_at() will use it
	node* ptr=new_node(m_dft_val,no_val,no_val);
	ptr->left = ptr->right = ptr->up = ptr->down = ptr;
	return ptr;
}

template <typename T> 
void cross_list<T>::reset_header(node* p)
{
	size_type no_val=std::numeric_limits<size_type>::max();//has_ptr_at() will use it
	p->left = p->right = p->up = p->down = p;
	p->row = p->col = no_val;
	p->value = m_dft_val;
}

template <typename T>
void cross_list<T>::attach_node_left_of(node* new_node, node* pos)
{
	new_node->left = pos->left;
	new_node->right = pos;
	new_node->left->right = new_node;
	new_node->right->left = new_node;
}

template <typename T>
void cross_list<T>::attach_node_right_of(node* new_node, node* pos)
{
	new_node->right = pos->right;
	new_node->left = pos;
	new_node->left->right = new_node;
	new_node->right->left = new_node;
}

template <typename T>
void cross_list<T>::attach_node_up_of(node* new_node, node* pos)
{
	new_node->up = pos->up;
	new_node->down = pos;
	new_node->up->down = new_node;
	new_node->down->up = new_node;
}

template <typename T>
void cross_list<T>::attach_node_down_of(node* new_node, node* pos)
{
	new_node->down = pos->down;
	new_node->up = pos;
	new_node->up->down = new_node;
	new_node->down->up = new_node;
}

template <typename T>
void cross_list<T>::detach_node(node* p)
{
	p->left->right = p->right;
	p->right->left = p->left;
	p->up->down = p->down;
	p->down->up = p->up;
}

template <typename T>
void cross_list<T>::increase_record(node* p)
{
	++m_r_rec[p->row];
	++m_c_rec[p->col];
	++m_size;
}

template <typename T>
void cross_list<T>::decrease_record(node* p)
{
	--m_r_rec[p->row];
	--m_c_rec[p->col];
	--m_size;
}

#endif //CROSS_LIST_H
