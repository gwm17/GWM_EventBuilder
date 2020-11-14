/*SlowSort.h
 *Class designed to first time-order raw data, and then based on a given coincidence window
 *sort the raw data into coincidence structures. Utilizes dictionary elements DPPChannel and
 *CoincEvent. Based on work by S. Balak, K. Macon, and E. Good from LSU. 
 *
 *Gordon M. Oct. 2019
 *
 *Refurbished and updated Jan 2020 GWM
 */
#ifndef SLOW_SORT_H
#define SLOW_SORT_H

#include "CompassHit.h"
#include "DataStructs.h"
#include "ChannelMap.h"

using namespace std;

class SlowSort {

  public:
    SlowSort();
    SlowSort(double windowSize, string& mapfile);
    ~SlowSort();
    inline void SetWindowSize(double window) { coincWindow = window; };
    inline bool SetMapFile(std::string& mapfile) { return cmap.FillMap(mapfile); };
    bool AddHitToEvent(CompassHit& mhit);
    CoincEvent GetEvent();
    void FlushHitsToEvent(); //For use with *last* hit list
    inline bool IsEventReady() { return eventFlag; };
    void Run(const char *infile_name, const char *outfile_name);

  private:
    void InitVariableMaps();
    void Reset();
    void StartEvent();
    void ProcessEvent();

    double coincWindow;
    vector<DPPChannel> hitList;
    bool eventFlag;
    DPPChannel hit;
    CoincEvent event;
    CoincEvent blank;
  
    double startTime, previousHitTime;    
    unordered_map<int, vector<DetectorHit>*> fpVMap;
    unordered_map<int, vector<DetectorHit>*> sabreVMap;

    ChannelMap cmap;
 
};

#endif
