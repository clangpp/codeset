// graph-inl.h
#ifndef GRAPH_INL_H_
#define GRAPH_INL_H_

#include "graph.h"
#include <queue>

namespace graph {
}  // namespace graph

namespace digraph {

template <typename T, typename OutputIterator>
OutputIterator topological_sort(CrossList<T>& g, OutputIterator result) {
    typedef typename CrossList<T>::size_type size_type;
    typedef size_type node_type;
    std::queue<node_type> q;
    for (node_type i=0; i<g.column_count(); ++i) {
        if (g.column_size(i)==0) q.push(i);  // enqueue node with indegree 0
    }
    for (size_type counter=0; counter!=g.column_count(); ++counter) {
        if (q.empty())
            throw std::runtime_error("cycle detected in graph");
        node_type p = q.front();  // get a node with indegree 0
        q.pop();
        typename CrossList<T>::row_iterator row_iter=g.row_begin(p);
        typename CrossList<T>::row_iterator row_end=g.row_end(p);
        while (row_iter!=row_end) {  // erase related edge from graph
            node_type c = row_iter.column();
            g.erase(row_iter++);
            if (g.column_size(c)==0) q.push(c);  // enqueue node with indegree 0
        }
        *(result++) = p;
    }
	return result;
}

}  // namespace digraph

#endif  // GRAPH_INL_H_
