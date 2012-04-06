// graph.h
#ifndef GRAPH_H_
#define GRAPH_H_

#include "../crosslist/crosslist.h"

namespace graph {

typedef std::size_t size_type;
typedef size_type vertex_type;

const vertex_type null_vertex = static_cast<vertex_type>(-1);

}  // namespace graph

namespace digraph {

using namespace graph;

template <typename T, typename OutputIterator>
OutputIterator topological_sort(CrossList<T>& g, OutputIterator result);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void dijkstra(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void dijkstra_acyclic(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

}  // namespace digraph

#include "graph-inl.h"

#endif  // GRAPH_H_
