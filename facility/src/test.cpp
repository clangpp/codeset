// test.cpp
#include "facility/facility.h"

#include <cstdlib>
#include <iostream>

#include "logging/logging.h"

using namespace std;
using namespace facility;
using namespace logging;

void test_to();
void test_trim();
void test_array_length();
void test_copy_string();
void test_split();

template <typename CharT, typename OStream>
OStream& write(OStream& os, const basic_string<CharT>& str);

int main() {
    test_to();
    test_trim();
    test_array_length();
    test_copy_string();
    test_split();

    system("pause");
    return 0;
}

void test_to() {
	cout << "35" << " " << to<int>("35") << endl;
}

void test_trim() {
}

void test_array_length() {
}

void test_copy_string() {
    Trace trace(INFO_, "test_copy_string()");
    string str0("ÎÒABCÅ_DEF"), str1, strb(str0);
	swap(strb[0], strb[1]);
	swap(strb[5], strb[6]);
    wstring wstr0, wstr1;
    wstr0.push_back((unsigned char)str0[0] | (str0[1]<<8));
    wstr0.append(str0.data()+2, str0.data()+5);
    wstr0.push_back((unsigned char)str0[5] | (str0[6]<<8));
    wstr0.append(str0.data()+7, str0.data()+10);

    bool ok = false;
    write(log(INFO_) << "str0: ", str0) << endl;
    write(log(INFO_) << "strb: ", strb) << endl;
    write(log(INFO_) << "wstr0: ", wstr0) << endl;

    log(INFO_) << "copy_little_endian(wstr0, str1)..." << endl;
    ok = copy_little_endian(wstr0, 2, str1) == str0;
    log(INFO_) << "str0=" << str0 << ", str1=" << str1 << endl;
    write(log(INFO_) << "str0: ", str0) << endl;
    write(log(INFO_) << "str1: ", str1) << endl;
    log(INFO_) << "=========" << (ok ? "pass" : "fail") << "=========" << endl;

    log(INFO_) << "copy_little_endian(str0, wstr1)..." << endl;
    ok = copy_little_endian(str0, 2, wstr1) == wstr0;
    write(log(INFO_) << "wstr0: ", wstr0) << endl;
    write(log(INFO_) << "wstr1: ", wstr1) << endl;
    log(INFO_) << "=========" << (ok ? "pass" : "fail") << "=========" << endl;

    log(INFO_) << "copy_big_endian(wstr0, str1)..." << endl;
    ok = copy_big_endian(wstr0, 2, str1) == strb;
    log(INFO_) << "strb=" << strb << ", str1=" << str1 << endl;
    write(log(INFO_) << "strb: ", strb) << endl;
    write(log(INFO_) << "str1: ", str1) << endl;
    log(INFO_) << "=========" << (ok ? "pass" : "fail") << "=========" << endl;

    log(INFO_) << "copy_big_endian(strb, wstr1)..." << endl;
    copy_big_endian(strb, 2, wstr1);
    write(log(INFO_) << "wstr0: ", wstr0) << endl;
    write(log(INFO_) << "wstr1: ", wstr1) << endl;
    ok = (wstr1[4]&0xff) == ((wstr0[4]>>8)&0xff);
    log(INFO_) << "---------" << (ok ? "pass" : "fail") << "---------" << endl;
    ok = ((wstr1[5]>>8)&0xff) == (wstr0[4]&0xff);
    log(INFO_) << "---------" << (ok ? "pass" : "fail") << "---------" << endl;
    ok = (wstr1[5]&0xff) == (wstr0[5]&0xff);
    log(INFO_) << "---------" << (ok ? "pass" : "fail") << "---------" << endl;
    wstr1[4] = wstr0[4];
    wstr1[5] = wstr0[5];
    ok = wstr1 == wstr0;
    log(INFO_) << "=========" << (ok ? "pass" : "fail") << "=========" << endl;
}

template <typename CharT, typename OStream>
OStream& write(OStream& os, const basic_string<CharT>& str) {
    const std::size_t W = sizeof(CharT);
    for (size_t i=0; i<str.size(); ++i) {
        for (size_t j=0; j<W; ++j) {
            os << (unsigned)(unsigned char)(str[i] >> (W-1-j)*8)
                << (W-1==j ? "" : ",");
        }
        os << (str.size()-1==i ? "" : " ");
    }
    return os;
}

void test_split() {
	Trace trace(INFO_, "test_split()");
	bool pass = true;
	string str;
	vector<string> parts;

	str = "abc def  ghi ";
	char* check[] = {"abc", "def", "", "ghi", ""};
	parts = split(str);
	pass = (parts.size()==5) && equal(parts.begin(), parts.end(), check);
	log(INFO_) << "test split: " << (pass ? "pass": "FAILED") << endl;
}
