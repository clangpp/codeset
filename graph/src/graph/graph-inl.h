// graph-inl.h
#ifndef GRAPH_INL_H_
#define GRAPH_INL_H_

#include "graph.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
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
OutputIterator topological_sort(const CrossList<T>& g, OutputIterator result) {
    if (g.row_count()!=g.column_count())
        throw std::invalid_argument("not a digraph");
    size_type vertex_count = g.row_count();
    std::queue<vertex_type> q;
    std::vector<size_type> indegrees(vertex_count);
    for (vertex_type v=0; v<vertex_count; ++v) {
        indegrees[v] = g.column_size(v);
        if (0 == indegrees[v]) q.push(v);
    }
    for (size_type counter=0; counter<vertex_count; ++counter) {
        if (q.empty())
            throw std::runtime_error("cycle detected in digraph");
        vertex_type v = q.front();  // pick a 0-indegree vertex
        q.pop();
        typename CrossList<T>::const_row_iterator row_iter, row_end;
        row_iter = g.row_begin(v), row_end = g.row_end(v);
        for (; row_iter!=row_end; ++row_iter) {
            vertex_type u = row_iter.column();
            --indegrees[u];
            if (0 == indegrees[u]) q.push(u);  // enqueue 0-indegree vertex
        }
        *(result++) = v;
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

template <typename T>
void print_acyclic(const CrossList<T>& g, std::ostream& out) {
    if (g.row_count()!=g.column_count())
        throw std::invalid_argument("not a digraph");
    size_type vertex_count = g.row_count();

    // initialize work components
    std::vector<size_type> indegrees(vertex_count);
    std::vector<size_type> group_ids(vertex_count, 0);
    std::list<vertex_type> topo_unsorted;
    size_type max_group_id = 0;
    std::vector<std::deque<vertex_type> > topo_groups(1);
    std::vector<bool> topo_grouped(vertex_count, false);
    for (vertex_type v=0; v<vertex_count; ++v) {
        indegrees[v] = g.column_size(v);
        topo_unsorted.push_back(v);
        if (0 == indegrees[v]) {
            topo_groups[0].push_back(v);
            topo_grouped[v] = true;
        }
    }

    // topo-sort algorithm to group vertices (signed with group_id)
    while (!topo_unsorted.empty()) {
		bool none_sorted = true;
        for (std::list<vertex_type>::iterator vi=
            topo_unsorted.begin(); vi!=topo_unsorted.end();) {
            if (indegrees[*vi]>0) {
                ++vi;
                continue;
            }
            typename CrossList<T>::const_row_iterator row_iter, row_end;
            row_iter = g.row_begin(*vi), row_end = g.row_end(*vi);
            for (; row_iter!=row_end; ++row_iter) {
                vertex_type u = row_iter.column();
                --indegrees[u];
                group_ids[u] = std::max(group_ids[u], group_ids[*vi]+1);
                if (group_ids[u] > max_group_id) max_group_id = group_ids[u];
            }
            topo_unsorted.erase(vi++);
			none_sorted = false;
        }
		if (none_sorted)
			throw std::runtime_error("cycle detected in digraph");
    }

    // group vertices by group_id
    std::vector<std::deque<vertex_type> > inlinks(vertex_count);
    topo_groups.resize(max_group_id+1);
    for (size_type gid=0; gid<topo_groups.size(); ++gid) {
        for (size_type i=0; i<topo_groups[gid].size(); ++i) {
            vertex_type v = topo_groups[gid][i];
            typename CrossList<T>::const_row_iterator row_iter, row_end;
            row_iter = g.row_begin(v), row_end = g.row_end(v);
            for (; row_iter!=row_end; ++row_iter) {
                vertex_type u = row_iter.column();
                inlinks[u].push_back(v);
                if (topo_grouped[u]) continue;
                topo_groups[group_ids[u]].push_front(u);
                topo_grouped[u] = true;
            }
        }
    }

    // place vertex coordinates
    std::vector<size_type> outdegrees(vertex_count);
    for (vertex_type v=0; v<vertex_count; ++v)
        outdegrees[v] = g.row_size(v);
    std::vector<vertex_type> ordered_vertices;
    std::vector<size_type> column_orders;
    for (size_type gid=0; gid<topo_groups.size(); ++gid) {
        for (size_type i=0; i<topo_groups[gid].size(); ++i) {
            ordered_vertices.push_back(topo_groups[gid][i]);
            if (!column_orders.empty() &&
                0==outdegrees[ordered_vertices[column_orders.back()]]) {
                column_orders.pop_back();
            }
            column_orders.push_back(ordered_vertices.size()-1);
        }
    }

    // draw arrows between vertices
    for (size_type row=0; row<ordered_vertices.size(); ++row) {
        vertex_type vt = ordered_vertices[row];
        std::string line;
        for (size_type col=0; col<column_orders.size() &&
                column_orders[col]<row; ++col) {
            vertex_type vs = ordered_vertices[column_orders[col]];
            std::string token;
            bool cont_line = !line.empty() && '-'==line[line.size()-1];

            // vs arrow to vt
            if (!inlinks[vt].empty() && vs==inlinks[vt].front()) {
                inlinks[vt].pop_front();
                --outdegrees[vs];
                token = (0==outdegrees[vs] ? "`---" : "|---");
                if (cont_line) line[line.size()-1] = '>';
            } else {  // vs doesn't arrow to vt
                token = cont_line ? "----" : "    ";
                if (outdegrees[vs] > 0) token[0] = '|';
                // if (cont_line && '|'==token[0]) {  // jumping signs
                //     token[1] = '`', line[line.size()-1] = '\'';
                // }
            }
            line.append(token);
        }
        if (!line.empty() && '-'==line[line.size()-1])
            line[line.size()-1] = '>';
        out << line << vt << std::endl;
    }
}

}  // namespace digraph

#endif  // GRAPH_INL_H_
