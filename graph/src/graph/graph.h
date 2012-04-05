// graph.h
#ifndef GRAPH_H_
#define GRAPH_H_

#include "../crosslist/crosslist.h"

namespace graph {
}  // namespace graph

namespace digraph {

template <typename T, typename OutputIterator>
OutputIterator topological_sort(CrossList<T>& g, OutputIterator result);

}  // namespace digraph

#include "graph-inl.h"

#endif  // GRAPH_H_
