//main.cpp
#include <iostream>
#include <fstream>
#include "cross_list/cross_list.h"

using namespace std;

int main()
{
	try
	{
		cross_list<int> cl(10,11);
		cl.clear();
		cl(0,1) = 3;
		cl(0,2) = 4;
		cl(1,2) = 5;
		cl(1,1) = 7;
		cl.set(3,4,8);
		cl.rset(3,3,9);
		cl.insert(4,0,10);
		cl.rinsert(4,1,11);
		cl.erase(4u,0u);
		cl.rerase(4u,1u);
		cl.at(3,0) = 12;
		cl.rat(3,1) = 13;

		cout << endl << "cl(0,1) exist? " << boolalpha << cl.rexist(0,1) << endl;
		cout << endl << "cl(0,2) exist? " << boolalpha << cl.exist(0,2) << endl;

		cout << endl << "elements in cross list:" << endl;
		for (cross_list<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (cross_list<int>::size_type j=0; j<cl.col_size(); ++j)
				cout << cl.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl.size() << endl;

		cl.erase(cl.begin());

		cout << endl << "elements in cross list:" << endl;
		for (cross_list<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (cross_list<int>::size_type j=0; j<cl.col_size(); ++j)
				cout << cl.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl.size() << endl;

		cl.resize(3,4);

		cout << endl << "elements in cross list:" << endl;
		for (cross_list<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (cross_list<int>::size_type j=0; j<cl.col_size(); ++j)
				cout << cl.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl.size() << endl;

		cl.transpose();

		cout << endl << "elements in cross list:" << endl;
		for (cross_list<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (cross_list<int>::size_type j=0; j<cl.col_size(); ++j)
				cout << cl.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl.size() << endl;
		
		cross_list<int>::size_type row_index=0;
		cross_list<int>::const_row_iterator cri=cl.cbegin_of_row(row_index);

		cout << endl << "elements in row " << row_index << ":" << endl;
		while (cri!=cl.cend_of_row(row_index))
		{
			cout << "(" << cri.row_index() << "," << cri.col_index()
				<< "," << *cri << ")" << endl;
			++cri;
		}
		cout << "or (reverse):" << endl;
		cross_list<int>::const_reverse_row_iterator crri=cl.crbegin_of_row(row_index);
		while (crri!=cl.crend_of_row(row_index))
		{
			cout << "(" << crri.row_index() << "," << crri.col_index()
				<< "," << *crri << ")" << endl;
			++crri;
		}

		cross_list<int>::size_type col_index=2;
		cross_list<int>::const_col_iterator cci=cl.cbegin_of_col(col_index);

		cout << endl << "elements in column " << col_index << ":" << endl;
		while (cci!=cl.cend_of_col(col_index))
		{
			cout << "(" << cci.row_index() << "," << cci.col_index()
				<< "," << *cci << ")" << endl;
			++cci;
		}
		cout << "or (reverse):" << endl;
		cross_list<int>::const_reverse_col_iterator crci=cl.crbegin_of_col(col_index);
		while (crci!=cl.crend_of_col(col_index))
		{
			cout << "(" << crci.row_index() << "," << crci.col_index()
				<< "," << *crci << ")" << endl;
			++crci;
		}

		cout << endl << "elements in cross list (row iterator):" << endl;
		for (cross_list<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (cross_list<int>::row_iterator ci=cl.begin_of_row(i);
					ci!=cl.end_of_row(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (reverse row iterator):" << endl;
		for (cross_list<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (cross_list<int>::reverse_row_iterator ci=cl.rbegin_of_row(i);
					ci!=cl.rend_of_row(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (col iterator):" << endl;
		for (cross_list<int>::size_type i=0; i<cl.col_size(); ++i)
		{
			for (cross_list<int>::col_iterator ci=cl.begin_of_col(i);
					ci!=cl.end_of_col(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (reverse col iterator):" << endl;
		for (cross_list<int>::size_type i=0; i<cl.col_size(); ++i)
		{
			for (cross_list<int>::reverse_col_iterator ci=cl.rbegin_of_col(i);
					ci!=cl.rend_of_col(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (iterator):" << endl;
		for (cross_list<int>::iterator i=cl.begin();
			i!=cl.end(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl; 

		cout << endl << "elements in cross list (const iterator):" << endl;
		for (cross_list<int>::const_iterator i=cl.cbegin();
			i!=cl.cend(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl; 

		cout << endl << "elements in cross list (reverse iterator):" << endl;
		for (cross_list<int>::reverse_iterator i=cl.rbegin();
			i!=cl.rend(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl;

		cout << endl << "elements in cross list (const reverse iterator):" << endl;
		for (cross_list<int>::const_reverse_iterator i=cl.crbegin();
			i!=cl.crend(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl;

		const cross_list<int>& rcl=cl;
		//cout << rcl(5,8) << endl;

		cross_list<int> cl1(3,8);
		cout << endl << cl1.size() << " " << cl1.row_size() << " " << cl1.col_size() << endl;
		copy(cl.cbegin(),cl.cend(),ostream_iterator<int>(cout," "));
		cout << endl << "cl1==cl? " << boolalpha << (cl1==cl) << endl;

		cl1 = cl;
		cout << endl << cl1.size() << " " << cl1.row_size() << " " << cl1.col_size() << endl;
		copy(cl1.begin(),cl1.end(),ostream_iterator<int>(cout," "));
		bool equal=(cl1==cl);
		cout << endl << "cl1==cl? " << boolalpha << equal << endl;

		vector<cross_list<int> > cls(3,cl);
		cls[0](0,0)=1;
		cross_list<int> cl2 = cl;
		accumulate_to(cls.begin(),cls.end(),cl2);
		cout << endl << "elements in cross list:" << endl;
		for (cross_list<int>::size_type i=0; i<cl2.row_size(); ++i)
		{
			for (cross_list<int>::size_type j=0; j<cl2.col_size(); ++j)
				cout << cl2.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl2.size() << endl;

		ofstream fout("cross_list.txt");
		fout << cl2;
		fout.close();
		ifstream fin("cross_list.txt");
		cross_list<int> cl3;
		fin >> cl3;
		cout << cl3;

		cout << "const iterators:" << endl;
		{
			cross_list<int>::const_iterator it=cl3.begin(), ite=cl3.end();
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		{
			cross_list<int>::const_row_iterator it=cl3.begin_of_row(2), ite=cl3.end_of_row(2);
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		{
			cross_list<int>::const_col_iterator it=cl3.begin_of_col(1), ite=cl3.end_of_col(1);
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		cout << "const reverse iterators:" << endl;
		{
			cross_list<int>::const_reverse_iterator it=cl3.rbegin(), ite=cl3.rend();
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		{
			cross_list<int>::const_reverse_row_iterator it=cl3.rbegin_of_row(2), ite=cl3.rend_of_row(2);
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		{
			cross_list<int>::const_reverse_col_iterator it=cl3.rbegin_of_col(1), ite=cl3.rend_of_col(1);
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		cout << "reverse iterators:" << endl;
		{
			cross_list<int>::reverse_iterator it=cl3.rbegin(), ite=cl3.rend();
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		{
			cross_list<int>::reverse_row_iterator it=cl3.rbegin_of_row(2), ite=cl3.rend_of_row(2);
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		{
			cross_list<int>::reverse_col_iterator it=cl3.rbegin_of_col(1), ite=cl3.rend_of_col(1);
			copy(it,ite,ostream_iterator<int>(cout," "));
			cout << endl;
		}
		cross_list<int>::const_iterator it=cl3.cbegin(), it_end=cl3.cend();
		cross_list<int>::iterator it1=cl3.begin();
		cross_list<int>::const_reverse_iterator rit=cl3.rbegin(), rit2=cl3.rend();
		rit = rit2;
		cout << it1.pos_less(it_end) << endl;
		cout << it.pos_less(it_end) << endl;
	}
	catch (const exception& e)
	{
		cerr << "\nexception: " << e.what() << endl;
	}

	return 0;
}
