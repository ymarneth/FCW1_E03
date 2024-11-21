// FA.h:                                                  HDO, 2011-2019
// ----
// FA is the abstract base class for the two finite automata classes:
// *  DFA for     deterministic finite automata and
// *  NFA for non-deterministic finite automata.
// FABuilder implements the builder pattern to construct (N|D)FAs.
//======================================================================

#pragma once
#ifndef FA_h
#define FA_h

#include <iosfwd>
#include <string>
#include <vector>

#include "ObjectCounter.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "DeltaStuff.h"


class FA {  // abstract base class for DFA and NFA

  friend std::ostream &operator<<(std::ostream &os, const FA &fa);

  protected:

    // called by constructors for DFA and NFA only
    FA(const StateSet &S,  const TapeSymbolSet &V,
       const State    &s1, const StateSet      &F)
    : S(S), V(V), s1(s1), F(F) {
    } // FA

    FA(const FA  &fa) = default;
    FA(      FA &&fa) = default;

    // used by operator<< and writeToGraphVizFile only
    //   returns StateSet even for DFA, in this case with one element
    virtual StateSet deltaAt(const State &src, TapeSymbol tSy) const = 0;

    // used by operator<< and writeToGraphVizFile only
    std::vector<State> topSortedStates() const; // topological sort

  public:

    const StateSet      S;     // set of states       (cf. "nonterminals")
    const TapeSymbolSet V;     // set of tape symbols (cf. "terminals")
//  const XDelta        delta; // trans. func. defined in derived classes
    const State         s1;    // start state, an element of S
    const StateSet      F;     // set of final states, a subset of S

    virtual ~FA() = default;

    virtual bool accepts(const Tape &tape) const = 0;

    void genGraphVizFile(const std::string &fileName,
                         const std::string &name = "") const;

}; // FA

// operator<< works for DFA and NFA objects only as FA is abstract
std::ostream &operator<<(std::ostream &os, const FA &fa);


#endif

// end of FA.h
//======================================================================