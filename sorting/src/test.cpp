// test.cpp
#include "sorting/sorting.h"

#include <cstdlib>
#include <exception>
#include <iostream>
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

int main(int argc, char* argv[]) {
    try {
        test_insertion_sort();
        test_selection_sort();
        test_shell_sort();
        test_heap_sort();
        test_merge_sort();
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
	
	for (int t=0; t<sizeof(lens)/sizeof(int); ++t) {
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
	
	for (int t=0; t<sizeof(lens)/sizeof(int); ++t) {
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
	
	for (int t=0; t<sizeof(lens)/sizeof(int); ++t) {
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
	
	for (int t=0; t<sizeof(lens)/sizeof(int); ++t) {
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
    int values[] = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
    vector<int> seq, buffer;
	int lens[] = {sizeof(values)/sizeof(int), 3, 2, 1, 0};
	
	for (int t=0; t<sizeof(lens)/sizeof(int); ++t) {
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
