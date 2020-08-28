/*SFPPlotter.h
 *Class for generating histogram files for SPS-SABRE data
 *Intended use case is generating a TChain of multiple analyzed files and making
 *histograms of the larger data set.
 *
 *Created Jan 2020 by GWM
 */

#ifndef SFPCLEANER_H
#define SFPCLEANER_H

#include "DataStructs.h"

using namespace std;

class SFPPlotter {
  public:
    SFPPlotter();
    SFPPlotter(bool tf);
    ~SFPPlotter();
    int SetCuts(string edename, string dexname, string exname, string xxname);
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
    TFile *edefile, *dexfile, *exfile, *xxfile;
    TCutG *EdECut, *dExCut, *x1x2Cut, *ExCut;

    TChain *chain;

};

#endif
