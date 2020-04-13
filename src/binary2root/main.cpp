#include "EventBuilder.h"
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
#include "CompassUnpacker.h"
#include "RunCollector.h"

using namespace std;

int main(int argc, char *argv[]) {
  if(argc == 2) {
    ifstream input(argv[1]);
    string junk, data, output;
    int min, max;
    input>>junk>>data;
    input>>junk>>min>>junk>>max;
    input>>junk>>output;
    input.close();
    cout<<"---------binary2root---------"<<endl;
    cout<<"Name of data dir: "<<data<<endl;
    cout<<"Name of output dir: "<<output<<endl;
    vector<TString> filelist;
    int validity;
    if(min>0 && max>0) {
      RunCollector grabber(data, "", ".tar.gz", min, max);
      validity = grabber.GrabFilesInRange();
      filelist = grabber.filelist;
    } else {
      RunCollector grabber(data, "", ".tar.gz");
      validity = grabber.GrabAllFiles();
      filelist = grabber.filelist;
    }
    if(validity) {
      string raw, this_data, this_output;
      for(unsigned int i=0; i<filelist.size(); i++) {
        gROOT->Reset();
        string raw = filelist[i].Data();
        cout<<"Working on "<<raw<<"..."<<endl;
        string suffix = "",test="", temp, suffix_sans_filetype = "";
        int endflag = 0, firstflag =0;
        for(unsigned int j=0; j<raw.size(); j++) {
          temp = raw[j];
          if(temp == "." && !firstflag) {
            firstflag++;
            suffix_sans_filetype = suffix;
          }
          test += temp;
          if(endflag) {
            suffix += temp;
          }
          if(test == data && !endflag) { 
            endflag++;
          }
        }
        this_data = data+suffix;
        this_output = output+"compass_"+suffix_sans_filetype+".root";
        CompassUnpacker goodluck(this_data);
        goodluck.Run(this_output);
      }
    }
    cout<<"-----------------------------"<<endl;
    return 0;
  } else { 
    cerr<<"No input file given; need a file which gives fullpath name of file to be"
        <<" parsed and the fullpath name of the file to be created."<<endl;
    return 1;
  }
}
