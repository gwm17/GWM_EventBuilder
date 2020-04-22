/*main.cpp
 *Main function for digital SPS processing. Takes in a single argument (input file name),
 *processes the input file and feeds the relevant information to analysis classes.
 *Improvement areas: add flags to make run cases, allow multi-file processing
 *
 *Gordon M. Oct 2019
 *
 *Updated Jan 2020 GWM -- Has multifile processing, also a neat splash
 */
#include "EventBuilder.h"
#include <TApplication.h>
#include "SFPAnalyzer.h"
#include "RealTimer.h"
#include "RunCollector.h"
#include "TimeSort.h"
#include "FastSort.h"
#include <chrono>
#include <unistd.h>

using namespace std;

//Option parsing; currently set so that user can choose to run with or without fast sorting, and can choose
//to run only the analyzer (will only work if sorted files already exist)
//set up in such a way that adding more cases is a non issue.
//s-slow sorting only a-analyze only (assumes analyze fast files) sa-only analyze slow files
static const char* optString = "sa";

struct options {
  bool slowFlag = false;
  bool onlyAnalyzeFlag = false;
} options;

int main(int argc, char *argv[]) {
  if(argc == 2 || argc == 3) {
    
    int c;
    while((c=getopt(argc,argv,optString)) != -1) {
      switch(c) {
        case 's':
          options.slowFlag = true;
          break;
        case 'a':
          options.onlyAnalyzeFlag = true;
          break;
      }
    }   

    TApplication app("app", &argc, argv);
    char* name;
    if(app.Argc() == 2) name = app.Argv(1);
    else  name = app.Argv(2);
    ifstream input(name);
    if(input.is_open()) {
      string dir, shifted, sorted, analyzed, junk, fast, map, gains, shifts;
      int zt, at, zp, ap, ze, ae;
      double ep, angle, b;
      float scint, cw, si_fcw, ion_fcw;
      int min, max;
      input>>junk>>zt>>junk>>at>>junk>>zp>>junk>>ap>>junk>>ze>>junk>>ae;
      input>>junk>>ep>>junk>>angle>>junk>>b;
      input>>junk>>shifts>>junk>>scint>>junk>>cw>>junk>>si_fcw>>junk>>ion_fcw;
      input>>junk>>dir>>junk>>min>>junk>>max;
      input>>junk>>shifted>>junk>>sorted>>junk>>fast>>junk>>analyzed;
      input>>junk>>map;
      input>>junk>>gains;
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
      cout<<"Board offset file: "<<shifts<<endl;
      cout<<"Coincidence window: "<<cw<<endl;
      cout<<"Si Fast Coincidence Window: "<<si_fcw<<endl;
      cout<<"Ion Chamber Fast Coincidence Window: "<<ion_fcw<<endl;
      cout<<"SABRE Channel Map File: "<<map<<endl;
      cout<<"Gain Matching File: "<<gains<<endl;
      cout<<"--------------------------------------------------"<<endl;
      cout<<"~~~~~~~~~~~~~~~~~ Analyzer Output ~~~~~~~~~~~~~~~~"<<endl;
      vector<TString> filelist;
      int validity;
      if(min>0 && max>0) {
        RunCollector grabber(dir, "compass", ".root",min, max);
        validity = grabber.GrabFilesInRange();
        filelist = grabber.filelist;
      } else {
        RunCollector grabber(dir, "compass", ".root");
        validity = grabber.GrabAllFiles();
        filelist = grabber.filelist;
      }
      if(validity) {
        string raw, this_shifted, this_sorted, this_analyzed, this_fast;

        /*Log file to document the statistics of the evb*/
        ofstream log("event_builder_log.txt");
        log<<"Event building statistics for most recent build"<<endl;
        double totalEvents=0, completeFP=0, completeFP_SABRE=0, SABREorphans=0,
               SABREorphans_noscint=0, FPorphans=0, FPorphans_partial=0,
               FPorphans_noscint=0, FPorphans_nogas=0, FPextras=0;

        for(unsigned int i=0; i<filelist.size(); i++) {
          gROOT->Reset();
          string raw = filelist[i].Data();
          cout<<"--------------------------------------------------"<<endl;
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

          RealTimer rt(scint, map, shifts);
          TimeSort no_hope(cw, map, gains);
          FastSort help_me(si_fcw, ion_fcw, map);
          SFPAnalyzer doa(zt,at,zp,ap,ze,ae,ep,angle,b);

          //if only analyze ... only analyze
          if(options.onlyAnalyzeFlag && options.slowFlag) {
            cout<<"Only analyze slow files option passed"<<endl;
            cout<<"Performing basic analysis..."<<endl;
            doa.Run(this_sorted.c_str(), this_analyzed.c_str());
            continue;
          } else if (options.onlyAnalyzeFlag) {
            cout<<"Only analyze fast files option passed"<<endl;
            cout<<"Performing basic analysis..."<<endl;
            doa.Run(this_fast.c_str(), this_analyzed.c_str());
            continue;
          }

          rt.Run(raw, this_shifted);
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
          if(options.slowFlag) {
            cout<<"Only slow sorting option passed"<<endl;
            cout<<"Skipping fast sort... Performing basic analysis..."<<endl;
            doa.Run(this_sorted.c_str(), this_analyzed.c_str());
            cout<<"--------------------------------------------------"<<endl;
          } else {
            cout<<"Sorting by fast coincidence..."<<endl;
            help_me.Run(this_sorted.c_str(), this_fast.c_str());
            cout<<"Performing basic analysis..."<<endl;
            doa.Run(this_fast.c_str(), this_analyzed.c_str());
            cout<<"--------------------------------------------------"<<endl;
          }
        }
        log<<"---------'Good' Events-----------"<<endl;
        log<<"Total number of events found: "<<totalEvents<<endl;
        log<<"Number of complete (2 anode, all delay line, scint) FP events: "
           <<completeFP<<endl;
        log<<"Number of complete FP events with SABRE: "<<completeFP_SABRE
           <<endl;
        log<<"Number of complete FP events with multi hits: "<<FPextras<<endl;
        log<<"---------------------------------"<<endl;
        log<<"----------'Bad' Events-----------"<<endl;
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
        log<<"---------------------------------"<<endl;
        log.close(); 
      }
      return 0;
    } else {
      cerr<<"input list file missing!"<<endl;
      return 1;
    }
  } else {
    cerr<<"Incorrect number of arguments! Requires input file name."<<endl;
    return 1;
  }
}
