//test.cpp

#include "bm/bm.h"

#include <iostream>
#include <string>

using namespace std;

void test_BMIterator();
void test_bug_1();

int main()
{
    test_BMIterator();
    test_bug_1();
	return 0;
}

void test_BMIterator() {
	string s1("aa"), str("aabcdefghiaac");
    bm::Subsequence<char> subseq(s1.begin(), s1.end());
	bm::SubsequenceIterator<string::const_iterator>
        bm_iter(str.begin(), str.end(), subseq), bm_end;
	for (; bm_iter!=bm_end; ++bm_iter) {
		cout << bm_iter->first-str.begin() << " "
            << string(bm_iter->first, bm_iter->second) << endl;
    }
}

void test_bug_1() {
    // when s1 has only two same character, and str contains s1,
    // when run bm::count(str.begin(), str.end(), Subsequence<T>(s1.begin(), s1.end())),
    // bug occures: program die here.
    // reason: in bm::next(first, subseq), next position set to:
    //      first + subseq.good_suffix(0) - subseq.size()
    //  but this is wrong. first + subseq.good_suffix(0) donates new tail
    //  element to be checked firstly in [first, last), it should minus subseq.size()-1.
    //  so the correct expression is:
    //      first + subseq.good_suffix(0) + 1 - subseq.size()
    //  which make first+subseq.good_suffix(0)+1 donates the element after
    //  new tail element that to be checked in [first, last), then the 
    //  whole expression donates the position matches subseq[0].
    //  bug fixed.
    
    try {
        string s1("aa"), str("aabcdefghiaac");
        bm::Subsequence<char> subseq(s1.begin(), s1.end());
        size_t c = bm::count(str.begin(), str.end(), subseq);
        if (2 == c)
            cout << "test_bug_1(): pass" << endl;
        else
            cout << "test_bug_1(): not pass" << endl;
    } catch (const std::exception& e) {
        cout << "test_bug_1(): exception: " << e.what() << endl;
    }
}
