// STATUS: small input passed, large input passed.
//
// Idea:
//  1. Edge x+y=n is an edge of a graph, with x and y as nodes, and n as
//    weight.
//  2. If x to y has odd number edges (n1, n2, n3, ..., nt), then x+y is
//    determined as n1-n2+n3-...+nt.
//  3. If there's edge z+z=n, then z is determined (n/2), and thus all nodes
//    connected with z are determined.
//    But normally no need for determination, one path of
//    (n1, n2, ..., nz, ..., nt) and
//    (n1, n2, ..., nz, n(z-to-z), ... nt) must has odd number edges, then x+y
//    can be determined by rule 2.
//  4. If there's circle (z to z) which has odd number edges
//    (n1, n2, n3, ..., nt), then z is determined as (n1-n2+n3-...+nt)/2, thus
//    all nodes connected with z are determined.
//    But normally no need for determination, one path of
//    (n1, n2, ..., nz, ..., nt) and
//    (n1, n2, ..., nz, n(z-to-circle1), ..., n(circle-1-to-z), ..., nt) must
//    has odd number edges, then x+y can be determined by rule 2.
//  5. Unnormally, if x and y are not connected, but x and y can both be
//    determined by rule 3 or rule 4, then x+y can still be determined.
//  6. Otherwise, x+y can't be determined.

#include <cstddef>  // for size_t
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>  // pair
#include <vector>

using namespace std;

struct Edge {
  size_t to = 0;
  int sum = 0;
  Edge(size_t to, int sum): to(to), sum(sum) {}
  Edge() {}
};

class NameIndexDict {
 public:
  size_t IndexOf(const string& name) {
    if (!name_to_index_.count(name)) {
      name_to_index_.insert(make_pair(name, index_to_name_.size()));
      index_to_name_.push_back(name);
    }
    return name_to_index_[name];
  }
  bool HasName(const string& name) { return name_to_index_.count(name) > 0; }
  const string& NameOf(size_t index) {
    return index_to_name_.at(index);  // uses exception to check input range
  }
  size_t size() const { return index_to_name_.size(); }
 private:
  unordered_map<string, size_t> name_to_index_;
  vector<string> index_to_name_;
};

bool DetermineSum(
    const vector<vector<Edge>>& edges,
    size_t var1,
    size_t var2,
    int* sum) {
  vector<bool> visited(edges.size());
  vector<int> var1_plus(edges.size());
  queue<size_t> determined_nodes;
  for (const auto& edge : edges[var1]) {
    var1_plus[edge.to] = edge.sum;
    visited[edge.to] = true;
    determined_nodes.push(edge.to);
  }
  for (; !determined_nodes.empty(); determined_nodes.pop()) {
    size_t start = determined_nodes.front();
    if (start == var2) {
      *sum = var1_plus[var2];
      return true;
    }
    for (const auto& edge1 : edges[start]) {
      size_t via = edge1.to;
      for (const auto& edge2 : edges[via]) {
        size_t next = edge2.to;
        if (!visited[next]) {
          var1_plus[next] = var1_plus[start] - edge1.sum + edge2.sum;
          visited[next] = true;
          determined_nodes.push(next);
        }
      }
    }
  }
  return false;
}

bool DetermineValue(
    const vector<vector<Edge>>& edges, size_t var0, int* value_times_2) {
  vector<bool> visited(edges.size());
  vector<int> var0_plus(edges.size());
  queue<size_t> determined_nodes;
  for (const auto& edge : edges[var0]) {
    var0_plus[edge.to] = edge.sum;
    visited[edge.to] = true;
    determined_nodes.push(edge.to);
  }
  for (; !determined_nodes.empty(); determined_nodes.pop()) {
    size_t start = determined_nodes.front();
    if (start == var0) {
      *value_times_2 = var0_plus[var0];
      return true;
    }
    for (const auto& edge1 : edges[start]) {
      size_t via = edge1.to;
      for (const auto& edge2 : edges[via]) {
        size_t next = edge2.to;
        if (!visited[next]) {
          var0_plus[next] = var0_plus[start] - edge1.sum + edge2.sum;
          visited[next] = true;
          determined_nodes.push(next);
        }
      }
    }
  }
  return false;
}

int main(int argc, char* argv[]) {
  int T = 0;
  cin >> T;
  for (int t = 0; t < T; ++t) {
    // Processes evidences
    int N = 0;
    cin >> N;
    vector<vector<Edge>> edges;
    NameIndexDict dict;
    for (int n = 0; n < N; ++n) {
      // Parses input evidence
      string unused, var1, var2;
      getline(cin, unused);  // new line
      getline(cin, var1, '+');
      getline(cin, var2, '=');
      int sum = 0;
      cin >> sum;
      size_t index1 = dict.IndexOf(var1);
      size_t index2 = dict.IndexOf(var2);

      // Puts edge into graph
      edges.resize(dict.size());
      if (index1 == index2) {  // not edge
        edges[index1].push_back(Edge(index2, sum));
      } else {
        edges[index1].push_back(Edge(index2, sum));
        edges[index2].push_back(Edge(index1, sum));
      }
    }

    // Handles requests
    cout << "Case #" << (t+1) << ":" << endl;
    int Q = 0;
    cin >> Q;
    for (int q = 0; q < Q; ++q) {
      string unused, var1, var2;
      getline(cin, unused);  // new line
      getline(cin, var1, '+');
      cin >> var2;
      if (!dict.HasName(var1) || !dict.HasName(var2)) {
        continue;  // new variable, can't determine
      }
      size_t index1 = dict.IndexOf(var1);
      size_t index2 = dict.IndexOf(var2);
      int sum = 0, var1_times_2 = 0, var2_times_2 = 0;
      if (DetermineSum(edges, index1, index2, &sum)) {
        cout << var1 << "+" << var2 << "=" << sum << endl;
      } else if (DetermineValue(edges, index1, &var1_times_2) &&
                 DetermineValue(edges, index2, &var2_times_2)) {
        cout << var1 << "+" << var2 << "="
             << (var1_times_2 + var2_times_2) / 2  << endl;
      }
    }
  }
  return 0;
}
