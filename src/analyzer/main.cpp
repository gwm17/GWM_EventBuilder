/*main.cpp
 *Main function for digital SPS processing. Takes in a single argument (input file name),
 *processes the input file and feeds the relevant information to analysis classes.
 *Improvement areas: add flags to make run cases, allow multi-file processing
 *
 *Gordon M. Oct 2019
 *
 *Updated Jan 2020 GWM -- Has multifile processing, also a neat splash
 */
#include <TROOT.h>
#include <TMath.h>
#include <TString.h>
#include <TApplication.h>
#include <iostream>
#include <fstream>
#include <string>
#include "SFPAnalyzer.h"
#include "RealTimer.h"
#include "RunCollector.h"
#include "TimeSort.h"
#include <chrono>

using namespace std;

int main(int argc, char *argv[]) {
  if(argc == 2) {
    TApplication app("app", &argc, argv);
    char *name = app.Argv(1);
    ifstream input(name);
    if(input.is_open()) {
      string dir, shifted, sorted, analyzed, junk;
      int zt, at, zp, ap, ze, ae;
      double ep, angle, b;
      float si, scint, cw;
      int min, max;
      input>>junk>>zt>>junk>>at>>junk>>zp>>junk>>ap>>junk>>ze>>junk>>ae;
      input>>junk>>ep>>junk>>angle>>junk>>b;
      input>>junk>>si>>junk>>scint>>junk>>cw;
      input>>junk>>dir>>junk>>min>>junk>>max;
      input>>junk>>shifted>>junk>>sorted>>junk>>analyzed;
      input.close();
      cout<<"--------------------------------------------------"<<endl;
      cout<<"|~~~~~~~~~~~~ GWM SPS-SABRE Analyzer ~~~~~~~~~~~~|"<<endl;
      ifstream logo("etc/logo.txt");
      string line;
      while(getline(logo, line)) {
        cout<<line<<endl;
      }
      logo.close();
      cout<<endl;
      cout<<"--------------------------------------------------"<<endl;
      cout<<"~~~~~~~~~~~~~~~~~ Run Information ~~~~~~~~~~~~~~~~"<<endl;
      cout<<"Data Directory: "<<dir<<endl;
      cout<<"Timeshifted File: "<<shifted+"run*.root"<<endl;
      cout<<"Sorted File: "<<sorted+"run*.root"<<endl;
      cout<<"Analyzed File: "<<analyzed+"run*.root"<<endl;
      cout<<"Zt: "<<zt<<" At: "<<at<<" Ze: "<<ze<<" Ae: "<<ae
          <<" Zp: "<<zp<<" Ap: "<<ap<<" E: "<<ep<<" angle: "
          <<angle<<" B: "<<b<<endl;
      cout<<"scintillator offset: "<<scint<<endl;
      cout<<"SABRE offset: "<<si<<endl;
      cout<<"Coincidence window: "<<cw<<endl;
      cout<<"--------------------------------------------------"<<endl;
      cout<<"~~~~~~~~~~~~~~~~~ Analyzer Output ~~~~~~~~~~~~~~~~"<<endl;
      vector<TString> filelist;
      int validity;
      if(min>0 && max>0) {
        RunCollector grabber(dir, min, max);
        validity = grabber.GrabFilesInRange();
        filelist = grabber.filelist;
      } else {
        RunCollector grabber(dir);
        validity = grabber.GrabAllFiles();
        filelist = grabber.filelist;
      }
      if(validity) {
        string raw, this_shifted, this_sorted, this_analyzed;
        for(unsigned int i=0; i<filelist.size(); i++) {
          gROOT->Reset();
          string raw = filelist[i].Data();
          cout<<"Working on "<<raw<<"..."<<endl;
          string suffix = "",test="", temp;
          int endflag = 0;
          for(unsigned int j=0; j<raw.size(); j++) {
            temp = raw[j];
            test += temp;
            if(endflag) {
              suffix += temp;
            }
            if(test == dir+"compass_") { //skip the path & the underscore after compass
              if(!endflag) {
                endflag++;
                i+=2;
              }
            }
          }
          this_shifted = shifted+suffix;
          this_sorted = sorted+suffix;
          this_analyzed = analyzed+suffix;
          RealTimer rt(si, scint);
          rt.Run(raw, this_shifted);
          TimeSort no_hope(cw);
          SFPAnalyzer doa(zt,at,zp,ap,ze,ae,ep,angle,b);
          auto start = chrono::high_resolution_clock::now();
          cout<<"Sorting the file by timestamp..."<<endl;
          no_hope.Run(this_shifted.c_str(), this_sorted.c_str());
          auto stop = chrono::high_resolution_clock::now();
          auto duration = chrono::duration_cast<chrono::milliseconds>(stop-start);
          cout<<"Elapsed time for Time Sort: "<<duration.count()<<" milliseconds"<<endl;
          cout<<"Performing basic analysis..."<<endl;
          doa.Run(this_sorted.c_str(), this_analyzed.c_str());
          cout<<"--------------------------------------------------"<<endl;
        }
      }
    } else {
      cerr<<"input list file missing!"<<endl;
    }
  } else {
    cerr<<"Incorrect number of arguments! Requires input file name."<<endl;
  }
}
