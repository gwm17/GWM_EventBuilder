/*RealTimer.h
 *Class used to shift timestamps of the SABRE detector and the scintillator
 *of the focal plane, so that time-order is approximately fp ion-chamber,
 *scintillator, SABRE. This is for ease of event-building.
 *Based on code written by S. Balak, K. Macon, E. Good from LSU
 *
 *Created Jan 2020 by GWM
 */
#ifndef REALTIMER_H
#define REALTIMER_H

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <string>
#include <iostream>
#include "DataStructs.h"
#include "SabreMap.h"

using namespace std;

class RealTimer {
  public:
    RealTimer(UInt_t si, UInt_t plast);
    ~RealTimer();
    void Run(string in, string out);

  private:
    UInt_t SI_OFFSET;
    UInt_t SCINT_OFFSET;
    unordered_map<int, sabrechan> smap; //channel map
    DPPChannel hit;
    Int_t illegalMap; //flag to ensure good map
};

#endif
