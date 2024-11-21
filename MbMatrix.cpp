// MbMatrix.h:                                                 HDO, 2019
// ----------
// Map-based Matrix (MbMatrix) is a generic class for 2D arrays where
// *  the two indices can be of any type providing operator< and
// *  operator[][] is applicable even to const MbMatrix objects.
// A  row   in an MbMatrix is represented by a map-based vector (MbVector).
// An entry in an MbMatrix can be seen as Triple consisting of
//   two indices and a value: (i, j, v).
//======================================================================

#include <map>
#include <iostream>
#include <string>

using namespace std;

#include "MbMatrix.h"


// implementations for the three generic classes
//   Triple, MbVector and MbMatrix are in header


// === test ============================================================

#if 0

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  cout << "START: MbMatrix" << endl;
  cout << endl;


  cout << "testing MbVector:" << endl;
  cout << endl;

        MbVector<int, int>  v;
  const MbVector<int, int> cv;

  v[1] = 1;
  cout << v[1] << endl;
  v[1] = 2;
  cout << v[1] << endl;
  cout << endl;

  // cv[1] = 1; // invalid assignment to const MbVector
  cout << cv[0] << endl;
  cout << endl;

  cout << "testing MbMatrix:" << endl;
  cout << endl;

        MbMatrix<string, string, string>  m;
  const MbMatrix<string, string, string> cm;

  m[ "0"]["0"] = "0:0";
  m["17"]["4"] = "17:4";

  cout << "m[ 0][ 0] = " << m["17"][ "4"] << endl;
  cout << "m[17][ 4] = " << m["17"][ "4"] << endl;
  cout << "m[99][99] = " << m["99"]["99"] << endl; // inserts new element ""

  cout << endl;
  cout << "m = " << endl;
  cout << m;

  cout << endl;
  cout << "m.elements() = " << endl;
  for (auto &e: m.elements())
    cout << e << endl;

  cout << endl;
  // cm["17"]["4"] = "17:4"; // invalid assignment to const MbMatrix
  cout << "cm[17][ 4] = " << endl;
  cout << cm["17"][ "4"] << endl;

  cout << endl;
  cout << "END" << endl;

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main

#endif

// end of MbMatrix.cpp
//======================================================================

