//#if 1
//// MainMealyAndMoore.cpp:                                HDO, 2021-10-23
//// ---------------------
//// Simple main program for classes MealyDFA and MooreDFA.
////======================================================================
//
//#include <cstdio>          // for getchar only
//#include <iostream>
//#include <stdexcept>
//
//using namespace std;
//
//#include "TapeStuff.h"
//#include "StateStuff.h"
//#include "MbMatrix.h"
//#include "DeltaStuff.h"
//#include "FA.h"
//#include "DFA.h"
//#include "NFA.h"
//#include "MooreDFA.h"
//#include "FABuilder.h"
//
//
//// Activation (1) allows simple builds via command line:
//// * for GNU   use:  g++      -std=c++17          Main.cpp
//// * for Clang use:  clang++  -std=c++17 -pthread Main.cpp
//// * for M.S.  use:  cl /EHsc /std:c++17          Main.cpp
//#if 1
//#include "TapeStuff.cpp"
//#include "StateStuff.cpp"
//#include "MbMatrix.cpp"
//#include "DeltaStuff.cpp"
//#include "FA.cpp"
//#include "DFA.cpp"
//#include "NFA.cpp"
//#include "MooreDFA.cpp"
//#include "FABuilder.cpp"
//#endif
//
//
//int main(int argc, char *argv[]) {
//    FABuilder *fab = nullptr;
//    MooreDFA *mooreDfa = nullptr;
//
//    cout << "START: Main" << endl;
//    cout << endl;
//
//    try {
//        // DFA for transformation 0 -> F and 1 -> T in binary numbers
//
//        fab = new FABuilder(
//            "-> S    -> 0 Zero | 1 One \n\
//      () Zero -> 0 Zero | 1 One \n\
//      () One  -> 0 Zero | 1 One ");
//
//        cout << "test MooreDFA:" << endl;
//        fab->setMooreLambda({
//            {"S", ' '},
//            {"Zero", 'F'},
//            {"One", 'T'}
//        });
//        mooreDfa = fab->buildMooreDFA();
//        cout << "mooreDfa->acceptsAsTemplateMethod(\"0101\") = " <<
//                mooreDfa->accepts("0101") << endl;
//        cout << endl;
//    } catch (const exception &e) {
//        cerr << "EXCEPTION (" << typeid(e).name() << "): " << e.what() << endl;
//    } // catch
//
//    delete fab;
//    delete mooreDfa;
//
//    cout << endl;
//    cout << "END Main" << endl;
//
//    return 0;
//} // main
//
//
//// end of MainMealyAndMoore.cpp
////======================================================================
//
//#endif
//