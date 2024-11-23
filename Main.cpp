// Main.cpp:                                              HDO, 2006-2020
// --------
// Simple main program for
// *     deterministic finite automata (DFA) and
// * non-deterministic finite automata (NFA).
//======================================================================

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <memory>  // For smart pointers

#include "GrammarBuilder.h"

using namespace std;

#include "SignalHandling.h"
#include "Timer.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "FA.h"
#include "DFA.h"
#include "NFA.h"
#include "FABuilder.h"
#include "GraphVizUtil.h"

void testDFA() {
    cout << "1. DFA" << endl;
    cout << "------" << endl;
    cout << endl;

    constexpr int buildCase = 1;
    unique_ptr<FABuilder> fab;

    switch (buildCase) {
        case 1: // programmatic init. of builder
            fab = make_unique<FABuilder>();
            fab->setStartState("B")
                    .addFinalState("R")
                    .addTransition("B", 'b', "R")
                    .addTransition("R", 'b', "R")
                    .addTransition("R", 'z', "R");
            break;
        case 2: // init. builder from text file
            fab = make_unique<FABuilder>("IdDFA.txt");
            break;
        case 3: // init. builder from C string literal
            fab = make_unique<FABuilder>(
                "-> B -> b R  \n\
                 () R -> b R | z R");
            break;
        default:
            throw runtime_error("invalid buildCase");
    }

    // Here, we use the raw pointer returned by buildDFA() and wrap it in a unique_ptr.
    const unique_ptr<DFA> dfa(fab->buildDFA());

    cout << "dfa:" << endl << *dfa;
    vizualizeFA("dfa", dfa.get());

    cout << "dfa->accepts(\"bzb\") = " << dfa->accepts("bzb") << endl;
    cout << "dfa->accepts(\"z\")   = " << dfa->accepts("z") << endl;
    cout << endl;
}

void testNFA() {
    cout << "2. NFA" << endl;
    cout << "------" << endl;
    cout << endl;

    const auto fab = make_unique<FABuilder>(
        "-> 1 -> a 2 | b 1    \n\
         () 2 -> a 2 | b 1 | b 3  \n\
            3 -> a 2 | b 4        \n\
         () 4 -> a 4 | b 4");

    const unique_ptr<NFA> nfa(fab->buildNFA());

    cout << "nfa:" << endl << *nfa;
    vizualizeFA("nfa", nfa.get());

    cout << "nfa->accepts1(\"abba\") = " << nfa->accepts1("abba") << endl;
    cout << "nfa->accepts2(\"abba\") = " << nfa->accepts2("abba") << endl;
    cout << "nfa->accepts3(\"abba\") = " << nfa->accepts3("abba") << endl;
    cout << "nfa->accepts1(\"bbab\") = " << nfa->accepts1("bbab") << endl;
    cout << "nfa->accepts2(\"bbab\") = " << nfa->accepts2("bbab") << endl;
    cout << "nfa->accepts3(\"bbab\") = " << nfa->accepts3("bbab") << endl;
    cout << endl;

    cout << "transforming nfa to dfaOfNfa ..." << endl;
    const unique_ptr<DFA> dfaOfNfa(nfa->dfaOf());
    cout << "dfaOfNfa:" << endl << *dfaOfNfa;
    vizualizeFA("dfaOfNfa", dfaOfNfa.get());

    cout << "minimizing dfaOfNfa to minDfaOfNfa ..." << endl;
    const unique_ptr<DFA> minDfaOfNfa(dfaOfNfa->minimalOf());
    cout << "minDfaOfNfa:" << endl << *minDfaOfNfa;
    vizualizeFA("minDfaOfNfa", minDfaOfNfa.get());

    cout << "renaming minDfaOfNfa to renMinDfaOfNfa ..." << endl;
    const unique_ptr<DFA> renMinDfaOfNfa(minDfaOfNfa->renamedOf());
    cout << "renMinDfaOfNfa:" << endl << *renMinDfaOfNfa;
    vizualizeFA("renMinDfaOfNfa", renMinDfaOfNfa.get());
}

NFA *faOf(const Grammar *g) {
    // Step 1: Prepare the FABuilder to construct the NFA
    FABuilder builder;

    // Step 2: Set up the start state of the NFA based on the root of the grammar
    const auto startStateSymbol = dynamic_cast<NTSymbol *>(g->root);
    builder.setStartState(startStateSymbol->name);

    // Step 3: Add transitions based on the production rules of the grammar
    for (const auto &rule: g->rules) {
        auto const &nt = rule.first;
        auto const &sequenceSet = rule.second;

        for (const Sequence *seq: sequenceSet) {
            const State srcState(nt->name);
            const TapeSymbol tapeSymbol = seq->symbolAt(0)->name[0];

            const auto targetSymbol = seq->back();

            if (targetSymbol->isNT()) {
                const State targetState(targetSymbol->name);
                builder.addTransition(srcState, tapeSymbol, targetState);
            } else {
                builder.addFinalState(nt->name);
            }
        }
    }

    return builder.buildNFA();
}

void testNFAFromGrammar() {
    cout << "3. NFA from Grammar" << endl;
    cout << "--------------------" << endl;
    cout << endl;

    const GrammarBuilder gb4(
        "G(1):                         \n\
                 1 -> a 2 | b 1           \n\
                 2 -> a 2 | b 1 | b 3 | a \n\
                 3 -> a 2 | b 4           \n\
                 4 -> a 4 | b 4 | b | a     ");

    const auto *grammar = gb4.buildGrammar();

    cout << "Creating NFA from Grammar..." << endl;
    const unique_ptr<NFA> nfa(faOf(grammar));

    cout << "nfaFromGrammar:" << endl << *nfa;
    vizualizeFA("nfaFromGrammar", nfa.get());
}

Grammar *grammarOf(const NFA *nfa) {
    SymbolPool sp;

    auto *rootSymbol = sp.ntSymbol(nfa->s1);
    const auto builder = std::make_unique<GrammarBuilder>(rootSymbol);

    // Step 3: Add transitions based on the production rules of the grammar
    for (const auto &transition: nfa->delta) {
        const auto &srcState = transition.first;
        const auto &transitions = transition.second;

        for (const auto &pair: transitions) {
            const auto &tapeSymbol = pair.first;
            const auto &destStates = pair.second;

            for (const auto &destState: destStates) {
                const auto srcSymbol = sp.ntSymbol(srcState);
                const auto destSymbol = sp.ntSymbol(destState);
                const auto tapeSymbolSymbol = sp.tSymbol(std::string(1, tapeSymbol));

                const auto sequence = new Sequence();
                sequence->append(tapeSymbolSymbol);
                sequence->append(destSymbol);

                builder->addRule(srcSymbol, sequence);

                if (nfa->F.contains(srcState) && srcState.at(0) == destState.at(0)) {
                    builder->addRule(srcSymbol, new Sequence(tapeSymbolSymbol));
                }
            }
        }
    }

    return builder->buildGrammar();
}

void testGrammarOfNFA() {
    cout << "4. Grammar from NFA" << endl;
    cout << "--------------------" << endl;
    cout << endl;

    const auto fab = make_unique<FABuilder>(
        "-> 1 -> a 2 | b 1         \n\
         () 2 -> a 2 | b 1 | b 3  \n\
            3 -> a 2 | b 4        \n\
         () 4 -> a 4 | b 4");

    const unique_ptr<NFA> nfa(fab->buildNFA());

    cout << "Creating Grammar from NFA..." << endl;
    const unique_ptr<Grammar> grammar(grammarOf(nfa.get()));

    cout << "grammarOfNFA:" << endl << *grammar;
}

int main(int argc, char *argv[]) {
    installSignalHandlers(); // to catch signals, especially SIGSEGV

    cout << "START: Main" << endl;
    cout << endl;

    startTimer();

    try {
        testDFA();
        cout << endl;

        testNFA();
        cout << endl;

        testNFAFromGrammar();
        cout << endl;

        testGrammarOfNFA();
        cout << endl;
    } catch (const exception &e) {
        cerr << "EXCEPTION (" << typeid(e).name() << "): " << e.what() << endl;
    }

    stopTimer();
    cout << "elapsed time: " << elapsedTime() << " [s]" << endl;

    cout << endl;
    cout << "END Main" << endl;

    return 0;
}
