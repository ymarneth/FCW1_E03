// DFA.cpp:                                               HDO, 2006-2019
// -------
// Objects of class DFA represent deterministic finite automata.
//======================================================================

#include <cmath>
#include <cstring>

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

using namespace std;

#include "TapeStuff.h"
#include "StateStuff.h"
#include "MbMatrix.h"
#include "FABuilder.h"
#include "DFA.h"


typedef MbMatrix<State, State, bool> NeTable; // non-equivalent table


static void printNeTable(const StateSet &S, const NeTable &ne) {
  cout << "ne\t|";
  for (const State &s: S)
    cout << "\t" << s;
  cout << endl;
  cout << "---\t+";
  for (size_t i = 0; i < S.size(); i++)
    cout << "\t---";
  cout << endl;
  for (const State &si: S) {
    cout << si << "\t| ";
    for (const State &sj: S)
      cout << "\t" << (ne[si][sj] ? "t" : "f");
    cout << endl;
  } // for
  cout << endl;
} // printNeTable


// --- implementation of class DFA ---

DFA::DFA(const StateSet &S,  const TapeSymbolSet &V,
         const State    &s1, const StateSet      &F,
         const DDelta   &delta)
: FA(S, V, s1, F), delta(delta) {
} // DFA::DFA


StateSet DFA::deltaAt(const State &src, TapeSymbol tSy) const {
  const State &s = delta[src][tSy];
  if (defined(s))
    return StateSet(s);
  else // !defined(s)
    return StateSet();     // empty set of states = undefined
} // DFA::deltaAt


bool DFA::accepts(const Tape &tape) const {
  int        i   = 0;       // index of first symbol
  TapeSymbol tSy = tape[i]; // fetch first tape symbol
  State      s   = s1;      // start state
  while (tSy != eot) {      // eot = end of tape
    s = delta[s][tSy];
    if (!defined(s))
      return false;         // s undefined, so no acceptance
    i++;
    tSy = tape[i];          // fetch next symbol
  } // while
  return F.contains(s);     // accepted <==> s element of F
} // DFA::accepts

void DFA::onStateEntered(const State &s) const {
  // nothing to do
} // DFA::onStateEntered

#if (defined(MEALY_DFA) || defined(MOORE_DFA))

void DFA::onTransitionFired(const State &s, TapeSymbol tSy) const {
  // nothing to do
} // DFA::onTransitionFired

bool DFA::acceptsAsTemplateMethod(const Tape &tape) const {
  int        i   = 0;       // index of first symbol
  TapeSymbol tSy = tape[i]; // fetch first tape symbol
  State      s   = s1;      // start state
  onStateEntered(s);
  while (tSy != eot) {      // eot = end of tape
    State oldS = s;
    s = delta[s][tSy];
    if (!defined(s))
      return false;         // s undefined, so no acceptance
    onTransitionFired(oldS, tSy);
    onStateEntered(s);
    i++;
    tSy = tape[i];          // fetch next symbol
  } // while
  return F.contains(s);     // accepted <==> s element of F
} // DFA::acceptsAsTemplateMethod

#endif


// State Minimization (cf. Asteroth/Baier, p. 270 and
// ------------------      Hopcroft/Motwani/Ullmann, p. 171):

DFA *DFA::minimalOf() const {

  NeTable ne; // table to define non-equivalent states

  // 1.  "table filling algorithm"
  // 1.a initialize ne table with false
  for (const State &si: S)
    for (const State &sj: S)
      ne[si][sj] = false;
  // 1.b final and non-final states are not equivalent
  for (const State &f : F)
    for (const State &s : (S - F))
      ne[f][s] =
      ne[s][f] = true;
  // 1.c now compute (non-)equivalent states
  bool anyChange = true;
  while (anyChange) {
    anyChange = false;
    for (const State &si: S)
      for (const State &sj: S)
          if ( (si != sj) && !ne[si][sj]) // si, sj seem to be equivalent
          for (TapeSymbol tSy: V) {
            State destSi = delta[si][tSy];
            State destSj = delta[sj][tSy];
            if ( (destSi != destSj) &&
                 ( !defined(destSi) ||
                   !defined(destSj) ||
                   ne[destSi][destSj] ) )
              ne[si][sj] = // true  // si and sj ...
              ne[sj][si] = // true  // ... are not equivalent
              anyChange  = true;
          }
  }

  // 2. from ne table create the partition of S (= a set of subsets)
  SetOfStateSets partition;
  for (const State &si: S) {
    StateSet subset = StateSet(si);
    for (const State &sj: S)
      if ( (    si != sj  ) &&
           (!ne[si]  [sj]) )  // si and sj are equiv.
        subset.insert(sj);
    partition.insert(subset);
  }

  FABuilder fab; // builder for the minimal DFA

  // 3. compute transitions for minimal DFA in builder
  for (const StateSet &srcStateSet: partition) {
    const State &srcState = srcStateSet.anyElement();
    for (TapeSymbol tSy: V) {
      const State &destState = delta[srcState][tSy];
      for (const StateSet &subset: partition)
        if (subset.contains(destState)) {
          fab.addTransition(srcStateSet.stateOf(), tSy,
                    /*dest*/subset.     stateOf());
          break;
        }
    }
  }

  // 4. look for start state s1 and define new s1 for min. DFA
  for (const StateSet &subset: partition)
    if (subset.contains(s1)) {
      fab.setStartState(subset.stateOf());
      break; // as s1 is in one subset only
    }

  // 5. look for final states f and define new F for min. DFA
  for (const StateSet &subset: partition)
    for (const State &f: F)
      if (subset.contains(f))
        fab.addFinalState(subset.stateOf());

  return fab.buildDFA();
}


DFA *DFA::renamedOf() const {

  // 1. rename states

  vector<State>  tss = FA::topSortedStates();
  int digits = (int)round(log(tss.size()) / log(10) + 0.5);
  map<State, State> newName; // map: old name -> new name ("0", "1", ...)

  for (size_t i = 0; i < tss.size(); i++) {
  string nn = to_string(i); // new name for tss[i]
  nn.insert(0, string(digits - nn.length(), '0'));
    newName[tss[i]] = nn;
    // cout << "  " << nn << " = " << tss[i]  << endl; // for debugging only
  } // for

  // 2. build new automaton

  FABuilder fab;

  for (auto &t: delta.transitions())
    fab.addTransition(newName[t.src], t.tSy, newName[t.dest]);
  fab.setStartState(newName[s1]);
  for (const State &f: F)
    fab.addFinalState(newName[f]);

  return fab.buildDFA();

} // DFA:renamedOf


// end of DFA.cpp
//======================================================================
