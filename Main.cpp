#if 1
// Main.cpp:                                              HDO, 2006-2020
// --------
// Simple main program for
// *     deterministic finite automata (DFA) and
// * non-deterministic fintie automata (NFA).
//======================================================================

#include <cstdio>          // for getchar only
#include <iostream>
#include <stdexcept>

using namespace std;

#include "SignalHandling.h"
#include "Timer.h"
#include "TapeStuff.h"
#include "StateStuff.h"
#include "MbMatrix.h"
#include "DeltaStuff.h"
#include "FA.h"
#include "DFA.h"
#include "NFA.h"
#include "FABuilder.h"
#include "GraphVizUtil.h"

int main(int argc, char *argv[]) {

  FABuilder *fab            = nullptr;
  DFA       *dfa            = nullptr;
  NFA       *nfa            = nullptr;
  DFA       *dfaOfNfa       = nullptr;
  DFA       *minDfaOfNfa    = nullptr;
  DFA       *renMinDfaOfNfa = nullptr;

  installSignalHandlers(); // to catch signals, especially SIGSEGV

  cout << "START: Main" << endl;
  cout << endl;

  startTimer();

try {

  cout << "1. DFA" << endl;
  cout << "------" << endl;
  cout << endl;

  int buildCase = 1;
  switch (buildCase) {
    case 1: // programmatical init. of builder
      fab = new FABuilder(); // example from FS slides p. 47
      fab->setStartState("B").
           addFinalState("R").
           addTransition("B", 'b', "R").
           addTransition("R", 'b', "R").
           addTransition("R", 'z', "R");
      break;
    case 2: // init. builder from text file
      fab = new FABuilder(string("IdDFA.txt"));
      break;
    case 3: // init. builder from C string literal
      fab = new FABuilder(
        "-> B -> b R  \n\
         () R -> b R | z R");
      break;
    default:
      throw runtime_error("invalid buildCase");
  } // switch

  dfa = fab->buildDFA();
  delete fab;

  cout << "dfa:" << endl << *dfa;
  vizualizeFA("dfa", dfa);

  cout << "dfa->accepts(\"bzb\") = " << dfa->accepts("bzb") << endl;
  cout << "dfa->accepts(\"z\")   = " << dfa->accepts("z")   << endl;
  cout << endl;

  cout << "type CR to continue ... ";
  getchar();
  cout << endl;


  cout << "2. NFA" << endl;
  cout << "------" << endl;
  cout << endl;

  fab = new FABuilder( // Floyd, Beigel: The Language of Machines, p. 255
          "-> 1 -> a 2 | b 1        \n\
           () 2 -> a 2 | b 1 | b 3  \n\
              3 -> a 2 | b 4        \n\
           () 4 -> a 4 | b 4");
  nfa = fab->buildNFA();
  delete fab;

  cout << "nfa:" << endl << *nfa;
  vizualizeFA("nfa", nfa);

  cout << "nfa->accepts1(\"abba\") = " << nfa->accepts1("abba") << endl;
  cout << "nfa->accepts2(\"abba\") = " << nfa->accepts2("abba") << endl;
  cout << "nfa->accepts3(\"abba\") = " << nfa->accepts3("abba") << endl;
  cout << "nfa->accepts1(\"bbab\") = " << nfa->accepts1("bbab") << endl;
  cout << "nfa->accepts2(\"bbab\") = " << nfa->accepts2("bbab") << endl;
  cout << "nfa->accepts3(\"bbab\") = " << nfa->accepts3("bbab") << endl;
  cout << endl;

  cout << "transforming nfa to dfaOfNfa ..." << endl;
  dfaOfNfa = nfa->dfaOf();
  cout << "dfaOfNfa:" << endl << *dfaOfNfa;
  vizualizeFA("dfaOfNfa", dfaOfNfa);

  cout << "minimizing dfaOfNfa to minDfaOfNfa ..." << endl;
  minDfaOfNfa = dfaOfNfa->minimalOf();
  cout << "minDfaOfNfa:" << endl << *minDfaOfNfa;
  vizualizeFA("minDfaOfNfa", minDfaOfNfa);

  cout << "renaming minDfaOfNfa to renMinDfaOfNfa ..." << endl;
  renMinDfaOfNfa = minDfaOfNfa->renamedOf();
  cout << "renMinDfaOfNfa:" << endl << *renMinDfaOfNfa;
  vizualizeFA("renMinDfaOfNfa", renMinDfaOfNfa);


} catch(const exception &e) {
  cerr <<  "EXCEPTION (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // fab has already been deleted
  delete dfa;
  delete nfa;
  delete dfaOfNfa ;
  delete minDfaOfNfa;
  delete renMinDfaOfNfa;

  stopTimer();
  cout << "elapsed time: " << elapsedTime() << " [s]" <<endl;

  cout << endl;
  cout << "END Main" << endl;

  return 0;

} // main


// end of Main.cpp
//======================================================================

#endif
