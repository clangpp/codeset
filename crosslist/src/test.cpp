// test.cpp
#include "crosslist/crosslist.h"

#include <cstdlib>
#include <iostream>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_basic_operations();
void test_iterators();

int main() {
    test_basic_operations();
    test_iterators();
    system("pause");
}

void test_basic_operations() {
    Trace trace(INFO_, "test_basic_operations()");
    bool pass = true;

    CrossList<int> c;
    c.reserve(3, 4);
    pass = c.row_count()==3;
    log(INFO_) << "test row_count(): " << (pass ? "pass" : "FAILED") << endl;
    pass = c.column_count()==4;
    log(INFO_) << "test column_count(): " << (pass ? "pass" : "FAILED") << endl;
    pass = c.size()==0;
    log(INFO_) << "test size(): " << (pass ? "pass" : "FAILED") << endl;
    pass = c.empty();
    log(INFO_) << "test empty(): " << (pass ? "pass" : "FAILED") << endl;

    CrossList<int> c1(c);
    pass = (c1.row_count()==3) && (c1.column_count()==4) && c1.empty();
    log(INFO_) << "test copy constructor: " << (pass ? "pass" : "FAILED") << endl;
    pass = (c1==c);
    log(INFO_) << "test ==: " << (pass ? "pass" : "FAILED") << endl;
    pass = !(c1!=c);
    log(INFO_) << "test !=: " << (pass ? "pass" : "FAILED") << endl;

    CrossList<int> c2;
    c2 = c;
    pass = c2==c;
    log(INFO_) << "test copy assignment: " << (pass ? "pass" : "FAILED") << endl;

    pass = c.insert(1, 2, 5);
    pass &= (c.size()==1);
    log(INFO_) << "test insert(): " << (pass ? "pass" : "FAILED") << endl;
    pass = c.rinsert(2, 1, 3);
    pass &= c(2,1)==3;
    log(INFO_) << "test rinsert(): " << (pass ? "pass" : "FAILED") << endl;
    pass = (c.row_size(1)==1) && (c.row_size(0)==0);
    log(INFO_) << "test row_size(): " << (pass ? "pass" : "FAILED") << endl;
    pass = (c.column_size(2)==1) && (c.column_size(0)==0);
    log(INFO_) << "test column_size(): " << (pass ? "pass" : "FAILED") << endl;
    pass = (c.at(1, 2)==5);
    log(INFO_) << "test at(): " << (pass ? "pass" : "FAILED") << endl;
    pass = (c.rat(1, 2)==5);
    log(INFO_) << "test rat(): " << (pass ? "pass" : "FAILED") << endl;
    pass = (c(1, 2)==5);
    log(INFO_) << "test operator (): " << (pass ? "pass" : "FAILED") << endl;

    const CrossList<int>& rc(c);
    pass = (rc.at(1, 2)==5);
    try {
        rc.at(1, 3);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    log(INFO_) << "test at() const: " << (pass ? "pass" : "FAILED") << endl;
    pass = (rc.rat(1, 2)==5);
    try {
        rc.rat(1, 3);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    log(INFO_) << "test rat() const: " << (pass ? "pass" : "FAILED") << endl;
    pass = (rc(1, 2)==5);
    try {
        rc(1, 3);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    log(INFO_) << "test operator () const: " << (pass ? "pass" : "FAILED") << endl;

    c.transpose();
    pass = (c.row_count()==4) && (c.column_count()==3) &&
        (c.size()==2) && (c(2,1)==5) && (c(1,2)==3);
    log(INFO_) << "test transpose(): " << (pass ? "pass" : "FAILED") << endl;

    pass = c.erase(1u, 2u);
    pass = !c.erase(0, 0);
    log(INFO_) << "test erase(r,c): " << (pass ? "pass" : "FAILED") << endl;

    pass = c.rerase(2, 1);
    pass = !c.rerase(0, 0);
    log(INFO_) << "test rerase(r,c): " << (pass ? "pass" : "FAILED") << endl;

    c.set(1, 2, 5);
    pass = c(1,2)==5;
    log(INFO_) << "test set(r,c): " << (pass ? "pass" : "FAILED") << endl;

    c.rset(2, 1, 4);
    pass = c(2,1)==4;
    log(INFO_) << "test rset(r,c): " << (pass ? "pass" : "FAILED") << endl;

    pass = (rc.get(1,2)==5) && (rc.get(0,0)==0);
    log(INFO_) << "test get(r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    pass = (rc.rget(1,2)==5) && (rc.rget(0,0)==0);
    log(INFO_) << "test rget(r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    pass = rc.exist(1,2) && !rc.exist(0,0);
    log(INFO_) << "test exist(r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    pass = rc.rexist(1,2) && !rc.rexist(0,0);
    log(INFO_) << "test rexist(r,c) const: " << (pass ? "pass" : "FAILED") << endl;

    c.clear();
    pass = c.empty();
    log(INFO_) << "test clear(): " << (pass ? "pass" : "FAILED") << endl;

    c.row_reserve(5);
    pass = c.row_count()==5;
    log(INFO_) << "test row_reserve(): " << (pass ? "pass" : "FAILED") << endl;

    c.column_reserve(5);
    pass = c.column_count()==5;
    log(INFO_) << "test column_reserve(): " << (pass ? "pass" : "FAILED") << endl;
}

void test_iterators() {
    Trace trace(INFO_, "test_iterators()");
	bool pass = true;
    CrossList<int> c(3, 4);
    const CrossList<int>& r(c);

    c.insert(0, 0, 0);
    c.insert(0, 1, 1);
    c.insert(0, 2, 2);
    c.insert(0, 3, 3);
    c.insert(1, 0, 10);
    c.insert(1, 2, 12);
    c.insert(1, 3, 13);
    c.insert(2, 1, 21);
    c.insert(2, 2, 22);

    int entire_checks[] = {0, 1, 2, 3, 10, 12, 13, 21, 22};
    int row_checks[][4] = {{0, 1, 2, 3}, {10, 12, 13}, {21, 22}};
    int column_checks[][4] = {{0, 10}, {1, 21}, {2, 12, 22}, {3, 13}};

    pass = equal(c.begin(), c.end(), entire_checks);
    log(INFO_) << "test begin(), end(): " << (pass ? "pass" : "FAILED") << endl;

    pass = equal(r.begin(), r.end(), entire_checks);
    log(INFO_) << "test begin() const, end() const: " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<c.row_count(); ++i)
        pass &= equal(c.row_begin(i), c.row_end(i), row_checks[i]);
    log(INFO_) << "test row_begin(r), row_end(r): " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<r.row_count(); ++i)
        pass &= equal(r.row_begin(i), r.row_end(i), row_checks[i]);
    log(INFO_) << "test row_begin(r) const, row_end(r) const: " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<c.column_count(); ++i)
        pass &= equal(c.column_begin(i), c.column_end(i), column_checks[i]);
    log(INFO_) << "test column_begin(c), column_end(c): " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<r.column_count(); ++i)
        pass &= equal(r.column_begin(i), r.column_end(i), column_checks[i]);
    log(INFO_) << "test column_begin(c) const, column_end(c) const: " << (pass ? "pass" : "FAILED") << endl;

    int entire_rchecks[] = {22, 21, 13, 12, 10, 3, 2, 1, 0};
    int row_rchecks[][4] = {{3, 2, 1, 0}, {13, 12, 10}, {22, 21}};
    int column_rchecks[][4] = {{10, 0}, {21, 1}, {22, 12, 2}, {13, 3}};

    pass = equal(c.rbegin(), c.rend(), entire_rchecks);
    log(INFO_) << "test rbegin(), rend(): " << (pass ? "pass" : "FAILED") << endl;

    pass = equal(r.rbegin(), r.rend(), entire_rchecks);
    log(INFO_) << "test rbegin() const, rend() const: " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<c.row_count(); ++i)
        pass &= equal(c.row_rbegin(i), c.row_rend(i), row_rchecks[i]);
    log(INFO_) << "test row_rbegin(r), row_rend(r): " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<r.row_count(); ++i)
        pass &= equal(r.row_rbegin(i), r.row_rend(i), row_rchecks[i]);
    log(INFO_) << "test row_rbegin(r) const, row_rend(r) const: " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<c.column_count(); ++i)
        pass &= equal(c.column_rbegin(i), c.column_rend(i), column_rchecks[i]);
    log(INFO_) << "test column_rbegin(c), column_rend(c): " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    for (size_t i=0; i<r.column_count(); ++i)
        pass &= equal(r.column_rbegin(i), r.column_rend(i), column_rchecks[i]);
    log(INFO_) << "test column_rbegin(c) const, column_rend(c) const: " << (pass ? "pass" : "FAILED") << endl;

    CrossList<int>::iterator iter1, iter2;
    CrossList<int>::row_iterator row_iter1, row_iter2;
    CrossList<int>::column_iterator col_iter1, col_iter2;

    iter1 = c.begin();
    c.erase(iter1);
    pass = !c.exist(0,0);
    log(INFO_) << "test erase(iterator): " << (pass ? "pass" : "FAILED") << endl;

    row_iter1 = c.row_begin(0);
    c.erase(row_iter1);
    pass = !c.exist(0,1);
    log(INFO_) << "test erase(row_iterator): " << (pass ? "pass" : "FAILED") << endl;

    col_iter1 = c.column_begin(3);
    c.erase(col_iter1);
    pass = !c.exist(0,3);
    log(INFO_) << "test erase(column_iterator): " << (pass ? "pass" : "FAILED") << endl;

    row_iter1 = c.row_begin(1);
    row_iter2 = c.row_end(1);
    c.erase_range(row_iter1, row_iter2);
    pass = c.row_size(1)==0;
    log(INFO_) << "test erase_range(row_iterator, row_iterator): " << (pass ? "pass" : "FAILED") << endl;

    col_iter1 = c.column_begin(2);
    col_iter2 = c.column_end(2);
    c.erase_range(col_iter1, col_iter2);
    pass = c.column_size(2)==0;
    log(INFO_) << "test erase_range(column_iterator, column_iterator): " << (pass ? "pass" : "FAILED") << endl;

    iter1 = c.begin();
    iter2 = c.end();
    c.erase_range(iter1, iter2);
    pass = c.size()==0;
    log(INFO_) << "test erase_range(iterator, iterator): " << (pass ? "pass" : "FAILED") << endl;
}
