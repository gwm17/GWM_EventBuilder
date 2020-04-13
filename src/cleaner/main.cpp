#include "EventBuilder.h"
#include "SFPCleaner.h"
#include "RunCollector.h"

using namespace std;

int main(int argc, char *argv[]) {
  if(argc == 2) {
    ifstream input(argv[1]);
    if(input.is_open()) {
      string junk, data, histos, edecut, dexcut, excut, xxcut;
      int min, max;
      input>>junk>>data;
      input>>junk>>min>>junk>>max;
      input>>junk>>histos;
      input>>junk>>edecut>>junk>>xxcut>>junk>>excut>>junk>>dexcut;
      input.close();
      SFPCleaner mr_clean;
      cout<<"------ SPS-SABRE Histogrammer & Cleaner ------"<<endl;
      cout<<"Data Dir: "<<data<<endl;
      cout<<"Histogram File: "<<histos<<endl;
      cout<<"E-dE Cut File: "<<edecut<<endl;
      cout<<"E-xavg Cut File: "<<excut<<endl;
      cout<<"dE-xavg Cut File: "<<dexcut<<endl;
      cout<<"x1-x2 Cut File: "<<xxcut<<endl;
      cout<<"Running cleaner..."<<endl;
      if(mr_clean.SetCuts(edecut, dexcut, excut, xxcut)) {
        int validity;
        vector<TString> files;
        if(min>0 && max>0) {
          RunCollector mushy(data, "", ".root", min, max);
          validity = mushy.GrabFilesInRange();
          files = mushy.filelist;
        } else {
          RunCollector mushy(data, "", ".root");
          validity = mushy.GrabAllFiles();
          files = mushy.filelist;
        }
        if(validity) {
          mr_clean.Run(files, histos);
        }
      } else {
        cerr<<"Invalid cut files! Unable to run cleaner"<<endl;
      }
      cout<<"Cleaner complete"<<endl;
      cout<<"----------------------------------------------"<<endl;
    } else {
      cerr<<"Cannot open input file: "<<argv[1]<<"; cannot run cleaner."<<endl;
    }
  } else {
    cerr<<"Cleaner needs an input file!"<<endl;
  }
}
