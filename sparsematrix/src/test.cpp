// test.cpp
#include "sparsematrix/sparsematrix.h"

#include <iostream>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_basic_operations();
void test_matrix_sparse();

int main(int argc, char* argv[]) {
    try {
        test_basic_operations();
        test_matrix_sparse();
    } catch (const exception& e) {
        log(CRITICAL_) << "exception: " << e.what() << endl;
    }
    system("pause");
    return 0;
}

void test_basic_operations() {
    Trace trace(INFO_, "test_basic_operations()");
    bool pass = true;

    SparseMatrix<int> m1(10,11), m2(3), m3(2,2,-1);
    pass = true;
    log(INFO_) << "test constructor: " << (pass ? "pass" : "FAILED") << endl;
    m1(0,1) = 3;
    m1(0,2) = 4;
    m1(1,2) = 5;
    m1(1,1) = 7;

    pass = (3==m1(0,1)) && (4==m1(0,2)) && (5==m1(1,2)) && (7==m1(1,1));
    log(INFO_) << "test operator () (r,c): " << (pass ? "pass" : "FAILED") << endl;

    const SparseMatrix<int>& cm1(m1);
    pass = (3==cm1(0,1)) && (4==cm1(0,2)) && (5==cm1(1,2)) && (7==cm1(1,1));
    log(INFO_) << "test operator () (r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    SparseMatrix<int> m4(m1);
    pass = (m4.size()==4) && (3==m4(0,1)) && (4==m4(0,2)) && (5==m4(1,2)) && (7==m4(1,1));
    log(INFO_) << "test copy constructor: " << (pass ? "pass" : "FAILED") << endl;

    m2 = m1;
    pass = (m2.size()==4) && (3==m2(0,1)) && (4==m2(0,2)) && (5==m2(1,2)) && (7==m2(1,1));
    log(INFO_) << "test operator = (): " << (pass ? "pass" : "FAILED") << endl;

    pass = (m1==m2) && !(m1==m3);
    log(INFO_) << "test operator == (): " << (pass ? "pass" : "FAILED") << endl;

    m1.iset(3,4,8);
    pass = (8==cm1(3,4));
    log(INFO_) << "test iset(r,c,value): " << (pass ? "pass" : "FAILED") << endl;

    pass = m1.iget(3,4)==8 && m1.iget(3,5)==0;
    log(INFO_) << "test iget(r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    pass = m1.iexist(3,4) && !m1.iexist(3,5);
    log(INFO_) << "test iexist(r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    m1.iat(3,0) = 12;
    pass = (m1(3,0)==12);
    log(INFO_) << "test iat(r,c): " << (pass ? "pass" : "FAILED") << endl;

    pass = (cm1.iat(3,0)==12);
    try {
        cm1.iat(3,5);
        pass &= false;
    } catch (const exception&) {
        pass &= true;
    }
    log(INFO_) << "test iat(r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    pass = m1.ierase(3,4) && !m1.ierase(3,5);
    pass &= !m1.iexist(3,4);
    log(INFO_) << "test ierase(r,c): " << (pass ? "pass" : "FAILED") << endl;
}

void test_matrix_sparse() {
    Trace trace(INFO_, "test_matrix_sparse()");
    bool pass = true;

    const int R=10, C=12;
    SparseMatrix<int> m1(R,C);
    for (int i=0; i<R*C; ++i)
        m1(i/C, i%C) = i;
    m1.sparse(5, 6);
	pass = (m1.sparse().first==5) && (m1.sparse().second==6);
    log(INFO_) << "test sparse(): " << (pass ? "pass" : "FAILED") << endl;

    m1.transpose();
    pass = (m1.row_count()==(size_t)C) && (m1.column_count()==(size_t)R);
    for (int i=0; i<R*C; ++i)
        pass &= (m1(i%C, i/C)==i);
	pass &= (m1.sparse().first==6) && (m1.sparse().second==5);
    log(INFO_) << "test transpose(): " << (pass ? "pass" : "FAILED") << endl;
}
