// test.cpp
#include "graph/graph.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "logging/logging.h"

using namespace std;
using namespace logging;

void test_topological_sort();

int main() {
    test_topological_sort();
    system("pause");
}

void test_topological_sort() {
    Trace trace(INFO_, "test_topological_sort()");
    bool pass = true;

    CrossList<bool> g(7, 7);
    g.insert(0, 1, true);
    g.insert(0, 2, true);
    g.insert(0, 3, true);
    g.insert(1, 3, true);
    g.insert(1, 4, true);
    g.insert(2, 5, true);
    g.insert(3, 2, true);
    g.insert(3, 5, true);
    g.insert(3, 6, true);
    g.insert(4, 3, true);
    g.insert(4, 6, true);
    g.insert(6, 5, true);

    vector<size_t> seq(7);
    vector<size_t>::iterator iter =
        digraph::topological_sort(g, seq.begin());
    size_t topo_check[] = {0, 1, 4, 3, 2, 6, 5};
    pass = (iter==seq.end()) && equal(seq.begin(), iter, topo_check);
    pass &= g.empty();
    log(INFO_) << "test topological_sort: " << (pass ? "pass": "FAILED") << endl;

    g.insert(3, 4, true);
    g.insert(4, 5, true);
    g.insert(5, 3, true);
    try {
        digraph::topological_sort(g, seq.begin());
        pass = false;
    } catch (const exception& e) {
        pass = true;
    }
    log(INFO_) << "test topological_sort boundry: " << (pass ? "pass": "FAILED") << endl;
}
