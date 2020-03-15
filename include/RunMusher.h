/*RunMusher.cpp
 *Copy of RunCollector for use in a separate program. 
 *Separated classes so that changes could be made independently
 *Check RunCollector for more detailed documentation
 *
 *Created Jan2020 GWM
 */
#ifndef RUNMUSHER_H
#define RUNMUSHER_H

#include <TROOT.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TCollection.h>
#include <TChain.h>
#include <TFile.h>
#include <TList.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

class RunMusher {
  public:
    RunMusher(string dirname);
    RunMusher(string dirname, int min, int max);
    ~RunMusher();
    int Merge(string outname);
    int GrabAllFiles();
    int GrabFilesInRange();
    vector<TString> filelist;

  private:
    TString dir;
    TString run;
    Int_t MaxRun, MinRun;
    const Int_t LITERALMAX = 1000;
};

#endif
