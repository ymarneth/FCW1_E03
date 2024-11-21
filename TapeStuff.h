// TapeStuff.h:                                           HDO, 2006-2019
// ------------
// Tape       is an alias for std::string, a sequences of TapeSymbols.
// TapeSymbol is an alias for char, a symbol on the tape of an automaton.
// TapeSymbolSet represents a set of TapeSymbols.
//======================================================================

#pragma once
#ifndef TapeStuff_h
#define TapeStuff_h

#include <initializer_list>
#include <iosfwd>
#include <set>
#include <string>

#include "ObjectCounter.h"


typedef std::string Tape;
typedef char        TapeSymbol;   // but printable chars only

constexpr TapeSymbol eot = '\0';  // end of tape symbol (used as sentinel on tape)
constexpr TapeSymbol eps = '\01'; // used for epsilon as greek e not in font

std::string stringOf(TapeSymbol tSy); // e. g., '0'  -> "0"   and 'A'   -> "A",
                                      // but    '\0' -> "eot" and '\01' -> "eps"


class TapeSymbolSet: public  std::set<TapeSymbol>
           /*OC+*/ , private ObjectCounter<TapeSymbolSet> /*+OC*/ {

    typedef std::set<TapeSymbol> Base;

  public:

    TapeSymbolSet() = default;
    TapeSymbolSet(const TapeSymbolSet  &tSySet) = default;
    TapeSymbolSet(      TapeSymbolSet &&tSySet) = default;

    TapeSymbolSet(TapeSymbol tSy);

    TapeSymbolSet(const std::string &str); // also accepts char* and char[]
    TapeSymbolSet(std::initializer_list<char> il);

    TapeSymbolSet &operator=(const TapeSymbolSet  &tSySet) = default;
    TapeSymbolSet &operator=(      TapeSymbolSet &&tSySet) = default;

    bool contains(TapeSymbol tSy) const;

    virtual ~TapeSymbolSet() = default;

}; // TapeSymbolSet

std::ostream &operator<<(std::ostream &os, const TapeSymbolSet &tSySet);


#endif

// end of TapeStuff.h
//======================================================================

