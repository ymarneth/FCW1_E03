// DeltaStuff.h:                                               HDO, 2019
// ------------
// Transition functions (delta) for finite automata:
//   delta: State x TapeSymbol -> either State or StateSet.
// Delta is the generic base class for
// * DDelta used in     deterministic finite automata (DFA) and
// * NDelta used in non-deterministic finite automata (NFA).
// Objects of class Transition represent a transition: (src, tSy) -> dest.
//======================================================================

#pragma once
#ifndef DeltaStuff_h
#define DeltaStuff_h

#include <iosfwd>

#include "ObjectCounter.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "MbMatrix.h"


// --- generic class Transition for delta functions, see below ---

template<typename DestT>   // generic class for transitions for delta func.:
class Transition           //   (src, tSy) -> dest
        /*OC+*/ : private ObjectCounter<Transition<DestT>> /*+OC*/ {
  public:

    const State      &src;
    const TapeSymbol  tSy; // no ref since it's only a char
    const DestT      &dest;

    Transition(const State &src, TapeSymbol tSy, const DestT &dest)
    : src(src), tSy(tSy), dest(dest) {
    } // Transition

}; // Transition

template<typename DestT>
std::ostream &operator<<(std::ostream &os, const Transition<DestT> &t) {
  os << "(" << t.src << ", " << stringOf(t.tSy) << ") -> "<< t.dest;
  return os;
} // operator<<


// --- generic class Delta ---

template <typename DestT>  // generic class for delta functions:
class Delta:               //   delta: State x TapeSymbol -> DestT
             public  MbMatrix<State, TapeSymbol, DestT>
   /*OC+*/ , private ObjectCounter<Delta<DestT>> /*+OC*/ {

     typedef MbMatrix<State, TapeSymbol, DestT> Base;

  public:

    vector<Transition<DestT>> transitions() const {
      vector<Transition<DestT>> v;
      for (auto &p1: *this)
        for (auto &p2: p1.second)
          v.emplace_back(p1.first, p2.first, p2.second);
      return v;
    } // transitions

}; // Delta


// --- concrete types for delta functions: ---
//     * DDelta for     deterministic finite automata and
//     * NDelta for non-deterministic finite automata

typedef Delta<State   > DDelta; //     deterministic delta for DFA
typedef Delta<StateSet> NDelta; // non-deterministic delta for NFA


// --- transformation functions: DDelta <-> NDelta ---

NDelta nDeltaOf(const DDelta &dDelta);
  // no restrictions on destination states in dDelta, s -> {s}

DDelta dDeltaOf(const NDelta &nDelta);
  // all destination sets in nDelta must have one element only, {s} -> s


#endif

// end of DeltaStuff.h
//======================================================================

