// test.cpp
#include "streammatrix/streammatrix.h"

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>

#include "logging/logging.h"

using namespace logging;
using namespace std;

void test_initialize(const string& mat_file);
void test_iterator(const string& mat_file);
void test_row_iterator(const string& mat_file);

int main() {
    string mat_file = "matrix.mat";
    test_initialize(mat_file);
    test_iterator(mat_file);
    test_row_iterator(mat_file);
    system("pause");
    return 0;
}

const int M=10, N=8;
int MAT[M][N] = {
    {10, 0, 0, 20, 30, 40, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {50, 0, 60, 70, 0, 0, 80, 0},
    {0, 90, 100, 0, 110, 0, 120, 130},
    {150, 160, 170, 180, 190, 200, 210, 220},
    {0, 0, 230, 0, 240, 250, 260, 0},
    {0, 270, 280, 290, 0, 0, 0, 0},
    {300, 0, 310, 0, 320, 0, 330, 0},
    {340, 0, 350, 0, 360, 0, 370, 0},
    {0, 0, 0, 380, 390, 0, 0, 400}
};

void test_initialize(const string& mat_file) {
    Trace trace(INFO_, "test_initialize()");
    bool pass=true;
    CrossList<int> mat(M,N);
    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            if (MAT[i][j]!=0)
                mat.insert(i, j, MAT[i][j]);
        }
    }
    log(INFO_) << "mat is:\n" << mat << endl;
    ofstream fout(mat_file.c_str());
    fout << mat;
    fout.close();
    
    ifstream fin(mat_file.c_str());
    StreamMatrix<int> smat(fin);
	pass = (smat.row_count()==M) && (smat.column_count()==N) && (smat.size()==39);
    log(INFO_) << "test initialize: " << (pass ? "pass" : "FAILED") << endl;

    pass = !smat.empty();
    log(INFO_) << "test empty: " << (pass ? "pass" : "FAILED") << endl;
}

void test_iterator(const string& mat_file) {
    Trace trace(INFO_, "test_iterator()");
    bool pass = true;
    ifstream fin(mat_file.c_str());
    StreamMatrix<int> smat(fin);
    vector<int> values;
    StreamMatrix<int>::iterator iter, end=smat.end();
    for (iter=smat.begin(); iter!=end; ++iter) {
        values.push_back(*iter);
    }
	vector<int> values_check;
    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            if (MAT[i][j]!=0)
                values_check.push_back(MAT[i][j]);
        }
    }
    pass = values == values_check;
    log(INFO_) << "test iterate: " << (pass ? "pass" : "FAILED") << endl;
}

void test_row_iterator(const string& mat_file) {
    Trace trace(INFO_, "test_row_iterator()");
    bool pass = true;
    ifstream fin(mat_file.c_str());
    StreamMatrix<int> smat(fin);
    for (int i=0; i<M; ++i) {
		pass = true;
        vector<int> values;
        StreamMatrix<int>::row_iterator iter, end=smat.row_end(i);
        for (iter=smat.row_begin(i); iter!=end; ++iter) {
            values.push_back(*iter);
            pass &= (iter.row()==i);
        }

        vector<int> values_check;
        for (int j=0; j<N; ++j) {
            if (MAT[i][j])
                values_check.push_back(MAT[i][j]);
        }
        pass &= (values==values_check);
        log(INFO_) << "test iterate row " << i
            << ": " << (pass ? "pass" : "FAILED") << endl;
    }
}
