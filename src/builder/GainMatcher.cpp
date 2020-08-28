/*GainMatcher.cpp
 *Class used to hold relevant information for performing gain matching
 *Based on work done by S. Balak, K. Macon, and E. Good from LSU
 *
 *Created Jan 2020 by GWM
 */
#include "EventBuilder.h"
#include "GainMatcher.h"

using namespace std;

GainMatcher::GainMatcher() {
  scaler_map.resize(144);
}

GainMatcher::~GainMatcher() {

}

bool GainMatcher::SetFile(string filename) {
  gain_file_name = filename;
  validFile = MakeVector();
  return validFile;
}
bool GainMatcher::MakeVector() {
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
    return true;
  } else {
    cerr<<"Unable to open GainMatcher file! Check to make sure one exists!"<<endl;
    return false;
  }
}

double GainMatcher::GetScaler(int channel) {
  if(validFile) {
    return scaler_map[channel];
  } else {
    return 1.0; //dont do anything if there is no matching
  }
}
