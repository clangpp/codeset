// test.cpp
#include "graph/graph.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_topological_sort();
void test_dijkstra();
void test_utility();

int main() {
	try {
        // logging::level() = DEBUG_;
		test_topological_sort();
		test_dijkstra();
        test_utility();
	} catch (const exception& e) {
		log(CRITICAL_) << e.what() << endl;
	}
    system("pause");
}

void test_topological_sort() {
    Trace trace(INFO_, "test_topological_sort()");
    bool pass = true;

    CrossList<bool> g(7, 7);
    g.rinsert(0, 1, true);
    g.rinsert(0, 2, true);
    g.rinsert(0, 3, true);
    g.rinsert(1, 3, true);
    g.rinsert(1, 4, true);
    g.rinsert(2, 5, true);
    g.rinsert(3, 2, true);
    g.rinsert(3, 5, true);
    g.rinsert(3, 6, true);
    g.rinsert(4, 3, true);
    g.rinsert(4, 6, true);
    g.rinsert(6, 5, true);

    vector<graph::vertex_type> seq(7);
    vector<graph::vertex_type>::iterator iter =
        digraph::topological_sort(g, seq.begin());
    size_t topo_check[] = {0, 1, 4, 3, 2, 6, 5};
    pass = (iter==seq.end()) && equal(seq.begin(), iter, topo_check);
    pass &= g.empty();
    copy(seq.begin(), seq.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test topological_sort: " << (pass ? "pass": "FAILED") << endl;

    g.rinsert(3, 4, true);
    g.rinsert(4, 5, true);
    g.rinsert(5, 3, true);
    try {
        digraph::topological_sort(g, seq.begin());
        pass = false;
    } catch (const exception&) {
        pass = true;
    }
    log(INFO_) << "test topological_sort boundry: " << (pass ? "pass": "FAILED") << endl;
}

void test_dijkstra() {
    Trace trace(INFO_, "test_dijkstra()");
    bool pass = true;

    CrossList<int> g;
    vector<graph::vertex_type> prevs;
    vector<int> dists;
    int n = 0;

    // case 1: on book
    n = 7;
    g.clear();
    g.reserve(n, n);
    prevs.assign(n, 0);
    dists.assign(n, 0);
    g.rinsert(0, 1, 2);
    g.rinsert(0, 3, 1);
    g.rinsert(1, 3, 3);
    g.rinsert(1, 4, 10);
    g.rinsert(2, 0, 4);
    g.rinsert(2, 5, 5);
    g.rinsert(3, 2, 2);
    g.rinsert(3, 4, 2);
    g.rinsert(3, 5, 8);
    g.rinsert(3, 6, 4);
    g.rinsert(4, 6, 6);
    g.rinsert(6, 5, 1);
    digraph::dijkstra_shortest(g, 0, prevs.begin(), dists.begin());
    graph::vertex_type prevs_check[] = {0, 0, 3, 0, 3, 6, 3};
    int dists_check[] = {0, 2, 3, 1, 3, 6, 5};
    pass = equal(prevs.begin(), prevs.end(), prevs_check) &&
        equal(dists.begin(), dists.end(), dists_check);
    copy(prevs.begin(), prevs.end(), log(DEBUG_)(" "));
    log() << endl;
    copy(dists.begin(), dists.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test dijkstra_shortest: " << (pass ? "pass": "FAILED") << endl;

    // case 2: my own case
    n = 5;
    g.clear();
    g.reserve(n, n);
    prevs.assign(n, 0);
    dists.assign(n, 0);
    g.rinsert(0, 1, 10);
    g.rinsert(0, 2, 5);
    g.rinsert(1, 3, 9);
    g.rinsert(2, 3, 13);
    g.rinsert(2, 4, 1);
    g.rinsert(4, 1, 2);
    digraph::dijkstra_shortest(g, 0, prevs.begin(), dists.begin());
    graph::vertex_type prevs_check1[] = {0, 4, 0, 1, 2};
    int dists_check1[] = {0, 8, 5, 17, 6};
    pass = equal(prevs.begin(), prevs.end(), prevs_check1) &&
        equal(dists.begin(), dists.end(), dists_check1);
    copy(prevs.begin(), prevs.end(), log(DEBUG_)(" "));
    log() << endl;
    copy(dists.begin(), dists.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test dijkstra_shortest: " << (pass ? "pass": "FAILED") << endl;

    // case 3: longest dijkstra
    n = 14;
    g.clear();
    g.reserve(n, n);
    prevs.assign(n, 0);
    dists.assign(n, 0);
    g.rinsert(0, 1, 3);
    g.rinsert(0, 2, 2);
    g.rinsert(1, 3, 0);
    g.rinsert(1, 4, 3);
    g.rinsert(2, 3, 0);
    g.rinsert(2, 6, 1);
    g.rinsert(3, 5, 2);
    g.rinsert(4, 7, 0);
    g.rinsert(5, 7, 0);
    g.rinsert(5, 8, 0);
    g.rinsert(6, 8, 0);
    g.rinsert(6, 11, 4);
    g.rinsert(7, 9, 3);
    g.rinsert(8, 10, 2);
    g.rinsert(9, 12, 0);
    g.rinsert(10, 12, 0);
    g.rinsert(11, 12, 0);
    g.rinsert(12, 13, 1);
    digraph::dijkstra_longest(g, 0, prevs.begin(), dists.begin());
    graph::vertex_type prevs_check3[] = {0, 0, 0, 1, 1, 3, 2, 4, 5, 7, 8, 6, 9, 12};
    int dists_check3[] = {0, 3, 2, 3, 6, 5, 3, 6, 5, 9, 7, 7, 9, 10};
    pass = equal(prevs.begin(), prevs.end(), prevs_check3) &&
        equal(dists.begin(), dists.end(), dists_check3);
    copy(prevs.begin(), prevs.end(), log(DEBUG_)(" "));
    log() << endl;
    copy(dists.begin(), dists.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test dijkstra_longest: " << (pass ? "pass": "FAILED") << endl;

    // case 4: acyclic dijkstra
    n = 11;
    g.clear();
    g.reserve(n, n);
    prevs.assign(n, 0);
    dists.assign(n, 0);
    g.rinsert(0, 3, 3);
    g.rinsert(1, 3, 5);
    g.rinsert(2, 4, 2);
    g.rinsert(3, 5, 2);
    g.rinsert(4, 5, 3);
    g.rinsert(5, 6, 4);
    g.rinsert(5, 7, 2);
    g.rinsert(6, 8, 2);
    g.rinsert(6, 9, 3);
    g.rinsert(7, 8, 5);
    g.rinsert(8, 10, 5);
    g.rinsert(9, 10, 6);
    digraph::acyclic_dijkstra_shortest(g, 0, prevs.begin(), dists.begin());
	graph::vertex_type NV=graph::null_vertex;
    graph::vertex_type prevs_check2[] = {0, NV, NV, 0, NV, 3, 5, 5, 6, 6, 8};
    int INF = numeric_limits<int>::max();
    int dists_check2[] = {0, INF, INF, 3, INF, 5, 9, 7, 11, 12, 16};
    pass = equal(prevs.begin(), prevs.end(), prevs_check2) &&
        equal(dists.begin(), dists.end(), dists_check2);
    copy(prevs.begin(), prevs.end(), log(DEBUG_)(" "));
    log() << endl;
    copy(dists.begin(), dists.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test acyclic_dijkstra_shortest: " << (pass ? "pass": "FAILED") << endl;

    // case 5: longest acyclic dijkstra
    n = 14;
    g.clear();
    g.reserve(n, n);
    prevs.assign(n, 0);
    dists.assign(n, 0);
    g.rinsert(0, 1, 3);
    g.rinsert(0, 2, 2);
    g.rinsert(1, 3, 0);
    g.rinsert(1, 4, 3);
    g.rinsert(2, 3, 0);
    g.rinsert(2, 6, 1);
    g.rinsert(3, 5, 2);
    g.rinsert(4, 7, 0);
    g.rinsert(5, 7, 0);
    g.rinsert(5, 8, 0);
    g.rinsert(6, 8, 0);
    g.rinsert(6, 11, 4);
    g.rinsert(7, 9, 3);
    g.rinsert(8, 10, 2);
    g.rinsert(9, 12, 0);
    g.rinsert(10, 12, 0);
    g.rinsert(11, 12, 0);
    g.rinsert(12, 13, 1);
    digraph::acyclic_dijkstra_longest(g, 0, prevs.begin(), dists.begin());
    graph::vertex_type prevs_check4[] = {0, 0, 0, 1, 1, 3, 2, 4, 5, 7, 8, 6, 9, 12};
    int dists_check4[] = {0, 3, 2, 3, 6, 5, 3, 6, 5, 9, 7, 7, 9, 10};
    pass = equal(prevs.begin(), prevs.end(), prevs_check4) &&
        equal(dists.begin(), dists.end(), dists_check4);
    copy(prevs.begin(), prevs.end(), log(DEBUG_)(" "));
    log() << endl;
    copy(dists.begin(), dists.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test acyclic_dijkstra_longest: " << (pass ? "pass": "FAILED") << endl;
}

void test_utility() {
    Trace trace(INFO_, "test_utility()");
    bool pass = true;

    // test fill_max, fill_min
    const int N = 3;
    {
        vector<int> values(N);
        graph::fill_max(values.begin(), values.end());
        pass = true;
        for (int i=0; i<N; ++i)
            pass &= (values[i]==numeric_limits<int>::max());
        log(DEBUG_) << "max vlaue: " << values[0] << endl;
        log(INFO_) << "test fill_max<int>: " << (pass ? "pass": "FAILED") << endl;
        graph::fill_min(values.begin(), values.end());
        pass = true;
        for (int i=0; i<N; ++i)
            pass &= (values[i]==numeric_limits<int>::min());
        log(DEBUG_) << "min vlaue: " << values[0] << endl;
        log(INFO_) << "test fill_max<int>: " << (pass ? "pass": "FAILED") << endl;
    }

    {
        vector<unsigned int> values(N);
        graph::fill_max(values.begin(), values.end());
        pass = true;
        for (int i=0; i<N; ++i)
            pass &= (values[i]==numeric_limits<unsigned int>::max());
        log(DEBUG_) << "max vlaue: " << values[0] << endl;
        log(INFO_) << "test fill_max<unsigned int>: " << (pass ? "pass": "FAILED") << endl;
        graph::fill_min(values.begin(), values.end());
        pass = true;
        for (int i=0; i<N; ++i)
            pass &= (values[i]==numeric_limits<unsigned int>::min());
        log(DEBUG_) << "min vlaue: " << values[0] << endl;
        log(INFO_) << "test fill_max<unsigned int>: " << (pass ? "pass": "FAILED") << endl;
    }

    {
        vector<double> values(N);
        graph::fill_max(values.begin(), values.end());
        pass = true;
        for (int i=0; i<N; ++i)
            pass &= (values[i]==numeric_limits<double>::max());
        log(DEBUG_) << "max vlaue: " << values[0] << endl;
        log(INFO_) << "test fill_max<double>: " << (pass ? "pass": "FAILED") << endl;
        graph::fill_min(values.begin(), values.end());
        pass = true;
        for (int i=0; i<N; ++i)
            pass &= (values[i]==-numeric_limits<double>::max());
        log(DEBUG_) << "min vlaue: " << values[0] << endl;
        log(INFO_) << "test fill_max<double>: " << (pass ? "pass": "FAILED") << endl;
    }

    // test WeightedVertex
    graph::WeightedVertex<int> v1(30, 2), v2(5), v3;
    pass = (v1.weight==30 && v1.vertex==2) &&
         (v2.weight==5 && v2.vertex==graph::null_vertex) &&
         (v3.weight==0 && v3.vertex==graph::null_vertex);
    log(INFO_) << "test WeightedVertex constructor: " << (pass ? "pass": "FAILED") << endl;

    graph::WeightedVertex<long> v4 = graph::make_weighted_vertex(12L, 3);
    pass = (v4.weight==12 && v4.vertex==3);
    log(INFO_) << "test make_weighted_vertex: " << (pass ? "pass": "FAILED") << endl;

    v3 = v4;
    pass = (v3.weight==12 && v3.vertex==3);
    log(INFO_) << "test WeightedVertex convertion constructor: " << (pass ? "pass": "FAILED") << endl;

    graph::WeightedCompare<less<int> > comp;
    pass = comp(v2,v1) && !comp(v1,v2);
    log(INFO_) << "test WeightedCompare: " << (pass ? "pass": "FAILED") << endl;

    graph::WeightedReverseCompare<less<int> > rcomp;
    pass = rcomp(v1,v2) && !rcomp(v2,v1);
    log(INFO_) << "test WeightedReverseCompare: " << (pass ? "pass": "FAILED") << endl;

    pass = graph::make_weighted_compare(less<int>())(v2,v1) &&
        !graph::make_weighted_compare(less<int>())(v1,v2);
    log(INFO_) << "test make_weighted_compare: " << (pass ? "pass": "FAILED") << endl;

    pass = graph::make_weighted_rcompare(less<int>())(v1,v2) &&
        !graph::make_weighted_rcompare(less<int>())(v2,v1);
    log(INFO_) << "test make_weighted_rcompare: " << (pass ? "pass": "FAILED") << endl;
}
