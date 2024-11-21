// FA.cpp:                                                HDO, 2011-2019
// ------
// FA is the abstract base class for two finite automata classes:
// *  DFA for     deterministic finite automata and
// *  NFA for non-deterministic finite automata.
// FABuilder implements the builder pattern to construct (N|D)FAs.
//======================================================================

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <set>
#include <sstream>
#include <typeinfo>

using namespace std;

#include "TapeStuff.h"
#include "StateStuff.h"
#include "DeltaStuff.h"
#include "FA.h"
#include "DFA.h"
#include "NFA.h"


// macro used in writeToGraphVizFile and operator<<:
#define LIST_STATES_IN_TOPOLOGIC_ORDER    // #undef for lexicographic order

// macro used in writeToGraphVizFile only:
#define COLLECT_TRANSITIONS_TO_ONE_ARROW  // #undef to draw one arrow per trans.

// macro used in operator<< only:
#define WRITE_TRANSITIONS_AS_TEXT         // #undef for programmatical init.


vector<State> FA::topSortedStates() const {
  TapeSymbolSet VwithEps = V;
  VwithEps.insert(eps);    // to respect epsilon transitions
  vector<State> tss;       // topologically sorted states
  tss.push_back(s1);       // start with start state s1
  for (unsigned int i = 0; i < tss.size(); i++) {
    State src = tss[i];    // copy State as tss.push_back may reallocate!!!
    for (char tSy: VwithEps)
      for (const State &dest: deltaAt(src, tSy))
        if (find(tss.begin(), tss.end(), dest) ==
            tss.end())     // dest is not in tss yet, so:
          tss.push_back(dest);
  } // for
  return tss;
} // topSortedStates


// writeToGraphVizFile:
// -------------------

// definition of layout for all texts in graphs in GraphViz:
static const string fontName = "Arial"; // used for nodes ...
static const int    fontSize = 10;      // ... and edges

static void writeTransition(ostream &ofs,
     const State &src, const State &dest, string tSysStr) {
  ofs << "  \"" << src  << "\" -> " <<
           "\"" << dest << "\"";
  if (tSysStr == stringOf(eps))
    ofs << " [label = \"e\"" << // greek e for EPSILON
           " fontname = Symbol";
  else
    ofs << " [label = \"" << tSysStr  << "\"" <<
           " fontname = " << fontName;
  ofs << " fontsize = " << fontSize << "]; " << endl;
} // writeTransition

void FA::genGraphVizFile(const string &fileName,
                         const string &name) const {
  ofstream ofs(fileName);
  if (!ofs.good())
    throw runtime_error("error on opening output file \"" +
                        fileName + "\"");

#ifdef LIST_STATES_IN_TOPOLOGIC_ORDER
  vector<State> Sordered = topSortedStates();
#else                      // list states in lexicographic
  vector<State> Sordered(this->S.begin(), this->S.end());
#endif
  TapeSymbolSet V = this->V; // copy to allow changes
  if (typeid(*this) == typeid(NFA)) // // it's an NFA, so need for ...
    V.insert(eps); // ... a trick to respect epsilon transitions

  ofs << "digraph finite_state_machine {" << endl;
  ofs << endl;
  ofs << "  rankdir = LR;" << endl;
  ofs << endl;
  ofs << "  node [fontname = " << fontName <<
                " fontsize = " << fontSize <<
                " style = filled fillcolor = gray90]" << endl;
  // optional name of automaton
  if (name != "")
    ofs << "  \"" << name << ":\" [shape = none style = \"\" " <<
           "fontsize = " << (fontSize + 2) << "];" << endl;
  // shape for START node *->
  ofs << "  node [shape = point]; START; // shape for START node" << endl;
  // shape for final nodes
  ofs << "  node [shape = " <<
        (maxStateNameLen(F) <= 2 ? "circle" : "ellipse") <<
         " peripheries = 2]";
  for (const State &f: F)
    ofs << " \"" << f << "\"";
  ofs << "; // shape for final nodes" << endl;
  // shape for non-final nodes
  ofs << "  node [shape = " <<
        (maxStateNameLen(S - F) <= 2 ? "circle" : "ellipse") <<
         " peripheries = 1];  // shape for non-final nodes" << endl;
  // finally all the transitions
  ofs << "  START -> \"" << s1 << "\";" << endl;
  ofs << endl;

#ifdef COLLECT_TRANSITIONS_TO_ONE_ARROW

  //       (src,  dest) -> {tSy1, tSy2, ...}
  map<pair<State, State>, TapeSymbolSet> collectedTransitions;
  for (const State &src: S)
    for (char tSy: V)
      for (const State &dest: deltaAt(src, tSy))
        collectedTransitions[make_pair(src, dest)].insert(tSy);
  for (const State &src: Sordered)
    for (const State &dest: S) {
      auto it = collectedTransitions.find(make_pair(src, dest));
      if (it != collectedTransitions.end()) {
          string tSysStr;
          bool hasEpsilonTransition = false;
          bool first = true;
          for (const auto &tSy: it->second) { // is a TapeSymbolSet
            if (tSy == eps)
              hasEpsilonTransition = true;
            else {
              if (!first)
                tSysStr.append(", ");
              tSysStr.append(stringOf(tSy)); // not EPSILON
              first = false;
            } // else
          } // for
          if (hasEpsilonTransition)
            writeTransition(ofs, src, dest, stringOf(eps));
          if (tSysStr != "")
            writeTransition(ofs, src, dest, tSysStr);
      } // if
    } // for

#else // draw one arrow per transition

  for (const State &src: Sordered)
    for (char tSy: V)
      for (const State &dest: deltaAt(src, tSy))
        if (tSy == EPSILON)
          writeTransition(ofs, src, dest, stringOf(EPSILON));
        else
          writeTransition(ofs, src, dest, stringOf(tSy));

#endif // COLLECT_TRANSITIONS_TO_ONE_ARROW

  ofs << endl;
  ofs << "}" << endl;
  ofs.close();
} // FA::genGraphVizFile


// operator<<:
// ----------

#ifndef WRITE_TRANSITIONS_AS_TEXT
static void writeStateSet(ostream &os, const StateSet &ss) {
   if (ss.size() == 1)
     os << "\"" << ss.anyElement() << "\"";
   else {
     os << "{";
     bool first = true;
     for (const State &s: ss) {
       if (!first)
         os << ", ";
       os << "\"" << s << "\"";
       first = false;
     } // for
     os << "}";
   } // else
} // writeStateSet
#endif

ostream &operator<<(ostream &os, const FA &fa) {
#ifdef LIST_STATES_IN_TOPOLOGIC_ORDER
  vector<State> Sordered = fa.topSortedStates();
#else // list states in lexicographic order
  vector<State> Sordered(fa.S.begin(), fa.S.end());
#endif
  TapeSymbolSet V = fa.V; // copy to allow changes
  StateSet      F = fa.F; // copy to allow changes
  if (typeid(fa)== typeid(NFA)) // it's an NFA, need for ...
    V.insert(eps);  //   ... a trick to respect epsilon transitions

#ifdef WRITE_TRANSITIONS_AS_TEXT

  for (const State &src: Sordered) {
    auto   tSyIt  = V.begin();
    while (tSyIt != V.end()) {
      TapeSymbol tSy = *tSyIt;
      const StateSet &destSet = fa.deltaAt(src, tSy);
      if (defined(destSet)) {
        if (src == fa.s1)
          os << "-> ";     // mark start state
        else
          os << "   ";
        if (fa.F.contains(src))
          os << "() ";     // mark final state
        else
          os << "   ";
        os << src << " -> ";
        bool first = true;
        for (const State &dest: destSet) {
          if (!first)
            os << " | ";
          first = false;
          os << stringOf(tSy) << " " << dest;
        } // for
        F.erase(src);
        tSyIt++;
        while (tSyIt != V.end()) {
          tSy = *tSyIt;
          const StateSet &destSet = fa.deltaAt(src, tSy);
          if (defined(destSet))
            for (const auto &dest: destSet)
              os << " | " << stringOf(tSy) << " " << dest;
          tSyIt++;
        } // while
        os << endl;
      } else // !defined(destSet)
        tSyIt++;
    } // while
  } // for
  for (const State &s: F)
    os << "   () " << s << endl;

 #else // write source text suitable for programmatic init.

   os << "  // assuming: FABuilder fab;" << endl;
   os << "  fab.setStartState (\"" << fa.s1 << "\");" << endl;
   os << "  fab.setFinalStates(";
   writeStateSet(os, fa.F);
   os << ");" << endl;
   for (const State &src: Sordered)
     for (const TapeSymbol tSy: V) {
       const StateSet &destSet = fa.deltaAt(src, tSy);
       if (defined(destSet)) {
         os << "  fab.addTransition(\"" << src << "\", \'" << tSy << "\', ";
         writeStateSet(os, fa.deltaAt(src, tSy));
         os << ");" << endl;
       } // if
     } // for

 #endif // WRITE_TRANSITIONS_AS_TEXT

  os << endl;
  return os;
} // operator<<


// end of FA.cpp
//======================================================================
