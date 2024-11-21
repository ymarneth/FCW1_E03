// DFA.h:                                                 HDO, 2006-2019
// -----
// Objects of class DFA represent deterministic finite automata.
//======================================================================

#pragma once
#ifndef DFA_h
#define DFA_h

#include "ObjectCounter.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "FA.h"

class FABuilder;           // forward for friend declaration only

class DFA: public  FA, ObjectCounter<DFA> {

  friend class FABuilder;  // so FABuilder::build... methods can call prot. constr.

  typedef FA Base;

  protected: // allows derived classes, e.g., for Mealy and or Moore

    // constructor called by FABuilder::build... methods and derived classes
    DFA(const StateSet &S,  const TapeSymbolSet &V,
        const State    &s1, const StateSet      &F,
        const DDelta   &delta);

    StateSet deltaAt(const State &src, TapeSymbol tSy) const override;

  public:

    const DDelta delta;    // deterministic transition function

    DFA(const DFA  &dfa) = default;
    DFA(      DFA &&dfa) = default;

    ~DFA() override = default;

    bool accepts(const Tape &tape) const override; // impl. of abstr. meth.
    virtual void onStateEntered(const State &s) const; // hook for derived classes

    DFA *minimalOf() const; // minimization: DFA => minimal DFA

    DFA *renamedOf() const; // equiv. automation with states named 0, 1, ...

}; // DFA

#endif

// end of DFA.h
//======================================================================
