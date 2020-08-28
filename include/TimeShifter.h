/*TimeShifter.h
 *Class used to shift timestamps of the SABRE detector and the scintillator
 *of the focal plane, so that time-order is such that the scintllator, SABRE,
 *and a specific piece of the ion chamber (usually an anode) are all coincident
 *This is for ease of event-building and to allow for the narrowest possible
 *coincidence windows.
 *Based on code written by S. Balak, K. Macon, E. Good from LSU
 *
 *Created Jan 2020 by GWM
 */
#ifndef REALTIMER_H
#define REALTIMER_H

#include "DataStructs.h"
#include "SabreMap.h"
#include "ShiftMap.h"

using namespace std;

class TimeShifter {
  public:
    TimeShifter(Int_t plast, string mapfile, string shiftfile);
    ~TimeShifter();
    void Run(string in, string out);

  private:
    Int_t SCINT_OFFSET;
    unordered_map<int, sabrechan> smap; //channel map
    DPPChannel hit;
    Int_t illegalMap; //flag to ensure good map
    ShiftMap shifts;
};

#endif
