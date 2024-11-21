# Aufgabe 1

## Umwandeln einer regulären Grammatik in einen NFA oder DFA

// TODO

## Umgekehrte Transformation von NFA oder DFA in eine reguläre Grammatik

// TODO

# Aufgabe 2

## a) 

// TODO

## b)

// TODO

# Aufgabe 3

## a)

// TODO

## b)

// TODO

## c)

```cpp
DFA *NFA::dfaOf() const {
    FABuilder fab;

    // 1. construct new delta function for DFA (S and V implicitly)
    const StateSet startStateSet = epsClosureOf(s1);
    SetOfStateSets allStateSets = startStateSet;
    SetOfStateSets sstc = startStateSet; // StateSets to check

    while (!sstc.empty()) {
        StateSet srcStateSet = sstc.anyElement();
        sstc.erase(srcStateSet);
        for (const TapeSymbol tSy: V) {
            StateSet destStateSet =
                    epsClosureOf(allDestsFor(srcStateSet, tSy));
            if (!destStateSet.empty()) {
                // transition is defined
                if (!allStateSets.contains(destStateSet)) {
                    allStateSets.insert(destStateSet);
                    sstc.insert(destStateSet);
                }
                fab.addTransition(srcStateSet.stateOf(), tSy,
                                  destStateSet.stateOf());
            }
        }
    }

    // 2. define new start state s1 for DFA
    fab.setStartState(startStateSet.stateOf());

    // 3. look for final states f and define new F for DFA
    for (const StateSet &stateSet: allStateSets)
        for (const State &f: F)
            if (stateSet.contains(f))
                fab.addFinalState(stateSet.stateOf());

    return fab.buildDFA();
}
```

## d)

// TODO

# Aufgabe 4

## a)

Init -> OptSign number
OptSign -> + | - | eps

S1

$\delta(Z,\epsilon,Iniz$) = $Z, number OptSign$
$\delta(Z,\epsilon,Optsign$) = Z, +$

S2

$\delta(Z, number, $

## b)

## c)

## d)

(Z, S.const int max = 100;)
|- (Z,';' Idlist Type 'const'. cosnt int max = 100;)
|- (Z, ';' IdList Type.int max = 100;)

# Aufgabe 5

## a)

## b)

## c)