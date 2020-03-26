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
#include "FastSort.h"
#include <chrono>

using namespace std;

int main(int argc, char *argv[]) {
  if(argc == 2) {
    TApplication app("app", &argc, argv);
    char *name = app.Argv(1);
    ifstream input(name);
    if(input.is_open()) {
      string dir, shifted, sorted, analyzed, junk, fast;
      int zt, at, zp, ap, ze, ae;
      double ep, angle, b;
      float si, scint, cw, fcw;
      int min, max;
      input>>junk>>zt>>junk>>at>>junk>>zp>>junk>>ap>>junk>>ze>>junk>>ae;
      input>>junk>>ep>>junk>>angle>>junk>>b;
      input>>junk>>si>>junk>>scint>>junk>>cw>>junk>>fcw;
      input>>junk>>dir>>junk>>min>>junk>>max;
      input>>junk>>shifted>>junk>>sorted>>junk>>fast>>junk>>analyzed;
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
      cout<<"Fast Coincidence File: "<<fast+"run*.root"<<endl;
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
        string raw, this_shifted, this_sorted, this_analyzed, this_fast;

        /*Log file to document the statistics of the evb*/
        ofstream log("event_builder_log.txt");
        log<<"Event building statistics for most recent build"<<endl;
        int totalEvents=0, completeFP=0, completeFP_SABRE=0, SABREorphans=0,
            SABREorphans_noscint=0, FPorphans=0, FPorphans_partial=0,
            FPorphans_noscint=0, FPorphans_nogas=0, FPextras=0;

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
              }
            }
          }
          this_shifted = shifted+suffix;
          this_sorted = sorted+suffix;
          this_fast = fast+suffix;
          this_analyzed = analyzed+suffix;
          RealTimer rt(si, scint);
          rt.Run(raw, this_shifted);
          TimeSort no_hope(cw);
          FastSort help_me(fcw);
          SFPAnalyzer doa(zt,at,zp,ap,ze,ae,ep,angle,b);
          cout<<"Sorting the file by timestamp..."<<endl;
          no_hope.Run(this_shifted.c_str(), this_sorted.c_str());
          /****Stats****/
          totalEvents += no_hope.totalEvents;
          completeFP += no_hope.completeFP;
          completeFP_SABRE += no_hope.completeFP_SABRE;
          SABREorphans += no_hope.SABREorphans;
          SABREorphans_noscint += no_hope.SABREorphans_noscint;
          FPorphans += no_hope.FPorphans;
          FPorphans_partial += no_hope.FPorphans_partial;
          FPorphans_noscint += no_hope.FPorphans_noscint;
          FPorphans_nogas += no_hope.FPorphans_nogas;
          /*************/
          FPextras += no_hope.FPextras;
          cout<<"Sorting by fast coincidence..."<<endl;
          help_me.Run(this_sorted.c_str(), this_fast.c_str());
          cout<<"Performing basic analysis..."<<endl;
          doa.Run(this_fast.c_str(), this_analyzed.c_str());
          cout<<"--------------------------------------------------"<<endl;
        }
        log<<"Total number of events found: "<<totalEvents<<endl;
        log<<"Number of complete (2 anode, all delay line, scint) FP events: "
           <<completeFP<<endl;
        log<<"Number of complete FP events with SABRE: "<<completeFP_SABRE
           <<endl;
        log<<"Number of complete FP events with multi hits: "<<FPextras<<endl;
        log<<"Number of orphaned (missing fp data) SABRE events: "
           <<SABREorphans<<endl;
        log<<"Number of SABRE orphans without a scint: "<<SABREorphans_noscint
           <<endl;
        log<<"Number of orphaned (missing fp data) focal plane events: "
           <<FPorphans<<endl;
        log<<"Number of FP orphans missing anode or delay: "<<FPorphans_partial
           <<endl;
        log<<"Number of FP orphans missing a scint: "<<FPorphans_noscint<<endl;
        log<<"Number of scintillator singles: "<<FPorphans_nogas<<endl;
        log.close(); 
      }
    } else {
      cerr<<"input list file missing!"<<endl;
    }
  } else {
    cerr<<"Incorrect number of arguments! Requires input file name."<<endl;
  }
}
