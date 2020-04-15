#ifndef SHIFTMAP_H
#define SHIFTMAP_H

using namespace std;

class ShiftMap {

  public:
    ShiftMap();
    ~ShiftMap();
    bool SetShiftMap(string shiftfile);
    double GetShift(int board);
    

  private:
    vector<double> shifts;
    bool validity;
    const int numBoards = 9;
    
};

#endif
