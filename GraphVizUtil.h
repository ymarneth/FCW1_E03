// GraphVizUtil.h:                                               HDO, 2020
// --------------
// GrahiViz (see https://graphviz.org) allows vizualization of complex
// graphs. Simple tests via http://www.webgraphviz.com.
// This module provides a function to vizualize finite automata
// by using dot.exe of GraphViz.
//======================================================================

#pragma once
#ifndef GraphVizUtil_h
#define GraphVizUtil_h

#include <string>

#include "FA.h"

void vizualizeFA(const std::string &faName, const FA *fa,
                 const std::string &format = "svg");

#endif

// end of GraphVizUtil.h
//======================================================================