// NFA.h:                                                 HDO, 2006-2019
// -----
// Objects of class NFA represent non-deterministic finite automata.
//======================================================================

#pragma once
#ifndef NFA_h
#define NFA_h

#include "ObjectCounter.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "FA.h"
#include "Grammar.h"

class FABuilder; // forward for friend declaration only
class DFA; // forward for transformation NFA -> DFA

class NFA final : public FA, private ObjectCounter<NFA> {
    friend class FABuilder; // so ::build.. methods can call prot. constr.

    using Base = FA;

    // constructor called by FABuilder::build... methods only
    NFA(const StateSet &S, const TapeSymbolSet &V,
        const State &s1, const StateSet &F,
        const NDelta &delta);

    StateSet deltaAt(const State &src, TapeSymbol tSy) const override;

    bool accepts2(const State &s, const Tape &tape, int i) const; // uses backtracking

public:
    const NDelta delta; // non-deterministic transition function

    NFA(const NFA &nfa) = default;

    NFA(NFA &&nfa) = default;

    ~NFA() override = default;

    bool accepts(const Tape &tape) const override; // impl. of abstract method
    // calls one of the three accepts methods (1, 2 or 3) below

    bool accepts1(const Tape &tape) const; // uses multithreading

    bool accepts2(const Tape &tape) const; // uses backtracking

    bool accepts3(const Tape &tape) const; // uses tracing of StateSets

    NFA* faOf(const Grammar* g) const;

    StateSet epsClosureOf(const State &src) const;

    StateSet epsClosureOf(const StateSet &srcSet) const;

    StateSet allDestsFor(const StateSet &srcSet, TapeSymbol tSy) const;

    DFA *dfaOf() const; // transformation: NFA => DFA
};


#endif

// end of NFA.h
//======================================================================
