// test.cpp
#include "facility/facility.h"
#include <iostream>

using namespace std;
using namespace facility;

// test to<ToType>(FromType from) facility
void test_to();

// test trim facility
void test_trim();

// test array_length facility
void test_array_length();

int main() {

    // test to<ToType>(FromType from) facility
    test_to();

    // test trim facility
    test_trim();

    // test array_length facility
    test_array_length();

    return 0;
}

void test_to() {
	cout << "35" << " " << to<int>("35") << endl;
}

void test_trim() {
}

void test_array_length() {
}
