// FABuilder.h:                                                HDO, 2019
// -----------
// FABuilder implements the builder pattern to build
// deterministic and non-deterministic finite automata, i.e.
// objects of classes DFA or NFA (both derived from FA)
//======================================================================

#include <cstring>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

using namespace std;

#include "ObjectCounter.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "FA.h"
#include "DFA.h"
#include "NFA.h"
#include "FABuilder.h"

#ifdef MEALY_DFA
#include "MealyDFA.h"
#endif

void FABuilder::checkStates() const {
  TapeSymbolSet VwithEps = this->V; // a copy to allow changes in ...
  VwithEps.insert(eps); // ... trick to respect epsilon transitions
  if (delta.empty())
    throw logic_error("FABuilder: transition function delta is empty");
  // as delta.size() > 0 there are transitions, so
  //   S is not empty but
  //   V may be empty as "->() S -> eps S" is legal
  if (!defined(s1))
    throw logic_error("FABuilder: no start state s1 defined");
  if (F.size() == 0)
    throw logic_error("FABuilder: no end state(s) F defined");
  if (delta.find(s1) == delta.end())
    throw logic_error("FABuilder: start state is not in delta's domain");
  StateSet reachableStates(s1); // start state is reachable
  size_t oldSize, newSize = 1;
  do {
    oldSize = newSize;
    for (const State &s: reachableStates)
      for (TapeSymbol tSy: VwithEps) {
         const StateSet &destSet = delta[s][tSy];
         if (defined(destSet))
           reachableStates.insert(destSet);
       } // for
    newSize = reachableStates.size();
  } while (newSize > oldSize);
  const StateSet unreachableStates = (S - reachableStates);
  if (unreachableStates.size() > 0)
    cout << "WARNING in FABuilder: the state(s) in set " <<
            unreachableStates << " cannot be reached" << endl;
} // FABuilder::checkStates


static string initMessageOf(int lnr, const string &msg) {
  return "error in line " + to_string(lnr) + ": " + msg;
} // initMessageOf

void FABuilder::initFromStream(istream &is) {
  string line, sy, state, arrowSy, destState;
  bool isStartState, isFinalState;
  int  lnr = 0;
  while (!is.eof()) {
    line = "";
    getline(is, line);
    if (is.fail())
      break;                // end of input stream
    lnr++;
    istringstream ls(line); // line stream
    sy = "";
    ls >> sy;
    if (sy == "" || sy.substr(0, 2) == "//") // skip empty or comment line
      continue;
    isStartState = (sy == "->");
    isFinalState = (sy == "()");
    if (sy == "->()")      // ->() indicates start state being final
      isStartState = isFinalState = true;
    if (isStartState) {
      if (!isFinalState) {
        ls >> state;
        if (state == "()") { // -> () also allowed for start & final state
          isFinalState = true;
          ls >> state;
        } // if
      } else // isStartState && isFinalState
        ls >> state;
    } else if (isFinalState)
      ls >> state;
    else // sy is already a state name
      state = sy;
    ls >> arrowSy;
    if (arrowSy != "->")
      throw runtime_error(initMessageOf(lnr, "-> missing"));
    if (isStartState) {
      if (s1 != "")
        throw runtime_error(initMessageOf(lnr, "redef. of start state"));
      setStartState(state);
    } // if
    if (isFinalState)
      addFinalState(state);
    while (!ls.eof()) {
      sy = "";             // necessary for correct end of line handling
      ls >> sy;            // terminal or |
      if (sy == "")
        break;             // end of line
      if (sy == "|") {
        sy = "";           // see above
        ls >> sy;          // terminal
        if (sy == "")
          throw runtime_error(initMessageOf(lnr,
                "no symbol for transition from " + state));
      } // if
      if ((sy[0] == eps) || (sy == "eps"))
        sy = eps;
      else if (sy.length() > 1)
        throw runtime_error(initMessageOf(lnr,
              "tape symbol " + sy + " too long" ));
      ls >> destState;
      if (destState == "" || destState == "|")
        throw runtime_error(initMessageOf(lnr,
              "dest. state missing with symbol " + stringOf(sy[0])));
      addTransition(state, sy[0], destState);
     } // while
  } // while
  if (s1 == "")
    throw runtime_error(initMessageOf(lnr,
          "no start state defined"));
  if (F.size() == 0)
    throw runtime_error(initMessageOf(lnr,
          "no final state(s) defined"));
} // FABuilder::initFromStream


FABuilder::FABuilder(const string &fileName) {
  ifstream ifs(fileName);
  if (!ifs.good())
    throw invalid_argument("file \"" + fileName + "\" not found");
  initFromStream(ifs);
} // FABuilder::FABuilder

FABuilder::FABuilder(const char *str) {
  istringstream iss(string(str, strlen(str)));
  initFromStream(iss);
} // FABuilder::FABuilder


FABuilder &FABuilder::setStartState(const State &s) {
  if (s1 != State())
    throw runtime_error("redefinition of start state");
  s1 = s;
  S.insert(s);
  return *this;
} // FABuilder::setStartState


FABuilder &FABuilder::addFinalState(const State &s) {
  F.insert(s);
  S.insert(s); // final state is a state, too
  return *this;
} // FABuilder::addFinalState

FABuilder &FABuilder::addFinalStates(const StateSet &sSet) {
  for (const State &s: sSet)
    addFinalState(s);
  return *this;
} // FABuilder::addFinalStates

FABuilder &FABuilder::addFinalStates(initializer_list<State> il) {
  for (const State &s: il)
    addFinalState(s);
  return *this;
} // FABuilder::addFinalStates


FABuilder &FABuilder::addTransition(
           const State &src, TapeSymbol tSy, const State &dest) {
  S.insert(src);
  if (tSy != eps) // epsilon is no tape symbol
    V.insert(tSy);
  S.insert(dest);
  delta[src][tSy].insert(dest);
  return *this;
} // FABuilder::addTransition

FABuilder &FABuilder::addTransition(
           const State &src, TapeSymbol tSy, const StateSet &destSet) {
  for (const State &dest: destSet)
    addTransition(src, tSy, dest);
  return *this;
} // FABuilder::addTransition

FABuilder &FABuilder::addTransition(
           const State &src, TapeSymbol tSy, initializer_list<State> il) {
  for (const State &dest: il)
    addTransition(src, tSy, dest);
  return *this;
} // FABuilder::addTransition

FABuilder &FABuilder::setMooreLambda(const map<State, char> &mooreLambda) {
  for (const auto& s : S) {
    if (mooreLambda.find(s) == mooreLambda.end())
      throw runtime_error("missing lambda value for state " + s);
  }
  this->mooreLambda = mooreLambda;
  return *this;
} // FABuilder::setMooreLambda

bool FABuilder::representsDFA() const {
  for (const auto &t: delta.transitions())
    if ( (t.tSy == eps) || // epsilon transition
         (t.dest.size() > 1))  // transition with several dest. states
      return false;
  return true;
} // FABuilder::representsDFA


FA *FABuilder::buildFA() const {
  checkStates();
  if (representsDFA())
    return new DFA(S, V, s1, F, dDeltaOf(delta));
  else
    return new NFA(S, V, s1, F, delta);
} // FABuilder::buildFA

DFA *FABuilder::buildDFA() const {
  if (!representsDFA())
    throw domain_error("cannot build DFA, builder's delta represents a NFA");
  checkStates();
  return new DFA(S, V, s1, F, dDeltaOf(delta));
} // FABuilder::buildDFA

NFA *FABuilder::buildNFA() const {
  checkStates();
  return new NFA(S, V, s1, F, delta);
} // FABuilder::buildNFA

MooreDFA *FABuilder::buildMooreDFA() const {
  if (mooreLambda.empty())
    throw domain_error("cannot build MooreDFA, no lambda function set");
  if (!representsDFA())
    throw domain_error("cannot build DFA, builder's delta represents a NFA");
  checkStates();
  return new MooreDFA(S, V, s1, F, dDeltaOf(delta), mooreLambda);
} // FABuilder::buildMooreDFA

void FABuilder::clear() {
  S.clear();
  V.clear();
  delta.clear();
  s1 = State();
  F.clear();
} // FABuilder::clear


ostream &operator<<(ostream &os, const FABuilder &fab) {
  os << "S  = " << fab.S  << endl;
  os << "V  = " << fab.V  << endl;
  os << "s1 = " << fab.s1 << endl;
  os << "F  = " << fab.F  << endl;
  os << "delta = {" << endl;
  for (const auto &t: fab.delta.transitions())
    os << "  " << t << endl;
  os << "}" << endl;
  return os;
} // operator<<


// === test ============================================================

#if 0

#include "TapeStuff.cpp"
#include "StateStuff.cpp"
#include "MbMatrix.cpp"
#include "DeltaStuff.cpp"
#include "FA.cpp"
#include "NFA.cpp"
#include "DFA.cpp"

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  cout << "START: FABuilder" << endl;
  cout << endl;

  DFA *dfa  = nullptr;
  DFA *dfa2 = nullptr;
  NFA *nfa  = nullptr;


// *** build case selection ***
#define BUILD_CASE 1
// ****************************

  cout << "BUILD_CASE = " << BUILD_CASE << endl << endl;


#if BUILD_CASE == 1

  // 1. build FAs programmatically:
  FABuilder fab;
  fab.setStartState("S");
  fab.addFinalState("E");
  fab.addTransition("S", '0', "S");
  fab.addTransition("S", '1', "E");
  // fab.addTransition("X", '0', "Y"); add unreachable states

  cout << "fab:" << endl << fab << endl;

  cout << "DFA?: " << fab.representsDFA() << endl;
  dfa = fab.buildDFA();
  cout << "dfa:" << endl;
  cout << *dfa;

  fab.addTransition("S", '1', {"S" , "E"});
  cout << "DFA?: " << fab.representsDFA() << endl;
  nfa = fab.buildNFA();

#elif BUILD_CASE == 2

  // 2. build NFA from text file:
  FABuilder fab(string("NFA1.txt"));
  cout << "DFA?: " << fab.representsDFA() << endl;
  nfa = fab.buildNFA();

#elif BUILD_CASE == 3

  // 3. build NFA from string literal:
  FABuilder fab(
    "-> S -> 0 S | 0 A | 1 S  \n\
       A -> 1 E               \n\
    () E ->                   \n" );
  cout << "DFA?: " << fab.representsDFA() << endl;
  nfa = fab.buildNFA();

#else // none of the BUILD_TESTCASEs above
  cerr << "ERROR: invalid BUILD_CASE == " << BUILD_CASE << endl;
#endif

  cout << "nfa:" << endl;
  cout << *nfa;

  cout << "nfa -> dfa2:" << endl;
  dfa2 = nfa->dfaOf();
  cout << "dfa2:" << endl;
  cout << *dfa2;

  fab.clear();

  delete dfa;
  delete dfa2;
  delete nfa;

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

// end of FABuilder.cpp
//======================================================================
