// test.cpp
#include "timing/timing.h"

#include <iomanip>
#include <iostream>

using namespace std;
using namespace timing;

int main(int argc, char* argv[]) {

	const int N=2000;
	timing::start();
	cout << setiosflags(ios::fixed) << setprecision(9);
	for (int i=0; i<N; ++i) {
		cout << i << " " << timing::duration() << endl;
	}
	cout << endl;
	timing::stop();
	cout << "total duration: " << timing::duration() << endl;
	cout << "period: " << timing::period() << endl;
	timing::clear();
	cout << "duration after clear: " << timing::duration() << endl;

	timing::start();
	for (int i=0; i<N; ++i) {
		cout << i << " " << timing::duration() << endl;
	}
    cout << "restart..." << endl;
    timing::restart();
	for (int i=0; i<N; ++i) {
		cout << i << " " << timing::duration() << endl;
	}
    timing::stop();
	cout << "total duration: " << timing::duration() << endl;
	timing::clear();
	cout << "duration after clear: " << timing::duration() << endl;
	return 0;
}
