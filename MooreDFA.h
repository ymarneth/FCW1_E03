// MooreDFA.h:                                           HDO, 2021-10-22
// ----------
// Objects of class MooreDFA represent deterministic finite automata
// for transformation purposes as defined by Edward F. Moore.
//======================================================================

#ifndef MooreDFA_h
#define MooreDFA_h

#include <map>

#include "ObjectCounter.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "DFA.h"


class FABuilder;

class MooreDFA final : public DFA, private ObjectCounter<MooreDFA> {

  friend class FABuilder;

  typedef DFA Base;

  protected:

    // constructor called by FABuilder::buildMooreDFA method only
    MooreDFA(const StateSet &S,  const TapeSymbolSet &V,
             const State    &s1, const StateSet      &F,
             const DDelta   &delta,
             const std::map<State, char> &lambda);

  public:

    const std::map<State, char> lambda;    // lambda function for output

    bool accepts(const Tape &tape) const override; // override DFA::accepts

    void onStateEntered(const State &s) const override;

};


#endif
