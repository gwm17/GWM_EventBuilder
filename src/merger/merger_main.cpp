#include "EventBuilder.h"
#include "RunCollector.h"

using namespace std;

int main(int argc, char *argv[]) {
  if(argc == 2) {
    ifstream input(argv[1]);
    if(input.is_open()) {
      string junk, dir, raw;
      int min, max;
      input>>junk>>dir>>junk>>min>>junk>>max>>junk>>raw;
      input.close();
      cout<<"CoMPASS Data Merger"<<endl;
      cout<<"-------------------"<<endl;
      cout<<"Data Directory: "<<dir<<endl;
      cout<<"Merged file: "<<raw<<endl;
      if(max >= 0 && min >= 0) {
        RunCollector rc(dir, "", ".root", min, max);
        rc.Merge_TChain(raw);
      } else {
        RunCollector rc(dir, "", ".root");
        rc.Merge_TChain(raw);
      }
      return 1;
    } else {
      cerr<<"Invalid input file! Unable to merge"<<endl;
      return 0;
    }
  } else{
    cerr<<"Need merger input file to gather data!"<<endl;
    return 0;
  }
}
