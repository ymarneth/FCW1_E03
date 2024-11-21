// TapeStuff.cpp:                                         HDO, 2006-2019
// -------------
// TapeSymbol, an alias for char, is a symbol on the tape of an automaton.
// Tape, an alias for char*, is a sequences of TapeSymbols.
// TapeSymbolSet represents a set of TapeSymbols.
//======================================================================

#include <iostream>
#include <set>
#include <string>

using namespace std;

#include "TapeStuff.h"


string stringOf(TapeSymbol tSy) {
  switch (tSy) {
    case eot:
      return "eot";
    case eps:
      return "eps";
    default:
      return string(1, tSy);
  } // switch
} // stringOf


TapeSymbolSet::TapeSymbolSet(TapeSymbol tSy) {
  insert(tSy);
} // TapeSymbolSet::TapeSymbolSet

TapeSymbolSet::TapeSymbolSet(const string &str) {
  for (TapeSymbol tSy: str)
    insert(tSy);
} // TapeSymbolSet::TapeSymbolSet

TapeSymbolSet::TapeSymbolSet(initializer_list<char> il) {
  for (TapeSymbol tSy: il)
    insert(tSy);
} // TapeSymbolSet::TapeSymbolSet


bool TapeSymbolSet::contains(TapeSymbol tSy) const {
  return find(tSy) != end();
} // TapeSymbolSet::contains


ostream &operator<<(ostream &os, const TapeSymbolSet &tSySet) {
  cout << "{";
  bool first = true;
  for (TapeSymbol tSy: tSySet) {
    if (!first)
      cout << ", ";
    cout << stringOf(tSy);
    first = false;
  } // for
  cout << "}";
  return os;
} // operator<<


// === test ============================================================

#if 0

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  cout << "START: TapeStuff" << endl;
  cout << endl;


  cout << "testing TapeSymbol: " << endl;
  cout << endl;

  TapeSymbol tSy = 'x';
  cout << "tSy = " << tSy << endl;
  cout << endl;

  cout << endl;
  cout << "testing TapeSymbolSet: " << endl;
  cout << endl;

  TapeSymbolSet emptySet;
  cout << "emptySet     = " << emptySet << endl;

  TapeSymbolSet singletonSet('s');
  cout << "singletonSet = " << singletonSet << endl;

  TapeSymbolSet abcSet1("abc"); // or: abcSet2(string("abc"))
  cout << "abcSet1   = " << abcSet1 << endl;

  TapeSymbolSet abcSet2({'a', 'b', 'c'}); // or: abcSet2{'a', 'b', 'c'}
  cout << "abcSet2   = " << abcSet2    << endl;

  TapeSymbolSet copySet(abcSet1);
  cout << "copySet   = " << copySet << endl;

  TapeSymbolSet assignSet;
  assignSet = abcSet2;
  cout << "assignSet = " << assignSet << endl;

  cout << "abcSet1.contains('a') = " << abcSet1.contains('a') << endl;

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


// end of TapeSymbolSet.h
//======================================================================
