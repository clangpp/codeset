// test.cpp
#include "bignumber/bignumber.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "facility/facility.h"
#include "logging/logging.h"

using namespace logging;
using namespace std;

template <typename ForwardIterator>
void to_digits(ForwardIterator first, ForwardIterator last) {
    for (; first!=last; ++first) {
        if ('0'<=*first && *first<='9') {
            *first -= '0';
        } else if ('a'<=*first && *first<='z') {
            *first = *first - 'a' + 10;
        } else if ('A'<=*first && *first<='Z') {
            *first = *first - 'A' + 10;
        }
    }
}

template <typename ForwardIterator>
void to_characters(ForwardIterator first, ForwardIterator last) {
    for (; first!=last; ++first) {
        if (0<=*first && *first<10) {
            *first += '0';
        } else if (10<=*first && *first<10+26) {
            *first = *first - 10 + 'A';
        }
    }
}

bool pass_multiplies(int num1, int num2);
void test_multiplies();

int main() {
    test_multiplies();
    system("pause");
    return 0;
}

bool pass_multiplies(int num1, int num2) {
    bool pass = true;
    string str_num1 = facility::to<string>(num1);
    string str_num2 = facility::to<string>(num2);
    int result = num1 * num2;
    string str_result_check = facility::to<string>(result);
    string str_result(str_num1.size()+str_num2.size(), '0');
    to_digits(str_num1.begin(), str_num1.end());
    to_digits(str_num2.begin(), str_num2.end());
    to_digits(str_result.begin(), str_result.end());
    string::reverse_iterator iter = bignumber::multiplies(
            str_num1.rbegin(), str_num1.rend(),
            str_num2.rbegin(), str_num2.rend(),
            10, str_result.rbegin());
    str_result.erase(str_result.begin(), iter.base());
    to_characters(str_result.begin(), str_result.end());
    pass = str_result == str_result_check;
    log(INFO_) << (pass ? "pass" : "FAILED") << " case: "
        << "num1=" << num1 << " num2=" << num2
        << " expected_result=" << result
        << " actural_result=" << str_result
        << endl;
    return pass;
}

void test_multiplies() {
    Trace trace(INFO_, "test_multiplies()");
    bool pass = true;
    pass &= pass_multiplies(0, 0);
    pass &= pass_multiplies(0, 1);
    pass &= pass_multiplies(1, 0);
    pass &= pass_multiplies(1234, 42);
    pass &= pass_multiplies(10054, 38722);
    log(INFO_) << "test multiplies: " << (pass ? "pass" : "FAILED") << endl;
}
