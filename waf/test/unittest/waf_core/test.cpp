// main.cpp

#include "waf_core/waf_core.h"
#include "waf_facility/waf_facility.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_term_frequency();
void test_co_occurrence();
void test_mean_distance();
void test_total_distance();
void test_word_activation_force();
void test_affinity_or_mean();
void test_affinity_measure();

int main() {
    test_term_frequency();
    test_co_occurrence();
    test_mean_distance();
    test_total_distance();
    test_word_activation_force();
    test_affinity_or_mean();
    test_affinity_measure();
    return 0;
}

void test_term_frequency() {
    Trace trace(INFO_, "test_term_frequency()");

    stringstream ss;
    ss << "1 2 3 5 3 -1 5 2 5 3 5 4 5 -1";
    istream_iterator<int> is_iter(ss), is_end;

    waf::TermSet termset;
    termset.insert(1, "1");
    termset.insert(2, "2");
    termset.insert(3, "3");
    termset.insert(5, "5");

    waf::FreqVector freqvec;
    waf::term_frequency(is_iter, is_end, waf::care_in(termset), freqvec.begin());

    assert(1 == freqvec[1]);
    assert(2 == freqvec[2]);
    assert(3 == freqvec[3]);
    assert(5 == freqvec[5]);

    assert(0 == freqvec[0]);
    assert(0 == freqvec[4]);

    log(INFO_) << "pass" << endl;
}

void test_co_occurrence() {
    Trace trace(INFO_, "test_co_occurrence()");
    SparseMatrix<waf::cooccur_type> co_mat;

    stringstream ss;
    ss << "0 1 2 2 3 5 -1 0 3 1";
    istream_iterator<int> is_iter(ss), is_end;
    waf::co_occurrence(is_iter, is_end,
            waf::care_all(), waf::care_all(), 5, co_mat);
    waf::mean_distance(co_mat.begin(), co_mat.end());

    assert(abs(co_mat(0,2).first - 2) < 0.01);
    assert(abs(co_mat(0,1).first - 1.5) < 0.01);
    assert(abs(co_mat.iget(5,0).first - 0) < 0.01);

    log(INFO_) << "pass" << endl;
}

void test_mean_distance() {
    Trace trace(INFO_, "test_mean_distance()");
    vector<pair<double, int> > vec;
    vec.push_back(make_pair(8, 4));
    vec.push_back(make_pair(12, -24));
    vec.push_back(make_pair(0, 1));

    waf::mean_distance(vec.begin(), vec.end());

    assert(abs(vec[0].first - 2) < 0.01);
    assert(abs(vec[1].first + 0.5) < 0.01);
    assert(abs(vec[2].first - 0) < 0.01);

    log(INFO_) << "pass" << endl;
}

void test_total_distance() {
    Trace trace(INFO_, "test_total_distance()");
    vector<pair<double, int> > vec;
    vec.push_back(make_pair(2, 4));
    vec.push_back(make_pair(-0.5, -24));
    vec.push_back(make_pair(0, 1));

    waf::total_distance(vec.begin(), vec.end());

    assert(abs(vec[0].first - 8) < 0.01);
    assert(abs(vec[1].first - 12) < 0.01);
    assert(abs(vec[2].first - 0) < 0.01);

    log(INFO_) << "pass" << endl;
}

void test_word_activation_force() {
    Trace trace(INFO_, "test_word_activation_force()");

    waf::cooccur_type co(5, 2);
    waf::force_type f = waf::word_activation_force(co, 3, 5);
    assert(abs(f - 0.010667) < 0.00001);

    log(INFO_) << "pass" << endl;
}

void test_affinity_or_mean() {
    Trace trace(INFO_, "test_affinity_or_mean()");
    waf::TermSet termset;
    termset.insert(0, "t");
    termset.insert(1, "u");
    termset.insert(2, "v");
    termset.insert(3, "w");
    termset.insert(4, "x");
    termset.insert(5, "y");
    termset.insert(6, "z");

    CrossList<waf::force_type> waf_mat(
            termset.max_termid()+1, termset.max_termid()+1);
    waf_mat(termset["t"], termset["u"]) = 0.04;
    waf_mat(termset["w"], termset["u"]) = 0.05;
    waf_mat(termset["z"], termset["u"]) = 0.05;

    waf_mat(termset["u"], termset["x"]) = 0.03;
    waf_mat(termset["u"], termset["y"]) = 0.03;

    waf_mat(termset["t"], termset["v"]) = 0.05;
    waf_mat(termset["w"], termset["v"]) = 0.02;

    waf_mat(termset["v"], termset["x"]) = 0.06;
    waf_mat(termset["v"], termset["y"]) = 0.03;
    waf_mat(termset["v"], termset["z"]) = 0.04;

    waf::termid_type u_id = termset["u"], v_id = termset["v"];

    // in-link of "u" and "v"
    waf::force_type or_mean = waf::affinity_or_mean(
            waf_mat.column_begin(u_id), waf_mat.column_end(u_id), waf::care_all(),
            waf_mat.column_begin(v_id), waf_mat.column_end(v_id), waf::care_all(),
            mem_fun_ref(
                &CrossList<waf::force_type>::const_column_iterator::row)
            );
    assert(abs(or_mean - 0.4) < 0.01);

    // out-link of "u" and "v"
    or_mean = waf::affinity_or_mean(
            waf_mat.row_begin(u_id), waf_mat.row_end(u_id), waf::care_all(),
            waf_mat.row_begin(v_id), waf_mat.row_end(v_id), waf::care_all(),
            mem_fun_ref(
                &CrossList<waf::force_type>::const_row_iterator::column)
            );
    assert(abs(or_mean - 0.5) < 0.01);

    log(INFO_) << "pass" << endl;
}

void test_affinity_measure() {
    Trace trace(INFO_, "test_affinity_measure()");
    waf::TermSet termset;
    termset.insert(0, "t");
    termset.insert(1, "u");
    termset.insert(2, "v");
    termset.insert(3, "w");
    termset.insert(4, "x");
    termset.insert(5, "y");
    termset.insert(6, "z");

    CrossList<waf::force_type> waf_mat(
            termset.max_termid()+1, termset.max_termid()+1);
    waf_mat(termset["t"], termset["u"]) = 0.04;
    waf_mat(termset["w"], termset["u"]) = 0.05;
    waf_mat(termset["z"], termset["u"]) = 0.05;

    waf_mat(termset["u"], termset["x"]) = 0.03;
    waf_mat(termset["u"], termset["y"]) = 0.03;

    waf_mat(termset["t"], termset["v"]) = 0.05;
    waf_mat(termset["w"], termset["v"]) = 0.02;

    waf_mat(termset["v"], termset["x"]) = 0.06;
    waf_mat(termset["v"], termset["y"]) = 0.03;
    waf_mat(termset["v"], termset["z"]) = 0.04;

    waf::termid_type u_id = termset["u"], v_id = termset["v"];

    waf::affinity_type a = waf::affinity_measure(
            waf_mat, u_id, waf::care_all(), waf_mat, v_id, waf::care_all());
    assert(abs(a - 0.45) < 0.01);

    log(INFO_) << "pass" << endl;
}

