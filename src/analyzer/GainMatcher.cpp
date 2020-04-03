/*GainMatcher.cpp
 *Class used to hold relevant information for performing gain matching
 *Based on work done by S. Balak, K. Macon, and E. Good from LSU
 *
 *Created Jan 2020 by GWM
 */
#include "GainMatcher.h"

using namespace std;

GainMatcher::GainMatcher() {
  gain_file_name = "etc/March2020_gainmatch_2.0V_5486Am241.txt";
  scaler_map.resize(144);
  MakeVector();
}

GainMatcher::~GainMatcher() {

}

void GainMatcher::MakeVector() {
  ifstream input(gain_file_name);
  string junk;
  getline(input, junk);
  if(input.is_open()) {
    int channel;
    double temp1, temp2;
    while(input>>channel) {
      input>>temp1>>temp2;
      scaler_map[channel] = temp2;
    }
  } else {
    cerr<<"Unable to open GainMatcher file! Check to make sure one exists!"<<endl;
  }
}

double GainMatcher::GetScaler(int channel) {
  return scaler_map[channel];
}
