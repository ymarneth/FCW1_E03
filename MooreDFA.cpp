// MooreDFA.h:                                           HDO, 2021-10-22
// ----------
// Objects of class MooreDFA represent deterministic finite automata
// for transformation purposes as defined by Edward F. Moore.
//======================================================================

#include <iostream>
#include <map>

using namespace std;

#include "MooreDFA.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "MbMatrix.h"
#include "FABuilder.h"


// --- implementation of class MooreDFA ---

MooreDFA::MooreDFA(const StateSet &S, const TapeSymbolSet &V,
                   const State &s1, const StateSet &F,
                   const DDelta &delta,
                   const map<State, char> &lambda)
    : DFA(S, V, s1, F, delta), lambda(lambda) {
}

bool MooreDFA::accepts(const Tape &tape) const {
    int i = 0; // index of first symbol
    TapeSymbol tSy = tape[i]; // fetch first tape symbol
    State s = s1; // start state

    // Output the start state's corresponding output
    cout << lambda.at(s); // this should print 'c' for state B at the beginning

    onStateEntered(s);

    while (tSy != eot) { // eot = end of tape
        s = delta[s][tSy]; // transition to the next state
        if (!defined(s))
            return false; // state is undefined, so no acceptance

        // Output the corresponding symbol for the current state
        cout << lambda.at(s); // should print 'd' for state R when transitioning to R for 'z'
        onStateEntered(s);

        i++;
        tSy = tape[i]; // fetch next symbol
    } // while

    return F.contains(s); // accepted if the final state is in the final state set
}

void MooreDFA::onStateEntered(const State &s) const {
    cout << " -> " << lambda.at(s);
} // MooreDFA::onStateEntered

// end of MooreDFA.cpp
//======================================================================
