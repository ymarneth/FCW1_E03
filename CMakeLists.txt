cmake_minimum_required(VERSION 3.29)
project(UE03_Program)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)

add_executable(UE03_Program
        DeltaStuff.cpp
        DeltaStuff.h
        DFA.cpp
        DFA.h
        FA.cpp
        FA.h
        FABuilder.cpp
        FABuilder.h
        Grammar.cpp
        Grammar.h
        GrammarBasics.cpp
        GrammarBasics.h
        GrammarBuilder.cpp
        GrammarBuilder.h
        GraphVizUtil.cpp
        GraphVizUtil.h
        Main.cpp
        MainMoore.cpp
        MbMatrix.cpp
        MbMatrix.h
        MooreDFA.cpp
        MooreDFA.h
        NFA.cpp
        NFA.h
        ObjectCounter.h
        SequenceStuff.cpp
        SequenceStuff.h
        SignalHandling.cpp
        SignalHandling.h
        StateStuff.cpp
        StateStuff.h
        SymbolStuff.cpp
        SymbolStuff.h
        TapeStuff.cpp
        TapeStuff.h
        Timer.cpp
        Timer.h
        Vocabulary.cpp
        Vocabulary.h
        MealyDFA.cpp
        MealyDFA.h)
