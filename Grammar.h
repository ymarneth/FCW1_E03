// Grammar.h:                                             HDO, 2004-2020
// ---------
// Objects of class Grammar represent context free grammars, where
//   G(root) = { root -> Sequence | ... , NT -> ... | ... , ... }
// =====================================================================

#ifndef Grammar_h
#define Grammar_h

#include <iosfwd>
#include "ObjectCounter.h"
#include "SymbolStuff.h"
#include "Vocabulary.h"
#include "GrammarBasics.h"


// === class Grammar ===================================================

class Grammar final: private ObjectCounter<Grammar> {

  friend class GrammarBuilder; // so its build method can call private constr.

  friend std::ostream &operator<<(std::ostream &os, const Grammar &g);


  mutable SymbolPool sp;

    // constructor called by GrammarBuilder::buildGrammar only
    Grammar(NTSymbol *root, const RulesMap &rules,
            const VNt &vnt, const VT &vT, const V &v);

  public:

    // data components: same as in class GrammarBuilder but all const

          NTSymbol *const root;  // no ownership: SymbolPool is the owner of all symbols
    const RulesMap        rules; // has at least an empty rule for root
    const VNt             vNt;   // all nonterminals for rules, including root
    const VT              vT;    // all terminals occuring in rules
    const V               v;     // all symbols, union of vNt and vT

    Grammar(const Grammar &g) = default; // useless as grammars don't change
    Grammar &operator=(const Grammar &g) = delete; // impossible because of const data

    ~Grammar() override = default;

    VNt deletableNTs() const;    // returns a subset of vNt

    bool isEpsilonFree() const;  // only root may have an epsilon alternative
    bool rootHasEpsilonAlternative() const; // S -> ... | EPS | ...

}; // Grammar

std::ostream &operator<<(std::ostream &os, const Grammar &g);


#endif

// end of Grammar.h
//======================================================================
