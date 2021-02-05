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
#include <TGProgressBar.h>

using namespace std;

class SFPPlotter {
  public:
    SFPPlotter();
    SFPPlotter(bool tf);
    ~SFPPlotter();
    int SetCuts(string edename, string dexname, string exname, string xxname);
    inline void AttachProgressBar(TGProgressBar* pb) { m_pb = pb; };
    int ReadCutlist(string& listname);
    void Run(vector<TString> files, string output);

  private:
    void Reset();
    void SetProgressBar(long total);
    void Chain(vector<TString> files); //Form TChain
    void MakeUncutHistograms(ProcessedEvent ev);
    void MakeCutHistograms(ProcessedEvent ev);

    /*Histogram fill wrapper functions*/
    void MyFill(string name, int binsx, double minx, double maxx, double valuex,
                             int binsy, double miny, double maxy, double valuey);
    void MyFill(string name, int binsx, double minx, double maxx, double valuex);

    ProcessedEvent *event_address, event, empty;

    /*ROOT Storage*/
    THashTable *rootObj;

    /*Files containing cuts, and actaul cuts... Names should be customized for
     *each individual experiment
     */
    TFile *edefile, *dexfile, *exfile, *xxfile;
    TCutG *EdECut, *dExCut, *x1x2Cut, *ExCut;
    bool cutFlag;

    TChain *chain;

    TGProgressBar* m_pb; //GUI progress

};

#endif
