// test.cpp
#include "ac/ac.h"

#include <exception>
#include <iostream>
#include <string>

using namespace std;

void test_Automation();
void test_Iterator();

int main(int argc, char* argv[]) {
	test_Automation();
	test_Iterator();
	return 0;
}

void test_Automation() {
	cout << "test Automation(): " << endl;
	string str("abcdefghijklmnopqrstuvwxyz");
	string s1("abc"), s2("abcdf"), s3("mnopq"), s4("bac");
	cout << "str: " << str << endl;
	cout << "sub: " << s1 << " " << s2 << " " << s3 << " " << s4 << endl;

	ac::Automation<char> automation;
	// ac::SubsequenceSet<char> automation;  // run ok!
	cout << "after initialize: empty? " << automation.empty() << endl;
	automation.insert(s1.begin(), s1.end());
	automation.insert(make_pair(s2.begin(), s2.end()));
	automation.insert(s3);
	automation.insert(s4);
	cout << "after insert: empty? " << automation.empty() << endl;

	std::pair<string::iterator, string::iterator> match =
		ac::find_first_of(str.begin(), str.end(), automation);
	cout << "find_first_of(): " << match.first-str.begin()
		<< " " << match.second-match.first<< endl;
	cout << "search(): found? "
		<< ac::search(str.begin(), str.end(), automation) << endl;
	cout << "count(): "
		<< ac::count(str.begin(), str.end(), automation) << endl;

	automation.clear();
	cout << "after clear: empty? " << automation.empty() << endl;
}

void test_Iterator() {
	try {
		cout << "test Iterator(): " << endl;
		string str("abcdefghijklmnopqrstuvwxyz");
		string s1("abc"), s2("abcdf"), s3("mnopq"), s4("ghi");
		cout << "str: " << str << endl;
		cout << "sub: " << s1 << " " << s2 << " " << s3 << " " << s4 << endl;

		ac::SubsequenceSet<char, int> subset;
		subset.insert(s1, 1);
		subset.insert(s2, 2);
		subset.insert(s3, 3);
		subset[s4] = 4;

		// subset.erase(s1);

		std::pair<string::iterator, string::iterator> match =
			ac::find_first_of(str.begin(), str.end(), subset);
		cout << "find_first_of(): " << match.first-str.begin()
			<< " " << match.second-match.first<< endl;
		cout << "search(): found? "
			<< ac::search(str.begin(), str.end(), subset) << endl;
		cout << "count(): "
			<< ac::count(str.begin(), str.end(), subset) << endl;

		ac::SubsequenceIterator<string::const_iterator, int>
			ac_iter(str.begin(), str.end(), subset), ac_end;
		for (; ac_iter!=ac_end; ++ac_iter) {
			cerr << string(ac_iter->first, ac_iter->second)
				<< " " << ++(ac_iter->mapped()) << endl;
		}
	} catch (const exception& e) {
		cerr << "test_MappedIterator(): " << e.what() << endl;
	}
}
