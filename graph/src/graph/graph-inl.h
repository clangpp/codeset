// graph-inl.h
#ifndef GRAPH_INL_H_
#define GRAPH_INL_H_

#include "graph.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace graph {

template <typename ForwardIterator>
void fill_max(ForwardIterator first, ForwardIterator last) {
    typedef typename std::iterator_traits<
        ForwardIterator>::value_type value_type;
    std::fill(first, last, std::numeric_limits<value_type>::max());
}

template <typename ForwardIterator>
void fill_min(ForwardIterator first, ForwardIterator last) {
    typedef typename std::iterator_traits<
        ForwardIterator>::value_type value_type;
    typedef std::numeric_limits<value_type> limits;
    std::fill(first, last, limits::is_integer ? limits::min() : -limits::max());
}

}  // namespace graph

namespace digraph {

template <typename T, typename OutputIterator>
OutputIterator topological_sort(CrossList<T>& g, OutputIterator result) {
    if (g.row_count()!=g.column_count())
        throw std::invalid_argument("not a digraph");
    std::queue<vertex_type> q;
    for (vertex_type i=0; i<g.column_count(); ++i) {
        if (g.column_size(i)==0) q.push(i);  // enqueue node with indegree 0
    }
    for (size_type counter=0; counter!=g.column_count(); ++counter) {
        if (q.empty())
            throw std::runtime_error("cycle detected in graph");
        vertex_type p = q.front();  // get a node with indegree 0
        q.pop();
        typename CrossList<T>::row_iterator row_iter=g.row_begin(p);
        typename CrossList<T>::row_iterator row_end=g.row_end(p);
        while (row_iter!=row_end) {  // erase related edge from graph
            vertex_type c = row_iter.column();
            g.erase(row_iter++);
            if (g.column_size(c)==0) q.push(c);  // enqueue node with indegree 0
        }
        *(result++) = p;
    }
	return result;
}

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void dijkstra(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist,
        BinaryPredicate pred) {
    if (g.row_count()!=g.column_count())
        throw std::invalid_argument("not a digraph");
    if (s >= g.column_count())
        throw std::out_of_range("s is not a valid vertex in g");

    // use priority_queue to arrange vertices by their states
    typedef T dist_type;
    typedef WeightedVertex<dist_type> weighted_vertex;
    typedef WeightedReverseCompare<BinaryPredicate> reverse_compare;
    std::priority_queue<weighted_vertex, std::vector<weighted_vertex>,
        reverse_compare> states(make_weighted_rcompare(pred));
    prev[s] = s;
    dist[s] = 0;
    states.push(make_weighted_vertex(dist[s], s));
    while (!states.empty()) {
        dist_type d = states.top().weight;
        vertex_type v = states.top().vertex;
        states.pop();
        if (dist[v] < d) continue;  // already adjusted by other states

        // check on outdegree vertices adjacent to current nearest vertex
        typename CrossList<T>::const_row_iterator row_iter, row_end;
        row_iter = g.row_begin(v);
        row_end = g.row_end(v);
        for (; row_iter!=row_end; ++row_iter) {
            vertex_type w = row_iter.column();
            dist_type dvw = *row_iter;
            if (pred(dist[v]+dvw, dist[w])) {
                prev[w] = v;
                dist[w] = dist[v] + dvw;
                states.push(make_weighted_vertex(dist[w], w));
            }
        }
    }
}

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void dijkstra_shortest(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist) {
    std::fill(prev, prev+g.column_count(), null_vertex);
    fill_max(dist, dist+g.column_count());
    dijkstra(g, s, prev, dist, std::less<T>());
}

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void dijkstra_longest(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist) {
    std::fill(prev, prev+g.column_count(), null_vertex);
    fill_min(dist, dist+g.column_count());
    dijkstra(g, s, prev, dist, std::greater<T>());
}

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename BinaryPredicate>
void acyclic_dijkstra(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist,
        BinaryPredicate pred) {
    if (g.row_count()!=g.column_count())
        throw std::invalid_argument("not a digraph");
    if (s >= g.column_count())
        throw std::out_of_range("s is not a valid vertex in g");

    // initialize states
    typedef T dist_type;
    dist_type null_dist = dist[s];
    prev[s] = s;
    dist[s] = 0;
    std::queue<vertex_type> topo_q;  // vertices with indegree 0
    for (vertex_type v=0; v<g.column_count(); ++v) {
        if (g.column_size(v)==0) topo_q.push(v);
    }

    // work in topological order
    while (!topo_q.empty()) {
        vertex_type v = topo_q.front();
        topo_q.pop();

        // remove vertices those cannot reach from s
        typename CrossList<T>::row_iterator row_iter, row_end;
        if (dist[v]==null_dist) {
            row_iter=g.row_begin(v), row_end=g.row_end(v);
            while (row_iter!=row_end) {
                vertex_type w = row_iter.column();
                g.erase(row_iter++);
                if (g.column_size(w)==0) topo_q.push(w);
            }
            continue;
        }

        // check on outdegree vertices adjacent to current nearest vertex
        row_iter = g.row_begin(v), row_end = g.row_end(v);
        while (row_iter!=row_end) {
            vertex_type w = row_iter.column();
            dist_type dvw = *row_iter;
            if (pred(dist[v]+dvw, dist[w])) {
                prev[w] = v;
                dist[w] = dist[v] + dvw;
            }
            g.erase(row_iter++);
            if (g.column_size(w)==0) topo_q.push(w);
        }
    }
}

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void acyclic_dijkstra_shortest(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist) {
    std::fill(prev, prev+g.column_count(), null_vertex);
    fill_max(dist, dist+g.column_count());
    acyclic_dijkstra(g, s, prev, dist, std::less<T>());
}

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void acyclic_dijkstra_longest(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist) {
    std::fill(prev, prev+g.column_count(), null_vertex);
    fill_min(dist, dist+g.column_count());
    acyclic_dijkstra(g, s, prev, dist, std::greater<T>());
}

}  // namespace digraph

#endif  // GRAPH_INL_H_
