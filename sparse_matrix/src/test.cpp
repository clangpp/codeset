//main.cpp
#include "sparse_matrix/sparse_matrix.h"

#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	try
	{
		sparse_matrix<int> cl(10,11),cl3;
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
		for (sparse_matrix<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (sparse_matrix<int>::size_type j=0; j<cl.col_size(); ++j)
				cout << cl.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl.size() << endl;

		cl.resize(3,4);
		cl.sparse(2,2);

		cout << endl << "elements in cross list:" << endl;
		for (sparse_matrix<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (sparse_matrix<int>::size_type j=0; j<cl.col_size(); ++j)
				cout << cl.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl.size() << endl;

		cl.transpose();

		cout << endl << "elements in cross list:" << endl;
		for (sparse_matrix<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (sparse_matrix<int>::size_type j=0; j<cl.col_size(); ++j)
				cout << cl.rget(i,j) << " ";
			cout << endl;
		}
		cout << cl.size() << endl;
		
		sparse_matrix<int>::size_type row_index=0;
		sparse_matrix<int>::const_row_iterator cri=cl.cbegin_of_row(row_index);

		cout << endl << "elements in row " << row_index << ":" << endl;
		while (cri!=cl.cend_of_row(row_index))
		{
			cout << "(" << cri.row_index() << "," << cri.col_index()
				<< "," << *cri << ")" << endl;
			++cri;
		}
		cout << "or (reverse):" << endl;
		sparse_matrix<int>::const_reverse_row_iterator crri=cl.crbegin_of_row(row_index);
		while (crri!=cl.crend_of_row(row_index))
		{
			cout << "(" << crri.row_index() << "," << crri.col_index()
				<< "," << *crri << ")" << endl;
			++crri;
		}

		sparse_matrix<int>::size_type col_index=2;
		sparse_matrix<int>::const_col_iterator cci=cl.cbegin_of_col(col_index);

		cout << endl << "elements in colume " << col_index << ":" << endl;
		while (cci!=cl.cend_of_col(col_index))
		{
			cout << "(" << cci.row_index() << "," << cci.col_index()
				<< "," << *cci << ")" << endl;
			++cci;
		}
		cout << "or (reverse):" << endl;
		sparse_matrix<int>::const_reverse_col_iterator crci=cl.crbegin_of_col(col_index);
		while (crci!=cl.crend_of_col(col_index))
		{
			cout << "(" << crci.row_index() << "," << crci.col_index()
				<< "," << *crci << ")" << endl;
			++crci;
		}

		cout << endl << "elements in cross list (row iterator):" << endl;
		for (sparse_matrix<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (sparse_matrix<int>::row_iterator ci=cl.begin_of_row(i);
					ci!=cl.end_of_row(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (reverse row iterator):" << endl;
		for (sparse_matrix<int>::size_type i=0; i<cl.row_size(); ++i)
		{
			for (sparse_matrix<int>::reverse_row_iterator ci=cl.rbegin_of_row(i);
					ci!=cl.rend_of_row(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (col iterator):" << endl;
		for (sparse_matrix<int>::size_type i=0; i<cl.col_size(); ++i)
		{
			for (sparse_matrix<int>::col_iterator ci=cl.begin_of_col(i);
					ci!=cl.end_of_col(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (reverse col iterator):" << endl;
		for (sparse_matrix<int>::size_type i=0; i<cl.col_size(); ++i)
		{
			for (sparse_matrix<int>::reverse_col_iterator ci=cl.rbegin_of_col(i);
					ci!=cl.rend_of_col(i); ci++)
				cout << "(" << ci.row_index() << "," << ci.col_index()
					<< "," << *ci << ")" << endl;
		}

		cout << endl << "elements in cross list (iterator):" << endl;
		for (sparse_matrix<int>::iterator i=cl.begin();
			i!=cl.end(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl; 

		cout << endl << "elements in cross list (const iterator):" << endl;
		for (sparse_matrix<int>::const_iterator i=cl.cbegin();
			i!=cl.cend(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl; 

		cout << endl << "elements in cross list (reverse iterator):" << endl;
		for (sparse_matrix<int>::reverse_iterator i=cl.rbegin();
			i!=cl.rend(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl;

		cout << endl << "elements in cross list (const reverse iterator):" << endl;
		for (sparse_matrix<int>::const_reverse_iterator i=cl.crbegin();
			i!=cl.crend(); ++i)
			cout << "(" << i.row_index() << "," << i.col_index()
				<< "," << *i << ")" << endl;


		sparse_matrix<int> cl1(3,8);
		cout << endl << cl1.size() << " " << cl1.row_size() << " " << cl1.col_size() << endl;
		copy(cl.cbegin(),cl.cend(),ostream_iterator<int>(cout," "));
		cout << endl << "cl1==cl? " << boolalpha << (cl1==cl) << endl;

		cl1 = cl;
		cout << endl << cl1.size() << " " << cl1.row_size() << " " << cl1.col_size() << endl;
		copy(cl1.begin(),cl1.end(),ostream_iterator<int>(cout," "));
		bool equal=(cl1==cl);
		cout << endl << "cl1==cl? " << boolalpha << equal << endl;

		const sparse_matrix<int>& rcl=cl;
		cl.iat(2,2)=5;
		cout << rcl.iat(2,2) << endl;
		cout << "(2,2) exist? " << rcl.exist(2,2) << " " 
			<< rcl.rexist(2,2) << " " << rcl.iexist(2,2) << endl;
		cout << rcl.sparse().first << " " << rcl.sparse().second << endl;

		ofstream fout("sparse_matrix.txt");
		fout << cl;
		fout.close();
		ifstream fin("sparse_matrix.txt");
		sparse_matrix<int> cl2;
		fin >> cl2;
		cout << cl2;
	}
	catch (const exception& e)
	{
		cerr << "\nexception: " << e.what() << endl;
	}

	return 0;
}
