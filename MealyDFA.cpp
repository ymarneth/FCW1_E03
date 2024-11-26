
#include <iostream>
#include <map>

using namespace std;

#include "MealyDFA.h"
#include "FABuilder.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "MbMatrix.h"


MealyDFA::MealyDFA(const StateSet &S, const TapeSymbolSet &V,
         const State &s1, const StateSet &F,
         const DDelta &delta,
         const map<pair<State, TapeSymbol>, char> &mealyLambda)
    : DFA(S, V, s1, F, delta), lambda(mealyLambda) {
}

bool MealyDFA::accepts(const Tape &tape) const {
    int i = 0;
    TapeSymbol tSy = tape[i];
    State s = s1;

    std::cout << lambda.at(make_pair(s, tSy));

    while (tSy != eot) {
        s = delta[s][tSy];

        if (!defined(s)) {
            return false;
        }

        i++;
        tSy = tape[i];

        if (tSy != eot) {
            std::cout << lambda.at(make_pair(s, tSy));
        }
    }

    return F.contains(s);
}
