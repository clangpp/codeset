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

int main() {
	try {
        // logging::level() = DEBUG_;
		test_topological_sort();
		test_dijkstra();
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
    digraph::dijkstra(g, 0, prevs.begin(), dists.begin());
    graph::vertex_type prevs_check[] = {0, 0, 3, 0, 3, 6, 3};
    int dists_check[] = {0, 2, 3, 1, 3, 6, 5};
    pass = equal(prevs.begin(), prevs.end(), prevs_check) &&
        equal(dists.begin(), dists.end(), dists_check);
    copy(prevs.begin(), prevs.end(), log(DEBUG_)(" "));
    log() << endl;
    copy(dists.begin(), dists.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test dijkstra: " << (pass ? "pass": "FAILED") << endl;

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
    digraph::dijkstra(g, 0, prevs.begin(), dists.begin());
    graph::vertex_type prevs_check1[] = {0, 4, 0, 1, 2};
    int dists_check1[] = {0, 8, 5, 17, 6};
    pass = equal(prevs.begin(), prevs.end(), prevs_check1) &&
        equal(dists.begin(), dists.end(), dists_check1);
    copy(prevs.begin(), prevs.end(), log(DEBUG_)(" "));
    log() << endl;
    copy(dists.begin(), dists.end(), log(DEBUG_)(" "));
    log() << endl;
    log(INFO_) << "test dijkstra: " << (pass ? "pass": "FAILED") << endl;

    // case 3: acyclic dijkstra
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
    digraph::dijkstra_acyclic(g, 0, prevs.begin(), dists.begin());
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
    log(INFO_) << "test dijkstra_acyclic: " << (pass ? "pass": "FAILED") << endl;
}
