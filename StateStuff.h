// StateStuff.h:                                          HDO, 2006-2019
// ------------
// State, an alias for std::string represents the state of an automaton,
//   so std::string, char[] and char* are valid state(name)s.
// StateSet represents a set of States.
//======================================================================

#pragma once
#ifndef StateStuff_h
#define StateStuff_h

#include <initializer_list>
#include <iosfwd>
#include <set>
#include <string>

#include "ObjectCounter.h"


typedef std::string State; // empty string "" is the undefined state

class  StateSet;           // empty set    {} is the undefined state set

bool defined(const State    &s );  // s  != undefined state
bool defined(const StateSet &ss);  // ss != undefined state set
bool empty  (const StateSet &ss);  // ss.empty()?

int  maxStateNameLen(const StateSet & ss);

StateSet stateSetOf(const State    &s ); //  "s"       -> {"s"}
State    stateOf   (const StateSet &ss); // {"s", ...} -> "s+..."


class StateSet: public  std::set<State>  // equivalent to std::set<string>
      /*OC+*/ , private ObjectCounter<StateSet> /*+OC*/ {

    typedef std::set<State> Base;

  public:

    StateSet() = default;
    StateSet(const StateSet  &ss) = default;
    StateSet(      StateSet &&ss) = default;

    StateSet(const State &s); // s -> {s}

    StateSet(std::initializer_list<State> il);

    StateSet &operator=(const StateSet  &ss) = default;
    StateSet &operator=(      StateSet &&ss) = default;

    virtual ~StateSet() = default;

    bool contains(const State &s) const;

    using Base::insert; // to avoid hiding of ...
// ...   insert(const State    &s );  provided by Base class
    void insert(const StateSet &ss);

    using Base::erase; // to avoid hiding of ...
//  ...  erase(const State    &s);
    void erase(const StateSet &ss);

    State anyElement() const; // the element is not removed

    State stateOf() const; // {"s", ... } -> "s+..."

}; // StateSet

StateSet operator^(const StateSet &ss1, const StateSet &ss2); // intersection
StateSet operator-(const StateSet &ss1, const StateSet &ss2); // difference

std::ostream &operator<<(std::ostream &os, const StateSet &ss);


class SetOfStateSets: public  std::set<StateSet>
            /*OC+*/ , private ObjectCounter<SetOfStateSets> /*+OC*/ {

    typedef std::set<StateSet> Base;

  public:

    SetOfStateSets() = default;
    SetOfStateSets(const SetOfStateSets  &soss) = default;
    SetOfStateSets(      SetOfStateSets &&soss) = default;

    SetOfStateSets(const StateSet &ss);

    SetOfStateSets &operator=(const SetOfStateSets  &soss) = default;
    SetOfStateSets &operator=(      SetOfStateSets &&soss) = default;

    virtual ~SetOfStateSets() = default;

    bool contains(const StateSet &ss) const;

    StateSet anyElement() const; // the element is not removed

}; // SetOfStateSets

std::ostream &operator<<(std::ostream &os, const SetOfStateSets &soss);


#endif

// end of StateStuff.h
//======================================================================

