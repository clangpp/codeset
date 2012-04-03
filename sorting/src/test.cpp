// test.cpp
#include "sorting/sorting.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_insertion_sort();
void test_selection_sort();
void test_shell_sort();
void test_heap_sort();
void test_merge_sort();
void test_quick_sort();
void test_select();
void test_indirect_sort();
void test_bucket_sort();

int main(int argc, char* argv[]) {
    try {
        test_insertion_sort();
        test_selection_sort();
        test_shell_sort();
        test_heap_sort();
        test_merge_sort();
        test_quick_sort();
        test_select();
        test_indirect_sort();
        test_bucket_sort();
    } catch (const exception& e) {
        log(CRITICAL_) << "exception: " << e.what() << endl;
    }
    system("pause");
    return 0;
}

void test_insertion_sort() {
    Trace trace(INFO_, "test_insertion_sort()");
    bool pass = true;
    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    list<int> seq;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before sorting: ";
		for (list<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		sorting::insertion_sort(seq.begin(), seq.end());
		log(INFO_) << "after sorting: ";
		for (list<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		int values_check[][sizeof(values)/sizeof(int)] = {
			{-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
		pass = equal(seq.begin(), seq.end(), values_check[t]);
		log(INFO_) << "test insertion_sort: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_selection_sort() {
    Trace trace(INFO_, "test_selection_sort()");
    bool pass = true;
    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    list<int> seq;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before sorting: ";
		for (list<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		sorting::selection_sort(seq.begin(), seq.end());
		log(INFO_) << "after sorting: ";
		for (list<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		int values_check[][sizeof(values)/sizeof(int)] = {
			{-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
		pass = equal(seq.begin(), seq.end(), values_check[t]);
		log(INFO_) << "test selection_sort: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_shell_sort() {
    Trace trace(INFO_, "test_shell_sort()");
    bool pass = true;
    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    vector<int> seq;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		sorting::shell_sort(seq.begin(), seq.end());
		log(INFO_) << "after sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		int values_check[][sizeof(values)/sizeof(int)] = {
			{-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
		pass = equal(seq.begin(), seq.end(), values_check[t]);
		log(INFO_) << "test shell_sort: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_heap_sort() {
    Trace trace(INFO_, "test_heap_sort()");
    bool pass = true;
    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    vector<int> seq;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		sorting::heap_sort(seq.begin(), seq.end());
		log(INFO_) << "after sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		int values_check[][sizeof(values)/sizeof(int)] = {
			{-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
		pass = equal(seq.begin(), seq.end(), values_check[t]);
		log(INFO_) << "test heap_sort: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_merge_sort() {
    Trace trace(INFO_, "test_merge_sort()");
    bool pass = true;

    int mvalues1[] = {1, 3, 5, 8, 12, 29}, len1=sizeof(mvalues1)/sizeof(int);
    int mvalues2[] = {4, 7, 10, 18, 25, 32}, len2=sizeof(mvalues2)/sizeof(int);
    int mvalues3[] = {2}, len3=sizeof(mvalues3)/sizeof(int);
    int mvalues4[1] = {}, len4=0;
    int mvalues5[1] = {}, len5=0;
    int mvalues12_check[] = {1, 3, 4, 5, 7, 8, 10, 12, 18, 25, 29, 32};
    int len12=sizeof(mvalues12_check)/sizeof(int);
    int mvalues34_check[] = {2}, len34=sizeof(mvalues34_check)/sizeof(int);
    int mvalues45_check[1] = {}, len45=0;
    int mvalues13_check[] = {1, 2, 3, 5, 8, 12, 29}, len13=sizeof(mvalues13_check)/sizeof(int);
    int mvalues24_check[] = {4, 7, 10, 18, 25, 32}, len24=sizeof(mvalues24_check)/sizeof(int);
    vector<int> check;
    vector<int>::iterator iter_check;

    check.resize(len12);
    iter_check = sorting::merge(mvalues1, mvalues1+len1,
            mvalues2, mvalues2+len2, check.begin());
    pass = (iter_check==check.end()) &&
        equal(check.begin(), check.end(), mvalues12_check);
    log(INFO_) << "test merge: " << (pass ? "pass": "FAILED") << endl;

    check.resize(len34);
    iter_check = sorting::merge(mvalues3, mvalues3+len3,
            mvalues4, mvalues4+len4, check.begin());
    pass = (iter_check==check.end()) &&
        equal(check.begin(), check.end(), mvalues34_check);
    log(INFO_) << "test merge: " << (pass ? "pass": "FAILED") << endl;

    check.resize(len45);
    iter_check = sorting::merge(mvalues4, mvalues4+len4,
            mvalues5, mvalues5+len5, check.begin());
    pass = (iter_check==check.end()) &&
        equal(check.begin(), check.end(), mvalues45_check);
    log(INFO_) << "test merge: " << (pass ? "pass": "FAILED") << endl;

    check.resize(len13);
    iter_check = sorting::merge(mvalues1, mvalues1+len1,
            mvalues3, mvalues3+len3, check.begin());
    pass = (iter_check==check.end()) &&
        equal(check.begin(), check.end(), mvalues13_check);
    log(INFO_) << "test merge: " << (pass ? "pass": "FAILED") << endl;

    check.resize(len24);
    iter_check = sorting::merge(mvalues2, mvalues2+len2,
            mvalues4, mvalues4+len4, check.begin());
    pass = (iter_check==check.end()) &&
        equal(check.begin(), check.end(), mvalues24_check);
    log(INFO_) << "test merge: " << (pass ? "pass": "FAILED") << endl;

    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    vector<int> seq, buffer;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
        buffer.resize(seq.size());
		log(INFO_) << "before sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		sorting::merge_sort(seq.begin(), seq.end(), buffer.begin());
		log(INFO_) << "after sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		int values_check[][sizeof(values)/sizeof(int)] = {
			{-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
		pass = equal(seq.begin(), seq.end(), values_check[t]);
		log(INFO_) << "test merge_sort: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_quick_sort() {
    Trace trace(INFO_, "test_quick_sort()");
    bool pass = true;

    int parts[][10] = { {}, {1}, {1, -1}, {1, -1, 25, -2, -8, 8, 22},
        {25, 25, 25, 25, 25}, {-8, -8, -8, -8, -8} };
    int parts_lens[sizeof(parts)/sizeof(parts[0])] = {0, 1, 2, 7, 5, 5};
    int checks[][10] = { {}, {1}, {-1, 1}, {-8, -1, -2, 25, 1, 8, 22},
        {25, 25, 25, 25, 25}, {-8, -8, -8, -8, -8} };
    int check_poss[sizeof(checks)/sizeof(checks[0])] = {0, 0, 1, 3, 0, 5};
	list<int> lst;

    for (size_t t=0; t<sizeof(parts_lens)/sizeof(int); ++t) {
		lst.assign(parts[t], parts[t]+parts_lens[t]);
        list<int>::iterator iter = sorting::partition(
			lst.begin(), lst.end(), bind2nd(less<int>(), 0));
        pass = (distance(lst.begin(),iter)==check_poss[t]) &&
            equal(lst.begin(), lst.end(), checks[t]);
		log(INFO_) << "test partition: " << (pass ? "pass": "FAILED") << endl;
    }

    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    vector<int> seq;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		sorting::quick_sort(seq.begin(), seq.end());
		log(INFO_) << "after sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		int values_check[][sizeof(values)/sizeof(int)] = {
			{-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
		pass = equal(seq.begin(), seq.end(), values_check[t]);
		log(INFO_) << "test quick_sort: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_select() {
    Trace trace(INFO_, "test_select()");
    bool pass = true;

    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
    int poss[] = {11, 1, 1, -1, 0};
    bool poss_legal[] = {true, true, true, false, false};
    int values_check[] = {20, 3, 4, 1, 0};
    vector<int> seq;
	
    // test quick_select
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before selecting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		vector<int>::iterator iter =
            sorting::quick_select(seq.begin(), seq.end(), poss[t]);
		log(INFO_) << "after selecting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
        pass = (iter!=seq.end())==poss_legal[t];
        if (iter!=seq.end())
            pass &= (*iter==values_check[t]);
		log(INFO_) << "test quick_select: " << (pass ? "pass": "FAILED") << endl;
	}
	
    // test heap_select
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before selecting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		vector<int>::iterator iter =
            sorting::heap_select(seq.begin(), seq.end(), poss[t]);
		log(INFO_) << "after selecting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
        pass = (iter!=seq.end())==poss_legal[t];
        if (iter!=seq.end())
            pass &= (*iter==values_check[t]);
		log(INFO_) << "test heap_select: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_indirect_sort() {
    Trace trace(INFO_, "test_indirect_sort()");
    bool pass = true;

    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    vector<int> seq;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (size_t t=0; t<sizeof(lens)/sizeof(int); ++t) {
		seq.assign(values, values+lens[t]);
		log(INFO_) << "before sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		sorting::indirect_sort(seq.begin(), seq.end());
		log(INFO_) << "after sorting: ";
		for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
			standard_logger() << *iter << " ";
		standard_logger() << endl;
		int values_check[][sizeof(values)/sizeof(int)] = {
			{-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
		pass = equal(seq.begin(), seq.end(), values_check[t]);
		log(INFO_) << "test indirect_sort: " << (pass ? "pass": "FAILED") << endl;
	}
}

void test_bucket_sort() {
    Trace trace(INFO_, "test_bucket_sort()");
    bool pass = true;
    vector<int> seq;

    int values[] = {3, 4, 1, 2, 8, 7, 10, 15, 12, 20, 0x7fffffff, -3};
    int len = sizeof(values)/sizeof(int);
	int lens[] = {len, len-1, len-2, 3, 2, 1, 0};
    int values_check[][sizeof(values)/sizeof(int)] = { {}, {},
        {1, 2, 3, 4, 7, 8, 10, 12, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};

    for (int t=0; t<2; ++t) {
		seq.assign(values, values+lens[t]);
        try {
            sorting::bucket_sort(values, values+lens[t]);
            pass = false;
        } catch (const exception&) {
            pass = true;
        }
        log(INFO_) << "test bucket_sort: " << (pass ? "pass" : "FAILED") << endl;
    }
	
	for (size_t t=2; t<sizeof(lens)/sizeof(int); ++t) {
        for (int ascending=0; ascending<2; ++ascending) {
            seq.assign(values, values+lens[t]);
            log(INFO_) << "before sorting: ";
            for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
                standard_logger() << *iter << " ";
            standard_logger() << endl;
            sorting::bucket_sort(seq.begin(), seq.end(), ascending);
            log(INFO_) << "after sorting: ";
            for (vector<int>::iterator iter=seq.begin(); iter!=seq.end(); ++iter)
                standard_logger() << *iter << " ";
            standard_logger() << endl;
            if (!ascending) reverse(seq.begin(), seq.end());
            pass = equal(seq.begin(), seq.end(), values_check[t]);
            log(INFO_) << "test bucket_sort: " << (pass ? "pass": "FAILED") << endl;
        }
	}
}
