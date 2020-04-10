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
    string idLetter;
    while(input>>chan_i) {
      input>>idLetter;
      if(idLetter ==  "A") {
        chan.detID = 0;
      } else if (idLetter == "B") {
        chan.detID = 1;
      } else if (idLetter == "C") {
        chan.detID = 2;
      } else if (idLetter == "D") {
        chan.detID = 3;
      } else if (idLetter == "E") {
        chan.detID = 4;
      }
      input>>chan.side_pos.first>>chan.side_pos.second>>chan.ECutLo>>chan.ECutHi;
      smap[chan_i] = chan;
    }
    input.close();
    return 1;
  } else {
    cerr<<"Unable to find the Sabre channel map file!! Terminating process."<<endl;
    return 0;
  }
}
