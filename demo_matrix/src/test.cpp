#include "demo_matrix/demo_matrix.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  LOG << "`Matrix<double> my(3,3)`";
  Matrix<double> my(3,3);
  my(2,2) = 2.12;
  cout << endl << endl;

  LOG << "`cout << my`";
  cout << my;
  cout << endl << endl;

  LOG << "`Matrix <double> aaa(my)`";
  Matrix<double> aaa(my);
  cout << endl << endl;

  LOG << "Constructing `bbb`";
  Matrix<double> bbb(my);
  cout << endl << endl;

  LOG << "`bbb = aaa + my + bbb`";
  bbb = aaa + my + bbb;
  cout << endl << endl;

  LOG << "`bbb = aaa + (my + bbb)`";
  bbb = aaa + (my + bbb);
  cout << endl << endl;

  LOG << "`Matrix<double> ccc = aaa + my + bbb`";
  Matrix<double> ccc = aaa + my + bbb;
  cout << endl << endl;

  LOG << "`Matrix<double> ddd=aaa`";
  Matrix<double> ddd=aaa;
  cout << endl << endl;

  LOG << "`ddd = bbb`";
  ddd = bbb;
  cout << endl << endl;

  LOG << "`Matrix<double> eee(aaa + bbb)`";
  Matrix<double> eee(aaa + bbb);
  cout << endl << endl;

  return 0;
}
