// test.cpp
#include "fft/fft.h"

#include <complex>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_filter();

int main(int argc, char* argv[]) {
    test_filter();
	system("pause");
    return 0;
}

void test_filter() {
    Trace trace(INFO_, "test_filter()");
    fft::Filter<double> f;
	vector<complex<double> > seq;
	seq.push_back(complex<double>(3));
	seq.push_back(complex<double>(3));
	seq.push_back(complex<double>(3));
	seq.push_back(complex<double>(3));
	seq.push_back(complex<double>(3));
	seq.push_back(complex<double>(3));
	seq.push_back(complex<double>(3));

	f.establish(seq.size());
	bool pass = true;
	pass = f.point_count()==8;
	log(INFO_) << "test establish: " << (pass ? "pass" : "FAILED") << endl;

	log(INFO_) << "test filter: " << endl;
	f.filter(seq);
	for (size_t i=0; i<seq.size(); ++i)
		log(INFO_) << "seq[" << i << "]=" << seq[i] << endl;
}
