/*SabreMap.cpp
 *Creates an unordered_map of sabre channels from an input file.
 *Based on work done by S. Balak, K. Macon, and E. Good.
 *
 *Gordon M. Oct. 2019
 */
#include "SabreMap.h"

int FillSabreMap(unordered_map<int, sabrechan> &smap) {
  ifstream input("etc/ChannelMap_March2020.dat"); //hardcoded!
  if(input.is_open()) {
    int chan_i;
    sabrechan chan;
    while(input>>chan_i) {
      input>>chan.detID>>chan.side_pos.first>>chan.side_pos.second>>chan.ECutLo>>chan.ECutHi;
      smap[chan_i] = chan;
    }
    input.close();
    return 1;
  } else {
    cerr<<"Unable to find the Sabre channel map file!! Terminating process."<<endl;
    return 0;
  }
}
