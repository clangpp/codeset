// STATUS: logic error:
//  if a+c=10, b+c=10, then c can be either 10-a or 10-b.
#include <algorithm>
#include <cstddef>  // size_t
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

typedef size_t Variable;

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
  vector<Variable> variables;
  vector<int> coefficients;

  Expression(int constant, Variable variable, int coefficient)
      : constant(constant), variables(1, variable),
        coefficients(1, coefficient) {
  }
  explicit Expression(int constant): constant(constant) {}
  Expression(): constant(0) {}

  bool operator == (const Expression& other) const {
    return (constant == other.constant &&
            variables == other.variables &&
            coefficients == other.coefficients);
  }

  bool operator != (const Expression& other) const {
    return !(*this == other);
  }

  Expression operator - () const {
    Expression result(*this);
    result.constant = -result.constant;
    for (auto& coefficient : result.coefficients) {
      coefficient = -coefficient;
    }
    return result;
  }

  Expression operator + (const Expression& other) const {
    Expression result;
    result.constant = constant + other.constant;
    for (size_t i = 0, j = 0;
         i < variables.size() || j < other.variables.size(); ) {
      if (i >= variables.size()) {
        for (; j < other.variables.size(); ++j) {
          result.variables.push_back(other.variables[j]);
          result.coefficients.push_back(other.coefficients[j]);
        }
      } else if (j >= other.variables.size()) {
        for (; i < variables.size(); ++i) {
          result.variables.push_back(variables[i]);
          result.coefficients.push_back(coefficients[i]);
        }
      } else {
        if (variables[i] < other.variables[j]) {
          result.variables.push_back(variables[i]);
          result.coefficients.push_back(coefficients[i]);
          ++i;
        } else if (variables[i] > other.variables[j]) {
          result.variables.push_back(other.variables[j]);
          result.coefficients.push_back(other.coefficients[j]);
          ++j;
        } else {  // variables[i] == other.variables[j]
          int coefficient = coefficients[i] + other.coefficients[j];
          if (coefficient != 0) {
            result.variables.push_back(variables[i]);
            result.coefficients.push_back(coefficient);
          }
          ++i;
          ++j;
        }
      }
    }
    return result;
  }

  Expression operator - (const Expression& other) const {
    return *this + (-other);
  }

  friend ostream& operator << (ostream& os, const Expression& exp) {
    os << exp.constant;
    for (size_t i = 0; i < exp.variables.size(); ++i) {
      if (exp.coefficients[i] > 0) {
        os << "+";
      }
      os << exp.coefficients[i] << "*x_" << exp.variables[i];
    }
    return os;
  }
};

struct Equation {
  Variable var1;
  Variable var2;
  int sum;
  Equation(Variable var1, Variable var2, int sum)
      : var1(var1), var2(var2), sum(sum) {}
  Equation(): var1(0), var2(0), sum(0) {}

  bool operator < (const Equation& other) const {
    if (var1 != other.var1) {
      return var1 < other.var1;
    }
    if (var2 != other.var2) {
      return var2 < other.var2;
    }
    return sum < other.sum;
  }
};

int main(int argc, char* argv[]) {
  int T = 0;
  cin >> T;
  for (int t = 0; t < T; ++t) {
    // Reads and stores expressions.
    int N = 0;
    cin >> N;
    set<Equation> equation_set;
    NameIndexDict dict;
    for (int n = 0; n < N; ++n) {
      string unused, var1, var2;
      getline(cin, unused);  // new line
      getline(cin, var1, '+');
      getline(cin, var2, '=');
      int sum = 0;
      cin >> sum;
      size_t index1 = dict.IndexOf(var1);
      size_t index2 = dict.IndexOf(var2);

      equation_set.insert(
          Equation(min(index1, index2), max(index1, index2), sum));
    }

    // Calulates variable representation.
    size_t num_vars = dict.size();
    size_t num_equations = equations_set.size();
    vector<Expression> exps(num_vars);
    vector<bool> determined(num_vars);
    vector<int> values(num_vars);
    vector<bool> visited(num_vars);
    for (const auto& equation : equation_set) {
      if (!visited[equation.var1]) {
        exps[equation.var1] = Expression(0, equation.var1, 1);  // a = a
        visited[equation.var1] = true;
      }
      Expression exp = Expression(equation.sum) - exps[equation.var1];
      clog << exps[equation.var1] << " " << exps[equation.var2] << " " << exp << endl;
      if (visited[equation.var2] && exps[equation.var2] != exp) {
        // Calculates first visited variable value.
        determined[exp.variables.front()] = true;
        values[exp.variables.front()] =
            (exp.constant - exps[equation.var2].constant) /
            (exps[equation.var2].coefficients.front() -
             exp.coefficients.front());
      }
      exps[equation.var2] = exp;
      visited[equation.var2] = true;
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
      Expression sum_exp = exps[index1] + exps[index2];
      int sum = sum_exp.constant;
      bool sum_determined = true;
      for (size_t i = 0; i < sum_exp.variables.size(); ++i) {
        if (determined[sum_exp.variables[i]]) {
          sum += values[sum_exp.variables[i]] * sum_exp.coefficients[i];
        } else {
          sum_determined = false;
          break;
        }
      }
      if (sum_determined) {
        cout << var1 << "+" << var2 << "=" << sum << endl;
      }
    }
  }
  return 0;
}
