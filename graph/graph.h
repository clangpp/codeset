#ifndef GRAPH_H_
#define GRAPH_H_

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <queue>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

#include "crosslist/crosslist.h"

namespace graph {

typedef std::size_t size_type;
typedef size_type vertex_type;

const vertex_type null_vertex = static_cast<vertex_type>(-1);

// fill [first, last) with maximum representable value of
//  typename std::numeric_limits<ForwardIterator>::value_type
template <typename ForwardIterator>
void fill_max(ForwardIterator first, ForwardIterator last) {
  typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
  std::fill(first, last, std::numeric_limits<value_type>::max());
}

// fill [first, last) with minimum representable value of
//  typename std::numeric_limits<ForwardIterator>::value_type
template <typename ForwardIterator>
void fill_min(ForwardIterator first, ForwardIterator last) {
  typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
  std::fill(first, last, std::numeric_limits<value_type>::lowest());
}

template <typename WeightT>
struct WeightedVertex {
  typedef WeightT weight_type;
  typedef graph::vertex_type vertex_type;
  weight_type weight;
  vertex_type vertex;
  WeightedVertex(const weight_type& w = weight_type(),
                 const vertex_type& v = null_vertex)
      : weight(w), vertex(v) {}
  template <typename WeightT1>
  WeightedVertex(const WeightedVertex<WeightT1>& wv)
      : weight(static_cast<weight_type>(wv.weight)), vertex(wv.vertex) {}
};

template <typename WeightCompare>
struct WeightedCompare
    : std::binary_function<
          WeightedVertex<typename WeightCompare::first_argument_type>,
          WeightedVertex<typename WeightCompare::first_argument_type>, bool> {
  typedef typename WeightCompare::first_argument_type weight_type;
  WeightCompare weight_comp;
  WeightedCompare(WeightCompare w_comp = WeightCompare())
      : weight_comp(w_comp) {}
  bool operator()(const WeightedVertex<weight_type>& lhs,
                  const WeightedVertex<weight_type>& rhs) const {
    bool result = false;
    if ((result = weight_comp(lhs.weight, rhs.weight))) {
      return result;
    }
    if ((result = weight_comp(rhs.weight, lhs.weight))) {
      return !result;
    }
    return lhs.vertex < rhs.vertex;
  }
};

template <typename WeightCompare>
struct WeightedReverseCompare
    : std::binary_function<
          WeightedVertex<typename WeightCompare::first_argument_type>,
          WeightedVertex<typename WeightCompare::first_argument_type>, bool> {
  typedef typename WeightCompare::first_argument_type weight_type;
  WeightCompare weight_comp;
  WeightedReverseCompare(WeightCompare w_comp = WeightCompare())
      : weight_comp(w_comp) {}
  bool operator()(const WeightedVertex<weight_type>& lhs,
                  const WeightedVertex<weight_type>& rhs) const {
    bool result = false;
    if ((result = weight_comp(rhs.weight, lhs.weight))) {
      return result;
    }
    if ((result = weight_comp(lhs.weight, rhs.weight))) {
      return !result;
    }
    return lhs.vertex < rhs.vertex;
  }
};

template <typename WeightT>
WeightedVertex<WeightT> make_weighted_vertex(const WeightT& weight,
                                             const vertex_type& vertex) {
  return WeightedVertex<WeightT>(weight, vertex);
}

template <typename BinaryPredicate>
WeightedCompare<BinaryPredicate> make_weighted_compare(BinaryPredicate pred) {
  return WeightedCompare<BinaryPredicate>(pred);
}

template <typename BinaryPredicate>
WeightedReverseCompare<BinaryPredicate> make_weighted_rcompare(
    BinaryPredicate pred) {
  return WeightedReverseCompare<BinaryPredicate>(pred);
}

}  // namespace graph

namespace digraph {

using namespace graph;

template <typename T, typename OutputIterator>
OutputIterator topological_sort(const CrossList<T>& g, OutputIterator result) {
  if (g.row_count() != g.column_count()) {
    throw std::invalid_argument("not a digraph");
  }
  size_type vertex_count = g.row_count();
  std::queue<vertex_type> q;
  std::vector<size_type> indegrees(vertex_count);
  for (vertex_type v = 0; v < vertex_count; ++v) {
    indegrees[v] = g.column_size(v);
    if (0 == indegrees[v]) {
      q.push(v);
    }
  }
  for (size_type counter = 0; counter < vertex_count; ++counter) {
    if (q.empty()) {
      throw std::runtime_error("cycle detected in digraph");
    }
    vertex_type v = q.front();  // pick a 0-indegree vertex
    q.pop();
    typename CrossList<T>::const_row_iterator row_iter, row_end;
    row_iter = g.row_begin(v), row_end = g.row_end(v);
    for (; row_iter != row_end; ++row_iter) {
      vertex_type u = row_iter.column();
      --indegrees[u];
      if (0 == indegrees[u]) {
        q.push(u);  // enqueue 0-indegree vertex
      }
    }
    *(result++) = v;
  }
  return result;
}

template <typename T, typename OutputIterator>
OutputIterator topological_sort_grouped(const CrossList<T>& g,
                                        OutputIterator result) {
  if (g.row_count() != g.column_count()) {
    throw std::invalid_argument("not a digraph");
  }
  size_type vertex_count = g.row_count();

  // initialize work components
  std::vector<size_type> indegrees(vertex_count);
  size_type max_group_id = 0;
  std::vector<size_type> group_ids(vertex_count, max_group_id);
  std::list<vertex_type> topo_unsorted;
  std::vector<std::deque<vertex_type>> topo_groups(1);
  std::vector<bool> topo_grouped(vertex_count, false);
  for (vertex_type v = 0; v < vertex_count; ++v) {
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
    for (std::list<vertex_type>::iterator vi = topo_unsorted.begin();
         vi != topo_unsorted.end();) {
      if (indegrees[*vi] > 0) {
        ++vi;
        continue;
      }
      typename CrossList<T>::const_row_iterator row_iter, row_end;
      row_iter = g.row_begin(*vi), row_end = g.row_end(*vi);
      for (; row_iter != row_end; ++row_iter) {
        vertex_type u = row_iter.column();
        --indegrees[u];
        group_ids[u] = std::max(group_ids[u], group_ids[*vi] + 1);
        if (group_ids[u] > max_group_id) {
          max_group_id = group_ids[u];
        }
      }
      topo_unsorted.erase(vi++);
      none_sorted = false;
    }
    if (none_sorted) {
      throw std::runtime_error("cycle detected in digraph");
    }
  }

  // group vertices by group_id
  std::vector<std::deque<vertex_type>> inlinks(vertex_count);
  topo_groups.resize(max_group_id + 1);
  for (size_type gid = 0; gid < topo_groups.size(); ++gid) {
    for (size_type i = 0; i < topo_groups[gid].size(); ++i) {
      vertex_type v = topo_groups[gid][i];
      typename CrossList<T>::const_row_iterator row_iter, row_end;
      row_iter = g.row_begin(v), row_end = g.row_end(v);
      for (; row_iter != row_end; ++row_iter) {
        vertex_type u = row_iter.column();
        inlinks[u].push_back(v);
        if (topo_grouped[u]) {
          continue;
        }
        topo_groups[group_ids[u]].push_front(u);
        topo_grouped[u] = true;
      }
    }
  }

  // output topological sequences
  for (size_type gid = 0; gid < topo_groups.size(); ++gid) {
    for (size_type i = 0; i < topo_groups[gid].size(); ++i)
      *(result++) = topo_groups[gid][i];
  }
  return result;
}

template <typename T, typename OutputIterator>
OutputIterator topological_sort_dfs(const CrossList<T>& g,
                                    OutputIterator result) {
  if (g.row_count() != g.column_count()) {
    throw std::invalid_argument("not a digraph");
  }
  size_type vertex_count = g.row_count();

  // prepare components
  std::deque<vertex_type> head_vertices, tail_vertices;
  for (vertex_type v = 0; v < vertex_count; ++v) {
    if (g.column_size(v) == 0) {  // 0-indegree vertex
      head_vertices.push_back(v);
      if (g.row_size(v) == 0) {  // 0-outdegree vertex
        tail_vertices.push_back(v);
      }
    }
  }

  // make every vertex's in-link list, in DFS order
  std::vector<std::deque<vertex_type>> inlinks(vertex_count);
  typedef typename CrossList<T>::const_row_iterator const_row_iterator;
  std::stack<std::pair<const_row_iterator, const_row_iterator>> dfs_stack;
  std::vector<bool> visited(vertex_count, false);
  for (; !head_vertices.empty(); head_vertices.pop_back()) {
    vertex_type v = head_vertices.back();
    dfs_stack.push(std::make_pair(g.row_begin(v), g.row_end(v)));
    visited[v] = true;
  }
  while (!dfs_stack.empty()) {
    const_row_iterator& iter = dfs_stack.top().first;
    const_row_iterator& end = dfs_stack.top().second;
    if (iter == end) {  // not an edge
      dfs_stack.pop();
      continue;
    }
    vertex_type vs = iter.row(), vt = iter.column();
    ++iter;
    inlinks[vt].push_back(vs);
    if (visited[vt]) {
      continue;  // skip visited vertex
    }
    visited[vt] = true;
    if (g.row_size(vt) == 0) {  // 0-outdegree vertex
      tail_vertices.push_back(vt);
    }
    dfs_stack.push(std::make_pair(g.row_begin(vt), g.row_end(vt)));
  }

  // sign vertices with order numbers, in DFS order
  std::stack<std::deque<vertex_type>*> inlink_stack;
  for (; !tail_vertices.empty(); tail_vertices.pop_back()) {
    inlink_stack.push(&inlinks[tail_vertices.back()]);
  }
  size_type global_order = 0;
  std::vector<size_type> vertex_orders(vertex_count, 0);
  while (!inlink_stack.empty()) {
    std::deque<vertex_type>* inlink = inlink_stack.top();
    if (inlink->empty()) {
      vertex_type v = inlink - &inlinks[0];
      vertex_orders[v] = ++global_order;
      inlink_stack.pop();
    } else {
      vertex_type v = inlink->front();
      if (0 == vertex_orders[v]) {
        inlink_stack.push(&inlinks[v]);
      }
      inlink->pop_front();
    }
  }

  // sort vertex by signed order numbers
  std::vector<std::pair<size_type, vertex_type>> sorting(vertex_count);
  for (vertex_type v = 0; v < vertex_count; ++v) {
    sorting[v] = std::make_pair(vertex_orders[v], v);
  }
  std::sort(sorting.begin(), sorting.end());

  // return ordered vertices
  for (size_type i = 0; i < vertex_count; ++i) {
    *(result++) = sorting[i].second;
  }
  return result;
}

// pre-condition: g.row_count()==g.column_count()
// pre-condition: array prev and dist are well initialized
template <typename T, typename RandomAccessIterator1,
          typename RandomAccessIterator2, typename BinaryPredicate>
void dijkstra(const CrossList<T>& g, vertex_type s, RandomAccessIterator1 prev,
              RandomAccessIterator2 dist, BinaryPredicate pred) {
  if (g.row_count() != g.column_count()) {
    throw std::invalid_argument("not a digraph");
  }
  if (s >= g.column_count()) {
    throw std::out_of_range("s is not a valid vertex in g");
  }

  // use priority_queue to arrange vertices by their states
  typedef T dist_type;
  typedef WeightedVertex<dist_type> weighted_vertex;
  typedef WeightedReverseCompare<BinaryPredicate> reverse_compare;
  std::priority_queue<weighted_vertex, std::vector<weighted_vertex>,
                      reverse_compare>
      states(make_weighted_rcompare(pred));
  prev[s] = s;
  dist[s] = 0;
  states.push(make_weighted_vertex(dist[s], s));
  while (!states.empty()) {
    dist_type d = states.top().weight;
    vertex_type v = states.top().vertex;
    states.pop();
    if (dist[v] < d) {
      continue;  // already adjusted by other states
    }

    // check on outdegree vertices adjacent to current nearest vertex
    typename CrossList<T>::const_row_iterator row_iter, row_end;
    row_iter = g.row_begin(v);
    row_end = g.row_end(v);
    for (; row_iter != row_end; ++row_iter) {
      vertex_type w = row_iter.column();
      dist_type dvw = *row_iter;
      if (pred(dist[v] + dvw, dist[w])) {
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
  std::fill(prev, prev + g.column_count(), null_vertex);
  fill_max(dist, dist + g.column_count());
  dijkstra(g, s, prev, dist, std::less<T>());
}

template <typename T, typename RandomAccessIterator1,
          typename RandomAccessIterator2>
void dijkstra_longest(const CrossList<T>& g, vertex_type s,
                      RandomAccessIterator1 prev, RandomAccessIterator2 dist) {
  std::fill(prev, prev + g.column_count(), null_vertex);
  fill_min(dist, dist + g.column_count());
  dijkstra(g, s, prev, dist, std::greater<T>());
}

// pre-condition: g.row_count()==g.column_count()
// pre-condition: g must be acyclic digraph
template <typename T, typename RandomAccessIterator1,
          typename RandomAccessIterator2, typename BinaryPredicate>
void acyclic_dijkstra(CrossList<T>& g, vertex_type s,
                      RandomAccessIterator1 prev, RandomAccessIterator2 dist,
                      BinaryPredicate pred) {
  if (g.row_count() != g.column_count()) {
    throw std::invalid_argument("not a digraph");
  }
  if (s >= g.column_count()) {
    throw std::out_of_range("s is not a valid vertex in g");
  }

  // initialize states
  typedef T dist_type;
  dist_type null_dist = dist[s];
  prev[s] = s;
  dist[s] = 0;
  std::queue<vertex_type> topo_q;  // vertices with indegree 0
  for (vertex_type v = 0; v < g.column_count(); ++v) {
    if (g.column_size(v) == 0) {
      topo_q.push(v);
    }
  }

  // work in topological order
  while (!topo_q.empty()) {
    vertex_type v = topo_q.front();
    topo_q.pop();

    // remove vertices those cannot reach from s
    typename CrossList<T>::row_iterator row_iter, row_end;
    if (dist[v] == null_dist) {
      row_iter = g.row_begin(v), row_end = g.row_end(v);
      while (row_iter != row_end) {
        vertex_type w = row_iter.column();
        g.erase(row_iter++);
        if (g.column_size(w) == 0) {
          topo_q.push(w);
        }
      }
      continue;
    }

    // check on outdegree vertices adjacent to current nearest vertex
    row_iter = g.row_begin(v), row_end = g.row_end(v);
    while (row_iter != row_end) {
      vertex_type w = row_iter.column();
      dist_type dvw = *row_iter;
      if (pred(dist[v] + dvw, dist[w])) {
        prev[w] = v;
        dist[w] = dist[v] + dvw;
      }
      g.erase(row_iter++);
      if (g.column_size(w) == 0) {
        topo_q.push(w);
      }
    }
  }
}

template <typename T, typename RandomAccessIterator1,
          typename RandomAccessIterator2>
void acyclic_dijkstra_shortest(CrossList<T>& g, vertex_type s,
                               RandomAccessIterator1 prev,
                               RandomAccessIterator2 dist) {
  std::fill(prev, prev + g.column_count(), null_vertex);
  fill_max(dist, dist + g.column_count());
  acyclic_dijkstra(g, s, prev, dist, std::less<T>());
}

template <typename T, typename RandomAccessIterator1,
          typename RandomAccessIterator2>
void acyclic_dijkstra_longest(CrossList<T>& g, vertex_type s,
                              RandomAccessIterator1 prev,
                              RandomAccessIterator2 dist) {
  std::fill(prev, prev + g.column_count(), null_vertex);
  fill_min(dist, dist + g.column_count());
  acyclic_dijkstra(g, s, prev, dist, std::greater<T>());
}

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
template <typename T, typename RandomAccessIterator, typename OutputIterator1,
          typename OutputIterator2>
void print_topological(const CrossList<T>& g, RandomAccessIterator topo_first,
                       RandomAccessIterator topo_last,
                       OutputIterator1 prefix_string,
                       OutputIterator2 vertex_index) {
  size_type vertex_count = g.column_count();
  size_type topo_count = static_cast<size_type>(topo_last - topo_first);

  // place vertex coordinates
  std::vector<size_type> column_orders;
  for (size_type i = 0; i < topo_count; ++i) {
    if (!column_orders.empty() &&
        0 == g.row_size(topo_first[column_orders.back()])) {
      column_orders.pop_back();
    }
    column_orders.push_back(i);
  }

  // initialize vertex degrees information
  std::vector<size_type> outdegrees(vertex_count);
  std::vector<std::list<vertex_type>> inlinks(vertex_count);
  for (size_type i = 0; i < topo_count; ++i) {
    vertex_type v = topo_first[i];
    outdegrees[v] = g.row_size(v);
    typename CrossList<T>::const_row_iterator row_iter, row_end;
    row_iter = g.row_begin(v), row_end = g.row_end(v);
    for (; row_iter != row_end; ++row_iter) {
      inlinks[row_iter.column()].push_back(v);
    }
  }

  // draw arrows and output results
  for (size_type row = 0; row < topo_count; ++row) {
    vertex_type vt = topo_first[row];
    std::string line;
    for (size_type col = 0;
         col < column_orders.size() && column_orders[col] < row; ++col) {
      vertex_type vs = topo_first[column_orders[col]];
      std::string token;
      bool cont_line = !line.empty() && '-' == line[line.size() - 1];

      // vs arrow to vt
      if (!inlinks[vt].empty() && vs == inlinks[vt].front()) {
        inlinks[vt].pop_front();
        --outdegrees[vs];
        token = (0 == outdegrees[vs] ? "`---" : "|---");
        if (cont_line) {
          line[line.size() - 1] = '>';
        }
      } else {  // vs doesn't arrow to vt
        token = cont_line ? "----" : "    ";
        if (outdegrees[vs] > 0) {
          token[0] = '|';
        }
        // if (cont_line && '|'==token[0]) {  // jumping signs
        //     token[1] = '`', line[line.size()-1] = '\'';
        // }
      }
      line.append(token);
    }
    if (!line.empty() && '-' == line[line.size() - 1]) {
      line[line.size() - 1] = '>';
    }
    *(prefix_string++) = line;
    *(vertex_index++) = vt;
  }
}

// print acyclic digraph to output iterators
// pre-condition: g.row_count()==g.column_count()
// pre-condition: g must be acyclic digraph (topological sortable)
// remarks: topological sort all vertices in digraph g,
//     then call print_topological() to print them to output iterators
template <typename T, typename OutputIterator1, typename OutputIterator2>
void print_acyclic(const CrossList<T>& g, OutputIterator1 prefix_string,
                   OutputIterator2 vertex_index) {
  std::vector<vertex_type> topo_vertices(g.column_count());
  // topological_sort(g, topo_vertices.begin());
  // topological_sort_grouped(g, topo_vertices.begin());
  topological_sort_dfs(g, topo_vertices.begin());
  print_topological(g, topo_vertices.begin(), topo_vertices.end(),
                    prefix_string, vertex_index);
}

// print acyclic digraph to std::ostream
// pre-condition: the same as its output iterator version
// remarks: a wrapper of its output iterator version,
//     set output iterators to std::ostream_iterator
template <typename T>
void print_acyclic(const CrossList<T>& g, std::ostream& out = std::cout) {
  std::ostream_iterator<std::string> prefix_iterator(out, "");
  std::ostream_iterator<vertex_type> vertex_iterator(out, "\n");
  print_acyclic(g, prefix_iterator, vertex_iterator);
}

}  // namespace digraph

#endif  // GRAPH_H_
