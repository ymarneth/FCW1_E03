// GraphVizUtil.h:                                               HDO, 2020
// --------------
// GrahiViz (see https://graphviz.org) allows vizualization of complex
// graphs. Simple tests via http://www.webgraphviz.com.
// This module provides a function to vizualize finite automata
// by using dot.exe of GraphViz.
//======================================================================

#include <cctype>

#include <algorithm>
#include <iostream>
#include <stdexcept>
using namespace std;

#include "GraphVizUtil.h"


static bool fileExists(const string &fileName) {
  ifstream ifs(fileName);
  return ifs.good();
} // fileExists

static string dotExeFileName() {
  static string defn = "";

  if (defn != "") // correct file name has already been found
    return defn;

  // check if GraphViz (with its dot.exe) is on path
  #pragma warning(suppress : 4996) // for MS cl to suppress deprecated warning for getenv
  string path = getenv("PATH");
  transform(path.begin(), path.end(), path.begin(),
            [] (unsigned char c) { return tolower(c); }
           );
  if (path.find("graphviz") != string::npos)
    return defn = "dot.exe"; // will be found via PATH

  // look for GraphViz in "Program Files ..." directories
  const int pn = 2;
  string programDirectory[pn] = {"C:\\Program Files", "C:\\Program Files (x86)"};
  const int vn = 13;
  string version[vn] = {"", // no specific version
           "6.0.1",
           "5.0.1", "5.0.0",
           "4.0.0",
           "3.0.0",
           "2.50.0", "2.49.3", "2.49.2", "2.49.1", "2.49.0", "2.44.1", "2.38"};

  for (int i = 0; i < pn; i++) {
    for (int j = 0; j < vn; j++) {
      // try with blank before version number
      defn = programDirectory[i] + "\\Graphviz " + version[j] + "\\bin\\dot.exe";
      if (fileExists(defn))
        return defn;
      // try without blank before version number
      defn = programDirectory[i] + "\\Graphviz"  + version[j] + "\\bin\\dot.exe";
      if (fileExists(defn))
        return defn;
    } // for
  } // for

  // no instance of the versions defined above found
  string versions = string("\"") + version[0] + "\"";
  for (int i = 1; i < vn; i++)
    versions.append(string(", \"") + version[i] + "\"");
  throw runtime_error(
    string("GraphViz file dot.exe not found in PATH and \n") +
           "  neither in " + programDirectory[0] + "\\Graphviz[VERSION]\\bin \n" +
           "  nor     in " + programDirectory[1] + "\\Graphviz[VERSION]\\bin \n" +
           "  for VERSION = " + versions + ": \n" +
           "please install current version from www.graphviz.org");
} // dotExeFileName


void vizualizeFA(const string &faName, const FA *fa,
                 const string &format) {
  string gvFileName = faName + ".gv";

  // 1. generate GraphViz file
  cout << "writing    " << faName << " to " << gvFileName << " ..." << endl;
  fa->genGraphVizFile(gvFileName, faName);
  cout << "rendering  " << gvFileName << " to " << (gvFileName + "." + format) << " ..." << endl;

  // 2. call GraphViz dot.exe to render gvFileName into gvFileName.format
  string cmdStr = "\"" + dotExeFileName() + "\" -T" + format + " " +
    gvFileName + " -o " + gvFileName + "." + format;
  system(cmdStr.c_str());

  // 3. display gvFileName.format
  cout << "displaying " << gvFileName + "." + format << " ..." << endl;
  // 3.a either use the default application for files with this format
  cmdStr = "cmd /c \"start " + gvFileName + "." + format + "\"";
  // 3.b or start Internet Explorer
  // cmdStr = "\"C:\\Program Files\\Internet Explorer\\iexplore.exe\" %cd%\\" + gvFileName + "." + format;
  // 3.c or start Microsoft Edge
  // cmdStr = "\"C:\\Windows\\SystemApps\\Microsoft.MicrosoftEdge_8wekyb3d8bbwe\\MicrosoftEdge.exe\" %cd%\\" + gvFileName + "." + format;
  system(cmdStr.c_str());

  cout << endl;
} // vizualizeFA


// end of GraphVizUtil.cpp
//======================================================================