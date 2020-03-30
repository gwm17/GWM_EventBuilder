/*
 *Goal is to provide a fast coinc window for rejecting si.
 *And a way to orgainize focal plane data within slow cw.
 *
 */
#ifndef FASTSORT_H
#define FASTSORT_H

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "SabreMap.h"
#include "DataStructs.h"

using namespace std;

class FastSort {
  
  public:
    FastSort(float windowSize);
    ~FastSort();
    void Run(const char *infile_name, const char *outfile_name);

  private:
    void Reset();
    void ProcessSABRE();
    void ProcessFocalPlane(unsigned int iter);

    float coincWindow;
    CoincEvent *event_address, slowEvent;
    FastCoincEvent fastEvent, blank;
    int illegalMap;

    unordered_map<int, sabrechan> smap;
};

#endif
