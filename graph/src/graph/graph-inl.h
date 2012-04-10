// graph-inl.h
#ifndef GRAPH_INL_H_
#define GRAPH_INL_H_

#include "graph.h"

#include <algorithm>
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
         typename RandomAccessIterator2>
void dijkstra(const CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist) {
    if (g.row_count()!=g.column_count())
        throw std::invalid_argument("not a digraph");
    if (s >= g.column_count())
        throw std::out_of_range("s is not a valid vertex in g");

    // init vertices states
    typedef T dist_type;
    dist_type infinity_dist = std::numeric_limits<dist_type>::max();
    size_type vertex_count = g.column_count();
    for (vertex_type v=0; v<vertex_count; ++v) {
        prev[v] = v==s ? s : null_vertex;
        dist[v] = v==s ? 0 : infinity_dist;
    }

    // use priority_queue to arrange vertices by their states
    typedef std::pair<dist_type, vertex_type> work_node;
    std::priority_queue<work_node,
        std::vector<work_node>, std::greater<work_node> > states;
    for (states.push(std::make_pair(dist[s], s)); !states.empty(); ) {
        dist_type d = states.top().first;
        vertex_type v = states.top().second;
        states.pop();
        if (dist[v] < d) continue;  // already adjusted by other states

        // check on outdegree vertices adjacent to current nearest vertex
        typename CrossList<T>::const_row_iterator row_iter, row_end;
        row_iter = g.row_begin(v);
        row_end = g.row_end(v);
        for (; row_iter!=row_end; ++row_iter) {
            vertex_type w = row_iter.column();
            dist_type dvw = *row_iter;
            if (dist[v] + dvw < dist[w]) {
                prev[w] = v;
                dist[w] = dist[v] + dvw;
                states.push(std::make_pair(dist[w], w));
            }
        }
    }
}

template <typename T, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
void dijkstra_acyclic(CrossList<T>& g, vertex_type s,
        RandomAccessIterator1 prev, RandomAccessIterator2 dist) {
    if (g.row_count()!=g.column_count())
        throw std::invalid_argument("not a digraph");
    if (s >= g.column_count())
        throw std::out_of_range("s is not a valid vertex in g");

    // init vertices states
    typedef T dist_type;
    dist_type infinity_dist = std::numeric_limits<dist_type>::max();
    size_type vertex_count = g.column_count();
    std::queue<vertex_type> topo_q;  // vertices with indegree 0
    for (vertex_type v=0; v<vertex_count; ++v) {
        prev[v] = v==s ? s : null_vertex;
        dist[v] = v==s ? 0 : infinity_dist;
        if (g.column_size(v)==0) topo_q.push(v);
    }

    // work in topological order
    for (topo_q.push(s); !topo_q.empty(); ) {
        vertex_type v = topo_q.front();
        topo_q.pop();

        // remove vertices those cannot reach from s
        typename CrossList<T>::row_iterator row_iter, row_end;
        if (dist[v]==infinity_dist) {
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
            if (dist[v] + dvw < dist[w]) {
                prev[w] = v;
                dist[w] = dist[v] + dvw;
            }
            g.erase(row_iter++);
            if (g.column_size(w)==0) topo_q.push(w);
        }
    }
}

}  // namespace digraph

#endif  // GRAPH_INL_H_
