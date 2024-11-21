// DeltaStuff.h:                                               HDO, 2019
// ------------
// Transition functions (delta) for finite automata, where
//   delta: State x TapeSymbol -> either State or StateSet.
// Delta is the generic base class for
// * DDelta used in     deterministic finite automata (DFA) and
// * NDelta used in non-deterministic finite automata (NFA).
//======================================================================

#include <map>
#include <stdexcept>

using namespace std;

#include "ObjectCounter.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "DeltaStuff.h"
#include "MbMatrix.h"


// --- function to transform DDelta to NDelta ---

NDelta nDeltaOf(const DDelta &dDelta) {
  NDelta nDelta;
  for (const auto &t: dDelta.transitions())
    nDelta[t.src][t.tSy] = StateSet(t.dest);
  return nDelta;
} // nDeltaOf


// --- function to transform NDelta to DDelta ---

DDelta dDeltaOf(const NDelta &nDelta) {
  DDelta dDelta;
  for (const auto &t: nDelta.transitions())
    if (t.dest.size() == 1)
      dDelta[t.src][t.tSy] = t.dest.anyElement(); // the one and only
    else // (t.dest.size() == 0) or (t.dest.size() > 1)
      throw domain_error(string("invalid transition (") +
            t.src + ", " + stringOf(t.tSy) +
            ") -> set of states with cardinality != 1");
  return dDelta;
} // dDeltaOf


// === test ============================================================

#if 0

#include "TapeStuff.cpp"
#include "StateStuff.cpp"

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  cout << "START: DeltaStuff" << endl;
  cout << endl;

  cout << "tests for DDelta:" << endl << endl;
  DDelta  dd;

  dd["S"]['0'] = "A";
  cout << "dd[S][0] = " << dd["S"]['0'] << endl;

  dd["S"]['0'] = "B"; // overwerites "A"
  cout << "dd[S][0] = " << dd["S"]['0'] << endl;

  cout << "dd[S][X] = " << dd["S"]['X'] << endl; // inserts "" for (S, X)
  cout << endl;

  cout << "dd:" << endl;
  cout << dd << endl;

  cout << "dd.transitions():" << endl;
  for (const auto &t: dd.transitions())
    cout << t << endl;
  cout << endl;

  cout << "tests for NDelta:" << endl << endl;
  NDelta nd;

  nd["S"]['0'] = StateSet("A");
  cout << "nd[S][0] = " << nd["S"]['0'] << endl;

  cout << "nd[S][X] = " << nd["S"]['X'] << endl; // inserts {} for (S, X)
  cout << endl;

  cout << "nd 1.:" << endl;
  cout << nd << endl;

  nd["S"]['X'] = StateSet("Z");
  cout << "nd 2.:" << endl;
  cout << nd << endl;

  cout << "dd2 = dDeltaOf(nd):" << endl;
  Delta<State> dd2 = dDeltaOf(nd);
  cout << dd2 << endl;

  cout << "nd2 = nDeltaOf(dd):" << endl;
  Delta<StateSet> nd2 = nDeltaOf(dd);
  cout << nd2 << endl;

  nd["S"]['X'] = StateSet();
  cout << "nd 3.:" << endl;
  cout << nd << endl;

  try { // error: nd has a trans. to an empty set
    cout << "dd3:" << endl;
    Delta<State> dd3 = dDeltaOf(nd);
  } catch (const exception &e) {
    cerr << "EXCEPTION: " << e.what() << endl;
  } // catch

  cout << endl;
  nd["S"]['X'] = StateSet({"S", "X"});
  cout << "nd 4.:" << endl;
  cout << nd << endl;

  try { // error: nd now has a trans. to a set with two states
    cout << "dd4:" << endl;
    Delta<State> dd4 = dDeltaOf(nd);
  } catch (const exception &e) {
    cerr << "EXCEPTION: " << e.what() << endl;
  } // catch

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


// end of DeltaStuff.cpp
//======================================================================

