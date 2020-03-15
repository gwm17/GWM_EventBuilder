#include <TROOT.h>
#include <TString.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "SFPCleaner.h"
#include "RunMusher.h"

using namespace std;

int main(int argc, char *argv[]) {
  if(argc == 2) {
    ifstream input(argv[1]);
    if(input.is_open()) {
      string junk, data, histos, acut, he3cut, xcut;
      input>>junk>>data>>junk>>histos;
      input>>junk>>acut>>junk>>he3cut>>junk>>xcut;
      input.close();
      SFPCleaner mr_clean;
      cout<<"------ SPS-SABRE Histogrammer & Cleaner ------"<<endl;
      cout<<"Data File: "<<data<<endl;
      cout<<"Histogram File: "<<histos<<endl;
      cout<<"Alpha Cut File: "<<acut<<endl;
      cout<<"3He Cut File: "<<he3cut<<endl;
      cout<<"x1x2 Cut File: "<<xcut<<endl;
      cout<<"Running cleaner..."<<endl;
      if(mr_clean.SetCuts(acut, he3cut, xcut)) {
        TString test(data.c_str());
        if(test.EndsWith(".root")) {
          vector<TString> files;
          files.push_back(data);
          mr_clean.Run(files, histos);
        } else {
          RunMusher mushy(data);
          if(mushy.GrabAllFiles()) {
            vector<TString> files = mushy.filelist;
            mr_clean.Run(files, histos);
          }
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
