// StateStuff.h:                                          HDO, 2006-2019
// ------------
// State, an alias for std::string represents the state of an automaton.
// So std::string, char[] or char* are state(names).
// StateSet represents a set of States.
//======================================================================

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

#include "StateStuff.h"


bool defined(const State    &s) {
  return s.size() > 0;
} // defined

bool defined(const StateSet &ss) {
  return ss.size() > 0;
} // defined

bool empty  (const StateSet &ss) {
  return ss.empty();
} // empty

int maxStateNameLen(const StateSet & ss) {
  int mnl = 0;             // max. name length
  for (const State &s: ss)
    mnl = max<int>(mnl, (int)s.length());
  return mnl;
} // maxStateNameLen

StateSet stateSetOf(const State    &s) {
  StateSet ss(s);
  return ss;
} // stateSetOf

State    stateOf   (const StateSet &ss) {
  return ss.stateOf();
} // stateOf


// --- implementation of class StateSet ---

StateSet::StateSet(const State &s) {
  Base::insert(s);
} // StateSet::StateSet


StateSet::StateSet(initializer_list<State> il) {
  for (const State &s: il)
    Base::insert(s);
} // StateSet::StateSet


bool StateSet::contains(const State &s) const {
  return find(s) != end();
} // StateSet::contains


void StateSet::insert(const StateSet &ss) {
  for (const State &s: ss)
    insert(s);
} // StateSet::insert


void StateSet::erase(const StateSet &ss) {
  for (const State &s: ss)
    Base::erase(s);
} // StateSet::erase


State StateSet::anyElement() const {
  return *begin();
} // StateSet::anyElement


State StateSet::stateOf() const {
  if (this->size() == 1)
    return this->anyElement(); // the one and only
  State sNew;
  bool first = true;
  for (const State &s: *this) {
    if (!first)
      sNew.append("+");
    if (s.find("+") != string::npos)
      sNew.append("(" + s + ")");
    else
      sNew.append(s);
    first = false;
  } // for
  return sNew;
} // StateSet::stateOf


StateSet operator^(const StateSet &ss1, const StateSet &ss2) {
  StateSet result;
  set_intersection(ss1.begin(), ss1.end(),
                   ss2.begin(), ss2.end(),
                   inserter(result, result.begin()));
  return result;
} // operator^

StateSet operator-(const StateSet &ss1, const StateSet &ss2) {
  StateSet result(ss1);
  result.erase(ss2);
  return result;
} // operator-


ostream &operator<<(ostream &os, const StateSet &ss) {
  os << "{";
  bool first = true;
  for (const State &s: ss) {
    if (!first)
      os << ", ";
    os << s;
    first = false;
  } // for
  os << "}";
  return os;
} // operator<<


// --- implementation of class SetOfStateSets ---

SetOfStateSets::SetOfStateSets(const StateSet &ss) {
  insert(ss);
} // SetOfStateSets


bool SetOfStateSets::contains(const StateSet &ss) const {
  return find(ss) != end();
} // SetOfStateSets::contains


StateSet SetOfStateSets::anyElement() const {
  return *begin();
} // SetOfStateSets::anyElement


ostream &operator<<(ostream &os, const SetOfStateSets &soss) {
  os << "{ ";
  bool first = true;
  for (const StateSet &ss: soss) {
    if (!first)
      os << ", ";
    os << ss;
    first = false;
  } // for
  os << " }";
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

  cout << "START: StateStuff" << endl;
  cout << endl;

  cout << "testing State:" << endl;
  cout << endl;

  cout << "undefState       = \"" << State()   << "\"" <<endl;
  cout << endl;

  cout << endl;
  cout << "testing for StateSet:" << endl;
  cout << endl;

  cout << "undefStateSet    = "   << StateSet() << endl;

  StateSet emptySet;
  cout << "emptySet         = " << emptySet << endl;

  StateSet aSet("a");
  cout << "aSet             = " << aSet << endl;
  cout << "aSet.asState()   = " << aSet.stateOf() << endl;

  StateSet nonEmptySet; // = aset; or:
  nonEmptySet = aSet;
  cout << "nonEmptySet      = " << nonEmptySet << endl;

  StateSet abcSet( {"a", "b", "c"} );
  cout << "abcSet           = " << abcSet   << endl;
  cout << "abcSet.asState() = " << abcSet.stateOf() << endl;

  cout << "abcSet.contains(\"a\") = " <<
           abcSet.contains( "a") << endl;
  cout << "abcSet.contains(\"x\") = " <<
           abcSet.contains( "x") << endl;

  abcSet.insert("a");
  cout << "abcSet   = " << abcSet   << endl;
  abcSet.insert(StateSet({"a" , "x"}));
  StateSet abcxSet = abcSet;
  abcSet.erase("x");
  cout << "abcxSet  = " << abcxSet  << endl;

  cout << "inters.  = " << (abcSet  ^ abcxSet) << endl;
  cout << "differ.  = " << (abcxSet - abcSet ) << endl;

  cout << endl;

  cout << "tests for SetOfStateSets:" << endl << endl;

  SetOfStateSets emptySetOfSets;
  cout << "emptySetOfSets  = " << emptySetOfSets << endl;
  SetOfStateSets setOfOneSet(abcSet);
  cout << "setOfOneSet     = " << setOfOneSet    << endl;

  SetOfStateSets abcSetOfSets;
  abcSetOfSets.insert(StateSet({ "a" }));
  abcSetOfSets.insert(StateSet({ "b", "c" }));
  cout << "abcSetOfSets = " << abcSetOfSets << endl;

  cout << "abcSetOfSets.contains(StateSet(\"{  a  }\")) = " <<
           abcSetOfSets.contains(StateSet(  { "a" })) << endl;
  cout << "abcSetOfSets.contains(StateSet(\"{  c  }\")) = " <<
           abcSetOfSets.contains(StateSet(  { "c" })) << endl;

  cout << "abcSetOfSets = " << abcSetOfSets   << endl;

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


// end of StateNameStuff.cpp
//======================================================================
