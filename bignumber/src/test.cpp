// test.cpp
#include "bignumber/bignumber.h"

#include <cstdlib>
#include <iostream>
#include <vector>

#include "logging/logging.h"

using namespace logging;
using namespace std;

void test_utility();
void test_multiplies();

int main() {
    test_utility();
    test_multiplies();

	system("pause");
    return 0;
}

void test_utility() {
    Trace trace(INFO_, "test_utility()");
    // int big_num[] = {1, 2, 3, 4, 5, 6};
    // int big_num_len = sizeof(big_num)/sizeof(int);
    // BigNumber bn(big_num, big_num_len);
}

void test_multiplies() {
    Trace trace(INFO_, "test_multiplies()");
    int num1[] = {1, 2, 3, 4, 5, 6};
    int num1_len = sizeof(num1)/sizeof(int);
    int num2[] = {4, 3, 2, 1, 0, 5};
    int num2_len = sizeof(num2)/sizeof(int);
    vector<int> result(num1_len+num2_len);
    vector<int>::iterator iter = bignumber::multiplies(
            num1, num1+num1_len, num2, num2+num2_len, 10, result.begin());
    result.erase(iter, result.end());
    int result_check[] = {4, 1, 1, 2, 3, 9, 7, 6, 9, 7, 2, 3};
    vector<int> vec_check(result_check,
            result_check+sizeof(result_check)/sizeof(int));
    bool pass = result == vec_check;
    log(INFO_) << "test multiplies: " << (pass ? "pass" : "FAILED") << endl;
}
