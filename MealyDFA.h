//
// Created by 1004y on 26.11.2024.
//

#ifndef MEALYDFA_H
#define MEALYDFA_H
#include "DFA.h"

class FABuilder;

class MealyDFA final : public DFA, private ObjectCounter<MealyDFA> {

    friend class FABuilder;

    typedef DFA Base;

protected:

    MealyDFA(const StateSet &S, const TapeSymbolSet &V,
             const State &s1, const StateSet &F,
             const DDelta &delta,
             const std::map<std::pair<State, TapeSymbol>, char> &lambda);

public:

    const std::map<std::pair<State, TapeSymbol>, char> lambda;   // lambda function for output

    bool accepts(const Tape &tape) const override; // override DFA::accepts
};



#endif //MEALYDFA_H
