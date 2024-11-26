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
    int i = 0; // Index des ersten Symbols
    TapeSymbol tSy = tape[i]; // Erstes Tape-Symbol
    State s = s1; // Startzustand

    // Ausgabe des Startzustands (vor Verarbeitung der Eingabe)
    std::cout << lambda.at(s);

    while (tSy != eot) { // Solange das Ende des Bands nicht erreicht ist
        s = delta[s][tSy]; // Übergang zum nächsten Zustand
        if (!defined(s)) {
            return false; // Ungültiger Zustand
        }

        i++;
        tSy = tape[i]; // Nächstes Symbol vom Band

        // Ausgabe des Zustands NACH dem Wechsel, ABER VOR dem Lesen des nächsten Symbols
        if (tSy != eot) {
            std::cout << lambda.at(s);
        }
    }

    return F.contains(s); // Akzeptieren, wenn Zustand final ist
}

void MooreDFA::onStateEntered(const State &s) const {
    cout << " -> " << lambda.at(s);
} // MooreDFA::onStateEntered

// end of MooreDFA.cpp
//======================================================================
