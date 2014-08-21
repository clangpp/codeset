#include "matrix/matrix.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  Matrix<int> a, b, c;
  c = a - Matrix<int>();

  Matrix<int> d(2, 2, 3);
  const Matrix<int>& cd(d);
  cout << d[0][0] << " " << cd[0][0] << endl;
  return 0;
}
