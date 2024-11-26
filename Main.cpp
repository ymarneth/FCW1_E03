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
    FABuilder builder;

    // Step 1: Set the start state of the NFA based on the root of the grammar
    const auto startStateSymbol = g->root;
    builder.setStartState(startStateSymbol->name);

    for (const auto &[key, value]: g->rules) {
        auto const &nt = key;
        auto const &sequenceSet = value;

        for (const Sequence *seq: sequenceSet) {
            const State srcState(nt->name);
            const TapeSymbol tapeSymbol = seq->symbolAt(0)->name[0];

            if (const auto targetSymbol = seq->back(); targetSymbol->isNT()) {
                // Step 2: Add transitions (delta functions) based on the production rules of the grammar
                const State targetState(targetSymbol->name);
                builder.addTransition(srcState, tapeSymbol, targetState);
            } else {
                // Step 3: Add final states for rules that produce terminal strings
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

void addProductionRules(GrammarBuilder *builder, SymbolPool &sp,
                        const std::string &srcState, const std::string &tapeSymbol,
                        const std::set<std::string> &destStates, const std::set<std::string> &finalStates) {
    auto *srcSymbol = sp.ntSymbol(srcState);
    auto *terminalSymbol = sp.tSymbol(tapeSymbol);

    for (const auto &destState: destStates) {
        auto *destSymbol = sp.ntSymbol(destState);

        auto *sequence = new Sequence();
        sequence->append(terminalSymbol);
        sequence->append(destSymbol);

        builder->addRule(srcSymbol, sequence);

        if (finalStates.find(srcState) != finalStates.end() && srcState.at(0) == destState.at(0)) {
            builder->addRule(srcSymbol, new Sequence(terminalSymbol));
        }
    }
}

Grammar *grammarOf(const NFA *nfa) {
    SymbolPool sp;

    auto *rootSymbol = sp.ntSymbol(nfa->s1);
    const auto builder = std::make_unique<GrammarBuilder>(rootSymbol);

    for (const auto &[srcState, transitions]: nfa->delta) {
        for (const auto &[tapeSymbol, destStates]: transitions) {
            addProductionRules(builder.get(), sp, srcState, std::string(1, tapeSymbol), destStates, nfa->F);
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

void testAcceptsWithDFA() {
    cout << "5. Test DFA" << endl;
    cout << "------------------------" << endl;
    cout << endl;

    FABuilder builder;
    builder.setStartState("B")
            .addFinalState("R")
            .addTransition("B", 'b', "R")
            .addTransition("R", 'b', "R")
            .addTransition("R", 'z', "R");

    // Here, we use the raw pointer returned by buildDFA() and wrap it in a unique_ptr.
    const unique_ptr<DFA> dfa(builder.buildDFA());

    cout << "dfa:" << endl << *dfa;
    vizualizeFA("dfa", dfa.get());

    auto testInput = [&](const string &input) {
        cout << "dfa->accepts(\"" << input << "\") => ";
        if (dfa->accepts(input)) {
            cout << " (accepted)" << endl;
        } else {
            cout << " (rejected)" << endl;
        }
    };

    testInput("b");
    testInput("bzb");
    testInput("bbb");
    testInput("bbzbb");
    testInput("bzzb");
    testInput("z");
    testInput("bbba");

    cout << endl;
}

void testMoore() {
    cout << "START: Moore" << endl;
    cout << endl;

    // DFA for transformation 0 -> F and 1 -> T in binary numbers
    FABuilder builder;
    builder.setStartState("S")
            .addFinalState("Zero")
            .addFinalState("One")
            .addTransition("S", '0', "Zero")
            .addTransition("S", '1', "One")
            .addTransition("Zero", '0', "Zero")
            .addTransition("Zero", '1', "One")
            .addTransition("One", '0', "Zero")
            .addTransition("One", '1', "One")
            .setMooreLambda({
                {"S", ' '},
                {"Zero", 'F'},
                {"One", 'T'}
            });

    const unique_ptr<DFA> mooreDfa(builder.buildMooreDFA());
    cout << "mooreDfa->acceptsAsTemplateMethod(\"0101\") = " <<
            mooreDfa->accepts("0101") << endl;
    cout << endl;
}

void testMooreTranslation() {
    cout << "6. Moore Translation" << endl;
    cout << "------------------------" << endl;
    cout << endl;

    FABuilder builder;

    // Konfiguriere den Automaten
    builder.setStartState("B")
           .addFinalState("R")
           .addTransition("B", 'b', "R")
           .addTransition("R", 'b', "R")
           .addTransition("R", 'z', "R");

    // Zustands-Ausgabe-Mapping (lambda-Funktion)
    builder.setMooreLambda({
        {"B", 'c'}, // Startzustand -> 'c' für Buchstabe
        {"R", 'd'}  // Zustand für Ziffern -> 'd'
    });

    // Erstelle den Moore-DFA
    const std::unique_ptr<MooreDFA> mooreDfa(builder.buildMooreDFA());

    // Eingabeband vorbereiten (bzzb -> cddc)
    Tape tape = {'b', 'z', 'z', 'b', eot};

    std::cout << "Eingabeband: bzzb" << std::endl;
    std::cout << "Ausgabeband: ";

    // Automaton ausführen und akzeptieren testen
    if (mooreDfa->accepts(tape)) {
        std::cout << "\nEingabe akzeptiert!" << std::endl;
    } else {
        std::cout << "\nEingabe nicht akzeptiert!" << std::endl;
    }

    cout << endl;
}

void testMealyTranslation() {
    cout << "6. Mealy Translation" << endl;
    cout << "------------------------" << endl;
    cout << endl;

    FABuilder builder;

    builder.setStartState("B")
            .addFinalState("R")
            .addTransition("B", 'b', "R")
            .addTransition("R", 'b', "R")
            .addTransition("R", 'z', "R")
            .setMealyLambda({
                {{"B", 'b'}, 'c'},
                {{"B", 'z'}, 'd'},
                {{"R", 'b'}, 'c'},
                {{"R", 'z'}, 'd'}
            });

    const std::unique_ptr<MealyDFA> mealyDfa(builder.buildMealyDFA());

    std::cout << "Eingabeband: bzzb" << std::endl;
    std::cout << "Ausgabeband: ";

    if (mealyDfa->accepts("bzzb")) {
        cout << endl;
        std::cout << "Eingabe akzeptiert!" << std::endl;
    } else {
        cout << endl;
        std::cout << "Eingabe nicht akzeptiert!" << std::endl;
    }

    cout << endl;
}

void testNFAAccepts() {
    cout << "7. Test accepts methods" << endl;
    cout << "------------------------" << endl;
    cout << endl;

    FABuilder builder;
    builder.setStartState("S")
            .addFinalState("R")
            .addTransition("S", 'a', "S")
            .addTransition("S", 'b', "S")
            .addTransition("S", 'c', "S")
            .addTransition("S", 'a', "A")
            .addTransition("S", 'b', "B")
            .addTransition("S", 'c', "C")
            .addTransition("A", 'a', "A")
            .addTransition("A", 'b', "A")
            .addTransition("A", 'c', "A")
            .addTransition("A", 'a', "R")
            .addTransition("B", 'a', "B")
            .addTransition("B", 'b', "B")
            .addTransition("B", 'c', "B")
            .addTransition("B", 'b', "R")
            .addTransition("C", 'a', "C")
            .addTransition("C", 'b', "C")
            .addTransition("C", 'c', "C")
            .addTransition("C", 'c', "R");

    const unique_ptr<NFA> nfa(builder.buildNFA());

    cout << "nfa:" << endl << *nfa;
    vizualizeFA("nfa", nfa.get());

    vector<string> testStrings = {
        "a", "b", "c",
        "aa", "ab", "ac",
        "abc", "abca", "abcb", "abcc",
        "aaaabbbbcccc", "abcabcabcabc", "aabbccbbcca",
        "aaaaaaaa", "bbbbbbbb", "cccccccc",
        "d", "xyz"
    };

    auto testMethod = [&](const string &methodName, auto acceptsMethod) {
        cout << "Testing " << methodName << ":" << endl;
        for (const auto &input: testStrings) {
            cout << "nfa->" << methodName << "(\"" << input << "\") =>";

            constexpr int numIterations = 1000;

            startTimer();
            for (int i = 0; i < numIterations; ++i) {
                const auto result = acceptsMethod(input);
            }
            stopTimer();

            cout << " - Time for 1000 iterations: " << elapsedTime() << "ms" << endl;
        }
        cout << endl;
    };

    testMethod("accepts1", [&](const string &input) { return nfa->accepts1(input); });
    testMethod("accepts2", [&](const string &input) { return nfa->accepts2(input); });
    testMethod("accepts3", [&](const string &input) { return nfa->accepts3(input); });
}

void testDfaOf() {
    cout << "8. DFA of NFA" << endl;
    cout << "------------------------" << endl;
    cout << endl;

    FABuilder builder;
    builder.setStartState("S")
            .addFinalState("R")
            .addTransition("S", 'a', "S")
            .addTransition("S", 'b', "S")
            .addTransition("S", 'c', "S")
            .addTransition("S", 'a', "A")
            .addTransition("S", 'b', "B")
            .addTransition("S", 'c', "C")
            .addTransition("A", 'a', "A")
            .addTransition("A", 'b', "A")
            .addTransition("A", 'c', "A")
            .addTransition("A", 'a', "R")
            .addTransition("B", 'a', "B")
            .addTransition("B", 'b', "B")
            .addTransition("B", 'c', "B")
            .addTransition("B", 'b', "R")
            .addTransition("C", 'a', "C")
            .addTransition("C", 'b', "C")
            .addTransition("C", 'c', "C")
            .addTransition("C", 'c', "R");

    const unique_ptr<NFA> nfa(builder.buildNFA());

    cout << "nfa:" << endl << *nfa;
    vizualizeFA("nfa", nfa.get());

    const unique_ptr<DFA> dfaOfNfa(nfa->dfaOf());

    cout << "dfaOfNfa:" << endl << *dfaOfNfa;
    vizualizeFA("dfaOfNfa", dfaOfNfa.get());

    const unique_ptr<DFA> minDfaOfNfa(dfaOfNfa->minimalOf());

    cout << "minDfaOfNfa:" << endl << *minDfaOfNfa;
    vizualizeFA("minDfaOfNfa", minDfaOfNfa.get());
}

int main(int argc, char *argv[]) {
    installSignalHandlers();

    cout << "START: Main" << endl;
    cout << endl;

    try {
        /*testDFA();
        cout << endl;

        testNFA();
        cout << endl;

        testNFAFromGrammar();
        cout << endl;

        testGrammarOfNFA();
        cout << endl;*/

        /*testAcceptsWithDFA();
        cout << endl;

        testMooreTranslation();
        cout << endl;*/

        /*testNFAAccepts();
        cout << endl;*/

        /*testDfaOf();
        cout << endl;*/

        testMooreTranslation();
        cout << endl;

        testMealyTranslation();
        cout << endl;
    } catch (const exception &e) {
        cerr << "EXCEPTION (" << typeid(e).name() << "): " << e.what() << endl;
    }

    cout << endl;
    cout << "END Main" << endl;

    return 0;
}
