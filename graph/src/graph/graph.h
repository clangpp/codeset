// graph.h
#ifndef GRAPH_H_
#define GRAPH_H_

#include <functional>
#include <iostream>
#include <iterator>

#include "../crosslist/crosslist.h"

namespace graph {

typedef std::size_t size_type;
typedef size_type vertex_type;

const vertex_type null_vertex = static_cast<vertex_type>(-1);

// fill [first, last) with maximum representable value of
//  typename std::numeric_limits<ForwardIterator>::value_type
template <typename ForwardIterator>
void fill_max(ForwardIterator first, ForwardIterator last);

// fill [first, last) with minimum representable value of
//  typename std::numeric_limits<ForwardIterator>::value_type
template <typename ForwardIterator>
void fill_min(ForwardIterator first, ForwardIterator last);

}  // namespace graph

namespace digraph {

using namespace graph;

template <typename T, typename OutputIterator>
OutputIterator topological_sort(const CrossList<T>& g, OutputIterator result);

template <typename T, typename OutputIterator>
OutputIterator topological_sort_grouped(
    const CrossList<T>& g, OutputIterator result);

template <typename T, typename OutputIterator>
OutputIterator topological_sort_dfs(
    const CrossList<T>& g, OutputIterator result);

// pre-condition: g.row_count()==g.column_count()
// pre-condition: array prev and dist are well initialized
template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void dijkstra(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist,
        BinaryPredicate pred);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void dijkstra_shortest(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void dijkstra_longest(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

// pre-condition: g.row_count()==g.column_count()
// pre-condition: g must be acyclic digraph
template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void acyclic_dijkstra(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist,
        BinaryPredicate pred);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void acyclic_dijkstra_shortest(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void acyclic_dijkstra_longest(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

// print a topological sortable subset in digraph
// pre-condition: [topo_first, topo_last) are
//     topological ordered vertex indices in digraph g
// remarks: let topo_count = std::distance(topo_first, topo_last)
// remarks: each OutputIterator have exactly <topo_count> iterations
// remarks: output iterations can be treated as:
//     for (size_type i=0; i<topo_count; ++i) {
//         *(prefix_string++) = prefix_i;
//         *(vertex_index++) = *(topo_first++);
//     }
template <typename T, typename RandomAccessIterator,
    typename OutputIterator1, typename OutputIterator2>
void print_topological(const CrossList<T>& g,
    RandomAccessIterator topo_first, RandomAccessIterator topo_last,
    OutputIterator1 prefix_string, OutputIterator2 vertex_index);

// print acyclic digraph to output iterators
// pre-condition: g.row_count()==g.column_count()
// pre-condition: g must be acyclic digraph (topological sortable)
// remarks: topological sort all vertices in digraph g,
//     then call print_topological() to print them to output iterators
template <typename T, typename OutputIterator1, typename OutputIterator2>
void print_acyclic(const CrossList<T>& g,
    OutputIterator1 prefix_string, OutputIterator2 vertex_index);

// print acyclic digraph to std::ostream
// pre-condition: the same as its output iterator version
// remarks: a wrapper of its output iterator version,
//     set output iterators to std::ostream_iterator
template <typename T>
void print_acyclic(const CrossList<T>& g, std::ostream& out = std::cout);

}  // namespace digraph

#include "graph-inl.h"

#endif  // GRAPH_H_
