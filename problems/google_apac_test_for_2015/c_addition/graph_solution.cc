// Idea (Peng Liu, Jie Mao, code by clangpp):
//
// 1. Evidence x+y=n is an edge of an (undirected) graph, with x and y as
// nodes, with n as weight.
//
// 2. In a connected subgraph, every node xi can be expressed by a base node x0
// in the form n1-x0 or n2+x0, and x0 can be expressed as, err, x0.
//
// 3. In the above connected subgraph, if any node xi can be expressed as n1-x0
// *and* n2+x0, then x0 can be determined as (n1-n2)/2.
//
// 4. Express all nodes in `count(subgraph)` base nodes, determine values of as
// many base nodes as possible.
//
// 5. When asking for a+b, plus their expressions.
// 5.1 If there's no variables, then a+b is determined.
// 5.2 If all remained (1 or 2) variables' value are determined, then a+b is
// determined.
// 5.3 Otherwise, a+b can't be determined.

#include <algorithm>
#include <cstddef>  // size_t
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

typedef size_t Variable;

struct Edge {
  Variable to;
  int sum;
  Edge(Variable to, int sum): to(to), sum(sum) {}
  Edge(): to(0), sum(0) {}
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

struct Expression {
  int constant;
  Variable variable;
  int coefficient;

  explicit Expression(
      int constant = 0, Variable variable = 0, int coefficient = 0)
      : constant(constant), variable(variable), coefficient(coefficient) {
  }

  bool operator != (const Expression& other) const {
    return !(constant == other.constant &&
             variable == other.variable &&
             coefficient == other.coefficient);
  }

  Expression operator + (const Expression& other) const {
    if (variable != other.variable) {
      cerr << "not addable variables: "
           << variable << " " << other.variable << endl;
      exit(1);
    }
    Expression result(*this);
    result.constant += other.constant;
    result.coefficient += other.coefficient;
    return result;
  }

  Expression operator - (const Expression& other) const {
    if (variable != other.variable) {
      cerr << "not minusable variables: "
           << variable << " " << other.variable << endl;
      exit(1);
    }
    Expression result(*this);
    result.constant -= other.constant;
    result.coefficient -= other.coefficient;
    return result;
  }

  friend ostream& operator << (ostream& os, const Expression& exp) {
    os << exp.constant;
    if (exp.coefficient > 0) {
      os << "+" << exp.coefficient << "*x_" << exp.variable;
    } else if (exp.coefficient < 0) {
      os << exp.coefficient << "*x_" << exp.variable;
    }
    return os;
  }
};

int main(int argc, char* argv[]) {
  int T = 0;
  cin >> T;
  for (int t = 0; t < T; ++t) {
    // Processes evidences, builds graph.
    int N = 0;
    cin >> N;
    vector<vector<Edge>> edges;
    NameIndexDict dict;
    for (int n = 0; n < N; ++n) {
      // Parses input edge
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

    // Preprocesses graph, calulates variable representation.
    size_t num_vars = edges.size();
    vector<Expression> exps(num_vars);
    vector<bool> determined(num_vars);
    vector<int> values_time_2(num_vars);
    // DFS, Expresses every node in connected subgraph by first visited node.
    vector<bool> visited(num_vars);
    stack<Variable> holder;
    for (int var = (int)num_vars-1; var >= 0; --var) {
      holder.push(var);
    }
    while (!holder.empty()) {
      Variable var = holder.top();
      holder.pop();
      if (!visited[var]) {
        exps[var] = Expression(0, var, 1);  // a = a
        visited[var] = true;
      }
      for (const auto& edge : edges[var]) {
        Expression sum_exp(edge.sum, exps[var].variable, 0);
        Expression exp = sum_exp - exps[var];  // b = n-a
        if (visited[edge.to] && exps[edge.to] != exp) {
          // Calculates first visited node value (constant).
          // NOTE(clangpp): Each variable can have and only have two forms,
          // n1+x, n2-x, so we can store 2*x=n1+n2 in case n1+n2 is odd number.
          determined[exp.variable] = true;
          values_time_2[exp.variable] =
              (exp.constant - exps[edge.to].constant) * 2 /
              (exps[edge.to].coefficient - exp.coefficient);
        }
        if (!visited[edge.to]) {
          exps[edge.to] = exp;
          visited[edge.to] = true;
          holder.push(edge.to);
        }
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
      const Expression& exp1 = exps[dict.IndexOf(var1)];
      const Expression& exp2 = exps[dict.IndexOf(var2)];
      int sum = exp1.constant + exp2.constant;
      if (exp1.variable == exp2.variable) {
        if (exp1.coefficient + exp2.coefficient == 0) {
          // sum += 0;
        } else if (determined[exp1.variable]) {
          sum += (exp1.coefficient + exp2.coefficient) *
              values_time_2[exp1.variable] / 2;
        } else {
          continue;
        }
      } else {
        if (determined[exp1.variable] && determined[exp2.variable]) {
          sum += ((exp1.coefficient * values_time_2[exp1.variable]) +
                  (exp2.coefficient * values_time_2[exp2.variable])) / 2;
        } else {
          continue;
        }
      }
      cout << var1 << "+" << var2 << "=" << sum << endl;
    }
  }
  return 0;
}
