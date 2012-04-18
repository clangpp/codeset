// main.cpp

#include "waf_facility/waf_facility.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_TermSet();
void test_FreqVector();
void test_CellComp();
void test_care_all();
void test_care_in();
void test_freq_dict();

int main() {
    test_TermSet();
    test_FreqVector();
    test_CellComp();
    test_care_all();
    test_care_in();
    test_freq_dict();
    return 0;
}

void test_TermSet() {
    Trace trace(INFO_, "test_TermSet()");
    waf::TermSet termset;

    stringstream ss;
    ss << " ( 0 aa ) \n ( 1 ab ) \n ( 2 c ) \n ( 4 e ) \n";
    string str = ss.str();
    ss >> termset;

    stringstream so;
    so << termset;
    assert(so.str() == str);

    assert(termset.size() == 4);

    assert("aa" == termset[0]);
    assert("ab" == termset[1]);
    assert("c" == termset[2]);
    assert("e" == termset[4]);

    assert(termset.search(3) == false);
    assert(termset.search(4) == true);
    assert(termset.search(5) == false);
    assert(termset.count(3) == 0);
    assert(termset.count(4) == 1);
    assert(termset.count(5) == 0);

    assert(0 == termset["aa"]);
    assert(1 == termset["ab"]);
    assert(2 == termset["c"]);
    assert(4 == termset["e"]);

    assert(termset.search("ab") == true);
    assert(termset.search("ac") == false);
    assert(termset.count("ab") == 1);
    assert(termset.count("ac") == 0);

    assert(termset.max_termid() == 4);

    assert(termset.insert(4, "ad") == false);
    assert(termset.search("ad") == false);

    assert(termset.insert("ab", 3) == false);
    assert(1 == termset.search("ab"));

    assert(termset.search(2) == true);
    assert(termset.search("c") == true);
    assert(termset.erase(3) == false);
    assert(termset.erase(2) == true);
    assert(termset.search(2) == false);
    assert(termset.search("c") == false);

    assert(termset.search("e") == true);
    assert(termset.search(4) == true);
    assert(termset.erase("f") == false);
    assert(termset.erase("e") == true);
    assert(termset.search("e") == false);
    assert(termset.search(4) == false);

    assert(termset.max_termid() == 1);

    assert(termset.size() == 2);
    termset.clear();
    assert(termset.size() == 0);

    log(INFO_) << "pass" << endl;
}

void test_FreqVector() {
    Trace trace(INFO_, "test_FreqVector()");
    waf::FreqVector freqvec;

    stringstream ss;
    ss << " ( 0 500 ) \n ( 1 100 ) \n ( 2 200 ) \n ( 4 400 ) \n";
    string str = ss.str();
    ss >> freqvec;
    
    stringstream so;
    so << freqvec;
    assert(so.str() == str);

    assert(500 == freqvec[0]);
    assert(100 == freqvec[1]);
    assert(200 == freqvec[2]);
    assert(0 == freqvec[3]);
    assert(400 == freqvec[4]);

    waf::FreqVector::const_iterator citer = freqvec.begin();
    assert(200 == *(citer+2));
    assert(200 == citer[2]);
    assert(0 == *(citer+3));
    assert(0 == *(citer+5));

    waf::FreqVector::iterator iter = freqvec.begin();
    assert(200 == *(iter+2));
    assert(200 == iter[2]);
    assert(0 == *(iter+3));
    assert(0 == *(iter+5));

    log(INFO_) << "pass" << endl;
}

void test_CellComp() {
    Trace trace(INFO_, "test_CellComp()");
    serialization::sparsematrix::Cell<int> c1(10,20,1000), c2(10,20,1000);

    assert(waf::cell_value_greater(c1,c2)==false);
    assert(waf::cell_value_greater(c2,c1)==false);

    c1.column = 19;
    assert(waf::cell_value_greater(c1,c2)==true);
    assert(waf::cell_value_greater(c2,c1)==false);

    c1.row = 9;
    assert(waf::cell_value_greater(c1,c2)==true);
    assert(waf::cell_value_greater(c2,c1)==false);

    c1.value = 1025;
    assert(waf::cell_value_greater(c1,c2)==true);
    assert(waf::cell_value_greater(c2,c1)==false);

    log(INFO_) << "pass" << endl;
}

void test_care_all() {
    Trace trace(INFO_, "test_care_all()");

    waf::Care care = waf::care_all();

    assert(care(1000) == true);
    assert(care(1) == true);
    assert(care(0) == true);

    log(INFO_) << "pass" << endl;
}

void test_care_in() {
    Trace trace(INFO_, "test_care_in()");
    waf::TermSet termset;
    
    termset.insert(0, "0");
    termset.insert(1, "1");
    termset.insert(3, "3");

    waf::Care care = waf::care_in(termset);
    assert(care(0) == true);
    assert(care(1) == true);
    assert(care(2) == false);
    assert(care(3) == true);
    assert(care(4) == false);

    int a[5] = {0, 1, 2, 3, 4};
    remove_if(a, a+5, waf::care_in(termset));
    assert(2 == a[0]);
    assert(4 == a[1]);

    log(INFO_) << "pass" << endl;
}

void test_freq_dict() {
    Trace trace(INFO_, "test_freq_dict()");
    waf::FreqVector freqvec;

    freqvec[0] = 1;
    freqvec[1] = 5;
    freqvec[3] = 11;

    assert(1 == waf::freq_dict(freqvec)(0));
    assert(5 == waf::freq_dict(freqvec)(1));
    assert(11 == waf::freq_dict(freqvec)(3));

    assert(0 == waf::freq_dict(freqvec)(2));
    assert(0 == waf::freq_dict(freqvec)(4));

    log(INFO_) << "pass" << endl;
}
