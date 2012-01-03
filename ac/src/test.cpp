// test.cpp
#include "ac/ac.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_Automation();
void test_TrieMap();
void test_Iterator();
void test_Chinese();

int main(int argc, char* argv[]) {
	test_Automation();
	test_TrieMap();
	test_Iterator();
	test_Chinese();
	return 0;
}

void test_Automation() {
	string func_sig("test_Automation()");
	Trace trace(INFO_, func_sig);
	try {
		string str("abcdefghijklmnopqrstuvwxyz");
		string s1("abc"), s2("abcdf"), s3("mnopq"), s4("bac");
		cout << "str: " << str << endl;
		cout << "sub: " << s1 << " " << s2 << " " << s3 << " " << s4 << endl;

		ac::Automation<char, int> automation;
		// ac::SubsequenceSet<char> automation;  // run ok!
		cout << boolalpha;
		cout << "after initialize: empty? " << automation.empty()
			<< " size()=" << automation.size() << endl;
		ac::Automation<char, int>::node_pointer pnode;
		automation.insert(s1.begin(), s1.end(), true, pnode);
		automation.insert(s2.begin(), s2.end(), true, pnode);
		automation.insert(s3.begin(), s3.end(), true, pnode);
		automation.insert(s4.begin(), s4.end(), true, pnode);
		cout << "after insert: empty? " << automation.empty()
			<< " size()=" << automation.size() << endl;

		std::pair<string::iterator, string::iterator> match =
			ac::find_first_of(str.begin(), str.end(), automation);
		cout << "find_first_of(): " << match.first-str.begin()
									   << " " << match.second-match.first<< endl;
		cout << "search(): found? "
						   << ac::search(str.begin(), str.end(), automation) << endl;
		cout << "count(): "
			<< ac::count(str.begin(), str.end(), automation) << endl;

		automation.clear();
		cout << "after clear: empty? " << automation.empty()
			<< " size()=" << automation.size() << endl;

	} catch (const exception& e) {
		log(ERROR_) << "error occured: " << e.what() << endl;
	}
}

void test_TrieMap() {
	string func_sig("test_TrieMap()");
	Trace trace(INFO_, func_sig);
	try {
		string str("abcdefghijklmnopqrstuvwxyz");
		string s1("abc"), s2("abcdf"), s3("mnopq"), s4("ghi"), s5("xyz"), s6("opq"), s7("rst");
		cout << "str: " << str << endl;
		cout << "sub: " << s1 << " " << s2 << " " << s3 << " " << s4 << endl;

		ac::TrieMap<char, int> mp;
		ac::TrieMap<char, int>::const_iterator citer(mp.end());
		cout << (citer==mp.end() ? "equal pass": "equal not pass") << endl;
		cout << (!(citer!=mp.end()) ? "not-equal pass": "not-equal not pass") << endl;
		cout << (mp.end()==citer ? "equal pass": "equal not pass") << endl;
		cout << (!(mp.end()!=citer) ? "not-equal pass": "not-equal not pass") << endl;

		pair<ac::TrieMap<char,int>::iterator, bool> ret =
			mp.insert(s1.begin(), s1.end(), 1);
		string iter_key(ret.first.key().begin(), ret.first.key().end());
		cout << (iter_key == s1 ? "key pass" : "key not pass") << endl;
		cout << (ret.first.value() == 1 ? "value pass" : "value not pass") << endl;
		
		const ac::TrieMap<char,int>::const_iterator cciter = ret.first;
		iter_key.assign(cciter.key().begin(), cciter.key().end());
		cout << (iter_key == s1 ? "const key pass" : "const key not pass") << endl;
		cout << (cciter.value() == 1 ? "const value pass" : "const value not pass") << endl;

		mp.insert(make_pair(s2.begin(), s2.end()), 2);
		mp.insert(s3, 3);
		mp[s4] = 4;

		ac::TrieMap<char, int>::node_pointer np;
		mp.insert(s5.begin(), s5.end(), 5, np);
		mp.insert(s6, 6);
		mp.insert(s7, 7);

		const ac::TrieMap<char, int>& cmp(mp);

		cout << "after insert: " << s5 << " found? " << (mp.find(s5)!=mp.end()) << endl;
		cout << "after insert: " << s5 << " found? " << (cmp.count(s5)!=0) << endl;

		mp.erase(s5.begin(), s5.end());
		mp.erase(make_pair(s6.begin(), s6.end()));
		mp.erase(s7);
		cout << "after erase: " << s5 << " found? " << (cmp.find(s5)!=cmp.end()) << endl;
		cout << "after erase: " << s5 << " found? " << (cmp.count(s5)!=0) << endl;
		cout << "size()=" << mp.size() << endl;

		std::pair<string::iterator, string::iterator> match =
			ac::find_first_of(str.begin(), str.end(), mp);
		cout << "find_first_of(): " << match.first-str.begin()
			<< " " << match.second-match.first<< endl;
		cout << "search(): found? "
			<< ac::search(str.begin(), str.end(), mp) << endl;
		cout << "count(): "
			<< ac::count(str.begin(), str.end(), mp) << endl;

	} catch (const exception& e) {
		log(ERROR_) << "error occured: " << e.what() << endl;
	}
}

void test_Iterator() {
	Trace trace(INFO_, "test_Iterator()");
	try {
		string str("abcdefghijklmnopqrstuvwxyz");
		string s1("abc"), s2("abcdf"), s3("mnopq"), s4("ghi");
		cout << "str: " << str << endl;
		cout << "sub: " << s1 << " " << s2 << " " << s3 << " " << s4 << endl;

		ac::SubsequenceSet<char> subset;
		subset.insert(s1.begin(), s1.end());
		subset.insert(make_pair(s2.begin(), s2.end()));
		subset.insert(s3);
		subset[s4] = true;

		// subset.erase(s1);

		std::pair<string::iterator, string::iterator> match =
			ac::find_first_of(str.begin(), str.end(), subset);
		cout << "find_first_of(): " << match.first-str.begin()
			<< " " << match.second-match.first<< endl;
		cout << "search(): found? "
			<< ac::search(str.begin(), str.end(), subset) << endl;
		cout << "count(): "
			<< ac::count(str.begin(), str.end(), subset) << endl;

		ac::SubsequenceIterator<string::const_iterator>
			ac_iter(str.begin(), str.end(), subset), ac_end;
		for (; ac_iter!=ac_end; ++ac_iter) {
			cerr << string(ac_iter->first, ac_iter->second)
				<< " " << ++(ac_iter->mapped()) << endl;
		}
	} catch (const exception& e) {
		log(ERROR_) << "error occured: " << e.what() << endl;
	}
}

void test_Chinese() {
	Trace trace(INFO_, "test_Chinese()");
	try {
		string str;
		ifstream ftext("text.txt");
		getline(ftext, str);
		ifstream fdict("dict.txt");
		vector<string> dict;
		string s;
		ac::SubsequenceSet<char> subset;
		while (getline(fdict, s)) {
			log(INFO_) << s.size() << " " << s << endl;
			dict.push_back(s);
			subset.insert(s.rbegin(), s.rend());
		}
		for (size_t i=0; i<dict.size(); ++i) {
			bool found = (subset.find(dict[i].rbegin(), dict[i].rend())!=subset.end());
			log(INFO_) << dict[i] << " in subset? " << found << endl;
		}
		ac::SubsequenceIterator<string::reverse_iterator>
			ac_iter(str.rbegin(), str.rend(), subset), ac_end;
		for (; ac_iter!=ac_end; ++ac_iter) {
			log(INFO_) << "matched " << string(
					ac_iter->second.base(), ac_iter->first.base()) << endl;
		}
	} catch (const exception& e) {
		log(ERROR_) << "error occured: " << e.what() << endl;
	}
}
