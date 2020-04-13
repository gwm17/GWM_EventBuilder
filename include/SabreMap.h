/*SabreMap.h
 *Creates an unordered_map of sabre channels from an input file.
 *Based on work by S. Balak, K. Macon, and E. Good from LSU.
 *
 *Gordon M. Oct. 2019
 */
#ifndef SABRE_MAP_H
#define SABRE_MAP_H

#include <TROOT.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

struct sabrechan {
  int detID;
  pair<string, int> side_pos;
  int ECutLo;
  int ECutHi;
};

int FillSabreMap(string mapfile, unordered_map<int, sabrechan> &smap);

#endif
