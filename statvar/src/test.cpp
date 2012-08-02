// test.cpp
#include "statvar/statvar.h"

#include <cstdlib>
#include <iostream>

using namespace std;

void test_common();
void test_histogram();

int main() {
	StatVar<double> var1("var1"), var2;

	cout << var1 << endl;

	var1.record(2.0);
	var1 += 1.0;

	var2.name = "var2";
	var2 += 1.3;
	var1 += var2;
	
	cout << var1 << endl;
	cout << var2 << endl;

	system("pause");
	return 0;
}