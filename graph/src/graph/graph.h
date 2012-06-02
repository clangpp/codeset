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
inline void fill_max(ForwardIterator first, ForwardIterator last);

// fill [first, last) with minimum representable value of
//  typename std::numeric_limits<ForwardIterator>::value_type
template <typename ForwardIterator>
inline void fill_min(ForwardIterator first, ForwardIterator last);

template <typename WeightT>
struct WeightedVertex {
    typedef WeightT weight_type;
    typedef graph::vertex_type vertex_type;
    weight_type weight;
    vertex_type vertex;
    WeightedVertex(const weight_type& w=weight_type(),
            const vertex_type& v=null_vertex): weight(w), vertex(v) {}
    template <typename WeightT1>
    WeightedVertex(const WeightedVertex<WeightT1>& wv):
        weight(static_cast<weight_type>(wv.weight)), vertex(wv.vertex) {}
};

template <typename WeightCompare>
struct WeightedCompare:
    std::binary_function<
        WeightedVertex<typename WeightCompare::first_argument_type>,
        WeightedVertex<typename WeightCompare::first_argument_type>, bool> {
    typedef typename WeightCompare::first_argument_type weight_type;
    WeightCompare weight_comp;
    WeightedCompare(WeightCompare w_comp=WeightCompare()):
		weight_comp(w_comp) {}
    bool operator () (const WeightedVertex<weight_type>& lhs,
            const WeightedVertex<weight_type>& rhs) const {
        bool result = false;
        if (result=weight_comp(lhs.weight, rhs.weight)) return result;
        if (result=weight_comp(rhs.weight, lhs.weight)) return !result;
        return lhs.vertex < rhs.vertex;
    }
};

template <typename WeightCompare>
struct WeightedReverseCompare:
    std::binary_function<
        WeightedVertex<typename WeightCompare::first_argument_type>,
        WeightedVertex<typename WeightCompare::first_argument_type>, bool> {
    typedef typename WeightCompare::first_argument_type weight_type;
    WeightCompare weight_comp;
    WeightedReverseCompare(WeightCompare w_comp=WeightCompare()):
		weight_comp(w_comp) {}
    bool operator () (const WeightedVertex<weight_type>& lhs,
            const WeightedVertex<weight_type>& rhs) const {
        bool result = false;
        if (result=weight_comp(rhs.weight, lhs.weight)) return result;
        if (result=weight_comp(lhs.weight, rhs.weight)) return !result;
        return lhs.vertex < rhs.vertex;
    }
};

template <typename WeightT>
WeightedVertex<WeightT> make_weighted_vertex(
        const WeightT& weight, const vertex_type& vertex) {
    return WeightedVertex<WeightT>(weight, vertex);
}

template <typename BinaryPredicate>
WeightedCompare<BinaryPredicate>
make_weighted_compare(BinaryPredicate pred) {
    return WeightedCompare<BinaryPredicate>(pred);
}

template <typename BinaryPredicate>
WeightedReverseCompare<BinaryPredicate>
make_weighted_rcompare(BinaryPredicate pred) {
    return WeightedReverseCompare<BinaryPredicate>(pred);
}

}  // namespace graph

namespace digraph {

using namespace graph;

template <typename T, typename OutputIterator>
OutputIterator topological_sort(CrossList<T>& g, OutputIterator result);

// pre-condition: g.row_count()==g.column_count()
// pre-condition: array prev and dist are well initialized
template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void dijkstra(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist,
        BinaryPredicate pred);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
inline void dijkstra_shortest(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
inline void dijkstra_longest(const CrossList<T>& g, vertex_type s,
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
inline void acyclic_dijkstra_shortest(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
inline void acyclic_dijkstra_longest(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist);

// print acyclic digraph
// pre-condition: g.row_count()==g.column_count()
// pre-condition: g must be acyclic digraph
template <typename T>
void print_acyclic(const CrossList<T>& g, std::ostream& out = std::cout);

}  // namespace digraph

#include "graph-inl.h"

#endif  // GRAPH_H_
