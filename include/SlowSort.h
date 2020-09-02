/*SlowSort.h
 *Class designed to first time-order raw data, and then based on a given coincidence window
 *sort the raw data into coincidence structures. Utilizes dictionary elements DPPChannel and
 *CoincEvent. Based on work by S. Balak, K. Macon, and E. Good from LSU. 
 *
 *Gordon M. Oct. 2019
 *
 *Refurbished and updated Jan 2020 GWM
 */
#ifndef TIME_SORT_H
#define TIME_SORT_H

#include "TTreeIndex.h"
#include "DataStructs.h"
#include "SabreMap.h"

using namespace std;

class SlowSort {

  public:
    SlowSort(float windowSize, string& mapfile);
    ~SlowSort();
    void Run(const char *infile_name, const char *outfile_name);

    /****** Data Counters *******/
    /*Use these for statistics on the event building*/
    double totalEvents;
    double completeFP;
    double completeFP_SABRE;
    double SABREorphans;
    double SABREorphans_noscint;
    double FPorphans;
    double FPorphans_partial;
    double FPorphans_noscint;
    double FPorphans_nogas;
    double FPextras;

  private:
    void Reset();
    void StartEvent();
    void ProcessEvent();

    float coincWindow;
    int illegalMap;
    vector<DPPChannel> hitList;
    DPPChannel hit;
    CoincEvent event;
    CoincEvent blank;
  
    double startTime, previousHitTime;    
    unordered_map<int, sabrechan> smap;

    /****** Focal Plane Global Channel Map ******/
    enum fpChMap {
      delayFL_id = 16*8+8,
      delayFR_id = 16*8+9,
      delayBL_id = 16*8+10,
      delayBR_id = 16*8+11,
      anodeF_id = 16*8+13,
      anodeB_id = 16*8+15,
      scintR_id = 16*8+0,
      scintL_id = 16*8+1,
      cath_id = 16*8+7
    };
 
};

#endif
