/*SFPCleaner.h
 *Class for generating histogram files for SPS-SABRE data
 *Intended use case is generating a TChain of multiple analyzed files and making
 *histograms of the larger data set.
 *
 *Created Jan 2020 by GWM
 */

#ifndef SFPCLEANER_H
#define SFPCLEANER_H

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TCutG.h>
#include <THashTable.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <iostream>
#include <string>
#include <vector>
#include "DataStructs.h"

using namespace std;

class SFPCleaner {
  public:
    SFPCleaner();
    SFPCleaner(bool tf);
    ~SFPCleaner();
    int SetCuts(string alphaname, string he3name, string x1x2name);
    void Run(string input, string output);
    void Run(vector<TString> files, string output);

  private:
    void Reset();
    void Chain(vector<TString> files); //Form TChain
    void MakeUncutHistograms(ProcessedEvent ev);
    void MakeCutHistograms(ProcessedEvent ev);

    /*Histogram fill wrapper functions*/
    void MyFill(string name, int binsx, double minx, double maxx, double valuex,
                             int binsy, double miny, double maxy, double valuey);
    void MyFill(string name, int binsx, double minx, double maxx, double valuex);

    ProcessedEvent *event_address, event, empty;
    bool treeFlag; /*Unused currently... add option to fill a gated output tree*/

    /*ROOT Storage*/
    THashTable *rootObj;

    /*Files containing cuts, and actaul cuts... Names should be customized for
     *each individual experiment
     */
    TFile *afile, *hefile, *xfile;
    TCutG *alphaCut, *he3Cut, *x1x2Cut;

    TChain *chain;

};

#endif
