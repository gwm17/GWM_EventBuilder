#include "EventBuilder.h"
#include "ShiftMap.h"

using namespace std;

ShiftMap::ShiftMap() {
  shifts.resize(numBoards);
  validity = false;
}

ShiftMap::~ShiftMap() {}

bool ShiftMap::SetShiftMap(string shiftfile) {
  if(validity == true) {
    cerr<<"ShiftMap is already set. No need to redo"<<endl;
    return validity;
  }

  ifstream input(shiftfile);
  if(!input.is_open()) {
    cerr<<"Unable to open shift input file!"<<endl;
    cerr<<"Locking shift map (all shifts = 0)."<<endl;
    return validity;
  }
  string junk;
  double this_shift;

  for (int i=0; i<numBoards; i++) {
    if(input.eof()) {
      cerr<<"ShiftMap expects more boards than found in input file!"<<endl;
      cerr<<"Number of boards found: "<<i<<" Number of boards expected: "<<numBoards<<endl;
      cerr<<"Unspecified boards left to 0 shift"<<endl;
    }
    
    input>>junk>>this_shift;
    shifts[i] = this_shift;
  }

  validity = true;
  return validity;
}

double ShiftMap::GetShift(int board) {
  if(!validity) {
    return 0.0;
  } else if (board >= numBoards || board < 0) {
    cerr<<"You've attempted to access a non-extant board! SHAME ON YOU"<<endl;
    cerr<<"Returning 0 shift"<<endl;
    return 0.0;
  }

  return shifts[board];
  
}
