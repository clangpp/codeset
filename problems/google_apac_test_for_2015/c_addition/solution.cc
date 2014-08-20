// STATUS: small input passed, large input runtime too long (hours and not finished yet).
//
// my idea:
//  1. Evidence x+y=n is an edge of a graph, with x and y as nodes, and n as weight.
//  2. If there's evidence x+x=n, then x is determined (n/2), and thus all nodes connected with x are determined.
//  3. If there's circle (x to x) which has odd number edges (n1, n2, n3, ..., nt), then x is determined as (n1-n2+n3-...+nt)/2, thus all nodes connected with x are determined.
//  4. If x to y has odd number edges (n1, n2, n3, ..., nt), then x+y is determined as n1-n2+n3-...+nt.
//  5. Otherwise, x+y can't be determined.

#include <cstddef>  // for size_t
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>  // pair
#include <vector>

using namespace std;

struct Variable {
  bool determined = false;
  int value = 0;
};

struct Evidence {
  size_t var1 = 0;
  size_t var2 = 0;
  int sum = 0;
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

void SpanDeterminedNodes(
    const vector<vector<Evidence>>& outlinks,
    const vector<vector<Evidence>>& inlinks,
    size_t determined_node,
    vector<Variable>* nodes) {
  queue<size_t> determined_nodes;
  for (determined_nodes.push(determined_node);
      !determined_nodes.empty();
      determined_nodes.pop()) {
    size_t node = determined_nodes.front();
    for (const auto& evidence : outlinks[node]) {
      if (!(*nodes)[evidence.var2].determined) {
        (*nodes)[evidence.var2].value = evidence.sum - (*nodes)[node].value;
        (*nodes)[evidence.var2].determined = true;
        determined_nodes.push(evidence.var2);
      }
    }
    for (const auto& evidence : inlinks[node]) {
      if (!(*nodes)[evidence.var1].determined) {
        (*nodes)[evidence.var1].value = evidence.sum - (*nodes)[node].value;
        (*nodes)[evidence.var1].determined = true;
        determined_nodes.push(evidence.var1);
      }
    }
  }
}

void SpanDeterminedNodes(
    const vector<vector<Evidence>>& outlinks,
    const vector<vector<Evidence>>& inlinks,
    vector<Variable>* nodes) {
  for (size_t node = 0; node < nodes->size(); ++node) {
    if ((*nodes)[node].determined) {
      SpanDeterminedNodes(outlinks, inlinks, node, nodes);
    }
  }
}

bool DetermineCircle(
    const vector<vector<Evidence>>& outlinks,
    const vector<vector<Evidence>>& inlinks,
    size_t var1,
    size_t curr,
    size_t sum_prev_curr,
    vector<bool>* visited,
    vector<pair<size_t, int>>* path,
    vector<Variable>* nodes) {
  if ((*visited)[curr]) {
    return false;
  }
  (*visited)[curr] = true;
  path->push_back(make_pair(curr, sum_prev_curr));
  clog << path->size() << endl;
  if (path->back().first == var1) {
    if (path->size() % 2 == 1) {  // odd edges
      int sign = 1;
      int sum = 0;
      for (const auto& step : *path) {
        sum += sign * step.second;
        sign *= -1;
      }
      (*nodes)[var1].value = sum / 2;
      (*nodes)[var1].determined = true;
      return true;
    } else {
      path->pop_back();
      (*visited)[curr] = false;
      return false;
    }
  }
  for (const auto& evidence : outlinks[curr]) {
    if (DetermineCircle(outlinks, inlinks, var1, evidence.var2, evidence.sum,
                        visited, path, nodes)) {
      return true;
    }
  }
  for (const auto& evidence : inlinks[curr]) {
    if (DetermineCircle(outlinks, inlinks, var1, evidence.var1, evidence.sum,
                        visited, path, nodes)) {
      return true;
    }
  }
  path->pop_back();
  (*visited)[curr] = false;
  return false;
}

void DetermineCircles(
    const vector<vector<Evidence>>& outlinks,
    const vector<vector<Evidence>>& inlinks,
    vector<Variable>* nodes) {
  for (size_t node = 0; node < nodes->size(); ++node) {
    if (!(*nodes)[node].determined) {
      vector<bool> visited(nodes->size());
      vector<pair<size_t, int>> path;
      bool circle_determined = false;
      for (const auto& evidence : outlinks[node]) {
        if (circle_determined) break;
        circle_determined = DetermineCircle(
            outlinks, inlinks, node, evidence.var2, evidence.sum, &visited,
            &path, nodes);
      }
      for (const auto& evidence : inlinks[node]) {
        if (circle_determined) break;
        circle_determined = DetermineCircle(
            outlinks, inlinks, node, evidence.var1, evidence.sum, &visited,
            &path, nodes);
      }
      if (circle_determined) {
        SpanDeterminedNodes(outlinks, inlinks, node, nodes);
      }
    }
  }
}

bool DetermineSum(
    const vector<Variable>& nodes,
    const vector<vector<Evidence>>& outlinks,
    const vector<vector<Evidence>>& inlinks,
    size_t curr,
    size_t var2,
    int sum_prev_curr,
    vector<bool>* visited,
    vector<pair<size_t, int>>* path,
    int* sum) {
  if ((*visited)[curr]) {
    return false;
  }
  (*visited)[curr] = true;
  path->push_back(make_pair(curr, sum_prev_curr));
  if (path->back().first == var2) {
    if (path->size() % 2 == 0) {  // odd edges, even nodes
      int sign = 1;
      *sum = 0;
      for (size_t i = 1; i < path->size(); ++i) {
        *sum += sign * (*path)[i].second;
        sign *= -1;
      }
      return true;
    } else {
      path->pop_back();
      (*visited)[curr] = false;
      return false;
    }
  }
  for (const auto& evidence : outlinks[curr]) {
    if (DetermineSum(nodes, outlinks, inlinks, evidence.var2, var2,
                     evidence.sum, visited, path, sum)) {
      return true;
    }
  }
  for (const auto& evidence : inlinks[curr]) {
    if (DetermineSum(nodes, outlinks, inlinks, evidence.var1, var2,
                     evidence.sum, visited, path, sum)) {
      return true;
    }
  }
  path->pop_back();
  (*visited)[curr] = false;
  return false;
}

bool DetermineSum(
    const vector<Variable>& nodes,
    const vector<vector<Evidence>>& outlinks,
    const vector<vector<Evidence>>& inlinks,
    size_t var1,
    size_t var2,
    int* sum) {
  vector<bool> visited(nodes.size());
  vector<pair<size_t, int>> path;
  return DetermineSum(
      nodes, outlinks, inlinks, var1, var2, 0, &visited, &path, sum);
}

int main(int argc, char* argv[]) {
  int T = 0;
  cin >> T;
  for (int t = 0; t < T; ++t) {
    // Processes evidences
    int N = 0;
    cin >> N;
    vector<Variable> nodes;
    vector<vector<Evidence>> outlinks, inlinks;
    NameIndexDict dict;
    for (int n = 0; n < N; ++n) {
      // Parses input evidence
      string unused, var1, var2;
      getline(cin, unused);  // new line
      getline(cin, var1, '+');
      getline(cin, var2, '=');
      int sum = 0;
      cin >> sum;
      clog << var1 << " " << var2 << " " << sum << endl;
      Evidence evidence;
      evidence.var1 = dict.IndexOf(var1);
      evidence.var2 = dict.IndexOf(var2);
      evidence.sum = sum;

      // Puts edge into graph
      nodes.resize(dict.size());
      outlinks.resize(dict.size());
      inlinks.resize(dict.size());
      if (evidence.var1 == evidence.var2) {  // not edge
        nodes[evidence.var1].determined = true;
        nodes[evidence.var1].value = evidence.sum / 2;
      } else {
        outlinks[evidence.var1].push_back(evidence);
        inlinks[evidence.var2].push_back(evidence);
      }
    }

    // Tries to determine as many nodes
    clog << "SpanDeterminedNodes()" << endl;
    SpanDeterminedNodes(outlinks, inlinks, &nodes);
    clog << "DetermineCircles()" << endl;
    DetermineCircles(outlinks, inlinks, &nodes);

    // Handles requests
    cout << "Case #" << (t+1) << ":" << endl;
    int Q = 0;
    cin >> Q;
    for (int q = 0; q < Q; ++q) {
      string unused, var1, var2;
      getline(cin, unused);  // new line
      getline(cin, var1, '+');
      cin >> var2;
      clog << var1 << " " << var2 << " ?" << endl;
      if (!dict.HasName(var1) || !dict.HasName(var2)) {
        continue;  // new variable, can't determine
      }
      size_t index1 = dict.IndexOf(var1);
      size_t index2 = dict.IndexOf(var2);
      if (nodes[index1].determined && nodes[index1].determined) {
        cout << var1 << "+" << var2 << "="
            << (nodes[index1].value + nodes[index2].value) << endl;
      } else if (!nodes[index1].determined && !nodes[index2].determined) {
        int sum = 0;
        if (DetermineSum(nodes, outlinks, inlinks, index1, index2, &sum)) {
          cout << var1 << "+" << var2 << "=" << sum << endl;
        }
      } else {
        // exactly one determined, meaning the other isn't connected
        // can't solve result, ignores this request.
      }
    }
  }
  return 0;
}
