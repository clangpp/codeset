// test.cpp

#include "disjoint_set/disjoint_set.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <stdexcept>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_union_sets();
void test_find();
void test_disjoint_set();

int main() {
    test_union_sets();
    test_find();
    test_disjoint_set();
    system("pause");
}

void test_union_sets() {
    Trace trace(INFO_, "test_union_sets()");
    bool pass = true;
    vector<int> sets;

    sets.assign(8, -1);
    disjoint_set::union_sets_plain(sets.begin(), 4, 5);
    disjoint_set::union_sets_plain(sets.begin(), 6, 7);
    disjoint_set::union_sets_plain(sets.begin(), 4, 6);
    disjoint_set::union_sets_plain(sets.begin(), 3, 4);
    int check1[] = {-1, -1, -1, -1, 3, 4, 4, 6};
    pass = equal(sets.begin(), sets.end(), check1);
    log(INFO_) << "test union_sets_plain: " << (pass ? "pass" : "FAILED") << endl;

    sets.assign(8, -1);
    disjoint_set::union_sets_by_size(sets.begin(), 4, 5);
    disjoint_set::union_sets_by_size(sets.begin(), 6, 7);
    disjoint_set::union_sets_by_size(sets.begin(), 4, 6);
    disjoint_set::union_sets_by_size(sets.begin(), 3, 4);
    int check2[] = {-1, -1, -1, 4, -5, 4, 4, 6};
    pass = equal(sets.begin(), sets.end(), check2);
    log(INFO_) << "test union_sets_by_size: " << (pass ? "pass" : "FAILED") << endl;

    sets.assign(8, -1);
    disjoint_set::union_sets_by_height(sets.begin(), 4, 5);
    disjoint_set::union_sets_by_height(sets.begin(), 6, 7);
    disjoint_set::union_sets_by_height(sets.begin(), 4, 6);
    disjoint_set::union_sets_by_height(sets.begin(), 3, 4);
    int check3[] = {-1, -1, -1, 4, -3, 4, 4, 6};
    pass = equal(sets.begin(), sets.end(), check3);
    log(INFO_) << "test union_sets_by_height: " << (pass ? "pass" : "FAILED") << endl;

    const int N = 16;
    sets.assign(N, -1);
    for (int step=1; step<N; step*=2) {
        for (int i=0; i<N; i+=2*step)
            disjoint_set::union_sets_by_size(sets.begin(), i, i+step);
    }
    int check_worst[N] = {-16, 0, 0, 2, 0, 4, 4, 6, 0, 8, 8, 10, 8, 12, 12, 14};
    pass = equal(sets.begin(), sets.end(), check_worst);
    log(INFO_) << "test union_sets_by_size: " << (pass ? "pass" : "FAILED") << endl;
}

void test_find() {
    Trace trace(INFO_, "test_find()");
    bool pass = true;
    vector<int> sets;
    const int N = 16;
    int worst[N] = {-16, 0, 0, 2, 0, 4, 4, 6, 0, 8, 8, 10, 8, 12, 12, 14};

    sets.assign(worst, worst+N);
    pass = (0==disjoint_set::find_plain(sets.begin(), 14)) &&
        equal(sets.begin(), sets.end(), worst);  // tree not change
    log(INFO_) << "test find_plain: " << (pass ? "pass" : "FAILED") << endl;

    sets.assign(worst, worst+N);
    int check1[N] = {-16, 0, 0, 2, 0, 4, 4, 6, 0, 8, 8, 10, 0, 12, 0, 14};
    pass = (0==disjoint_set::find_compress_path(sets.begin(), 14)) &&
        equal(sets.begin(), sets.end(), check1);
    log(INFO_) << "test find_compress_path: " << (pass ? "pass" : "FAILED") << endl;
}

void test_disjoint_set() {
    Trace trace(INFO_, "test_disjoint_set()");
    bool pass = true;
    const int N = 8;
    DisjointSet ds(N);
    const DisjointSet& cds(ds);

    // test union_sets
    pass = true;
    try {
        ds.union_sets(-1, 0);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        ds.union_sets(0, -1);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        ds.union_sets(0, N);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        ds.union_sets(N, 0);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        ds.union_sets(-1, N);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        ds.union_sets(N, -1);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    log(INFO_) << "test DisjointSet::union_sets() edges: "
        << (pass ? "pass" : "FAILED") << endl;

    pass = ds.set_count()==DisjointSet::size_type(N);
    ds.union_sets(4, 5);
    pass &= cds.set_count()==N-1;
    log(INFO_) << "test DisjointSet::set_count(): " << (pass ? "pass" : "FAILED") << endl;

    pass = true;
    try {
        cds.set_size(-1);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        cds.set_size(N);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        cds.set_size(5);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    pass &= (cds.set_size(4)==2);
    log(INFO_) << "test DisjointSet::set_size(): " << (pass ? "pass" : "FAILED") << endl;

    pass = cds.size()==DisjointSet::size_type(N);
    ds.union_sets(6, 7);
    pass &= cds.size()==DisjointSet::size_type(N);
    log(INFO_) << "test DisjointSet::size(): " << (pass ? "pass" : "FAILED") << endl;

    ds.union_sets(4, 6);
    ds.union_sets(3, 4);
    pass = (ds.set_count()==4) && (ds.set_size(4)==5);
    try {
        ds.union_sets(3, 6);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    ds.union_sets(4, 4);
    pass &= (cds.set_count()==4);
    log(INFO_) << "test DisjointSet::union_sets(): " << (pass ? "pass" : "FAILED") << endl;

    // test find
    const int M = 16;
    ds = DisjointSet(M);
    for (int step=1; step<M; step*=2) {
        for (int i=0; i<M; i+=2*step)
            ds.union_sets(i, i+step);
    }
    pass = true;
    try {
        cds.find(-1);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        cds.find(M);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    pass &= (cds.find(14)==0);
    log(INFO_) << "test DisjointSet::find() const: " << (pass ? "pass" : "FAILED") << endl;

    try {
        ds.find(-1);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    try {
        ds.find(M);
        pass &= false;
    } catch (const exception& e) {
        pass &= true;
    }
    pass &= (ds.find(14)==0);
    log(INFO_) << "test DisjointSet::find(): " << (pass ? "pass" : "FAILED") << endl;
}
