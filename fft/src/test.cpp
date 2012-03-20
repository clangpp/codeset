// test.cpp
#include "fft/fft.h"

#include <complex>
#include <cstdlib>
#include <deque>
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
    fft::Filter<long double> f;
	deque<complex<long double> > seq;
	seq.push_back(complex<long double>(1));
	seq.push_back(complex<long double>(0));
	seq.push_back(complex<long double>(-1));
	seq.push_back(complex<long double>(0));
	seq.push_back(complex<long double>(1));
	seq.push_back(complex<long double>(0));
	seq.push_back(complex<long double>(-1));
	seq.push_back(complex<long double>(0));
    deque<complex<long double> > seqb(seq);

	f.establish(seq.size());
	bool pass = true;
	pass = f.point_count()==8;
	log(INFO_) << "test establish: " << (pass ? "pass" : "FAILED") << endl;

	log(INFO_) << "test filter: " << endl;
	f.filter(seq);
	for (size_t i=0; i<seq.size(); ++i)
		log(INFO_) << "seq[" << i << "]=" << seq[i] << endl;
	long double seq1[] = {0,0,4,0,0,0,4,0};
	deque<complex<long double> > seq2(
            seq1, seq1+sizeof(seq1)/sizeof(long double));
	pass = seq == seq2;
	log(INFO_) << "test filter: " << (pass ? "pass" : "FAILED") << endl;

    pass = f(seqb)==seq2;
	log(INFO_) << "test operator (): " << (pass ? "pass" : "FAILED") << endl;
}
