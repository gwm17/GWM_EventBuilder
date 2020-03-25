/*RunMusher.cpp
 *Copy of RunCollector for use in a separate program. 
 *Separated classes so that changes could be made independently
 *Check RunCollector for more detailed documentation
 *
 *Created Jan2020 GWM
 */
#include "RunMusher.h"

using namespace std;

RunMusher::RunMusher(string dirname) {
  dir = dirname.c_str();
  cout<<"Searching directory: "<<dir.Data()<<" for segments of run: "<<run.Data(); 
  MinRun = 0; MaxRun = LITERALMAX;
}

RunMusher::RunMusher(string dirname, int min, int max) {
  dir = dirname.c_str();
  cout<<"Searching directory "<<dir.Data()<<" for segments of run: "<<run.Data();
  MinRun = min; MaxRun = max;
  cout<<" from run no. "<<MinRun<<" to run no. "<<MaxRun<<endl;
}

RunMusher::~RunMusher() {}

int RunMusher::GrabAllFiles() {
  TSystemDirectory sysdir(dir.Data(), dir.Data());
  TList *flist = sysdir.GetListOfFiles();
  int counter = 0;
  if(flist) {
    TSystemFile *file;
    TString fname, temp;
    TIter next_element(flist);
    while((file = (TSystemFile*)next_element())) {
      temp = file->GetName();
      if(!file->IsDirectory() && temp.EndsWith(".root")) {
        counter++;
        fname = dir+temp;
        cout<<"Found file: "<<fname.Data()<<endl;
        filelist.push_back(fname);
      } 
    }
    if(counter>0) {
      delete flist;
      return 1;
    } else {
      cerr<<"Unable to find files with matching run name in directory; check input.txt"<<endl;
      delete flist;
      return 0;
    }
  } else {
    cerr<<"Unable to find any files in directory; check name given to the input.txt"<<endl;
    delete flist;
    return 0;
  } 
}

int RunMusher::GrabFilesInRange() {
  TSystemDirectory sysdir(dir.Data(), dir.Data());
  TList *flist = sysdir.GetListOfFiles();
  int counter = 0;
  if(flist) {
    TSystemFile *file;
    TString fname, temp;
    string runno;
    for(int i=MinRun; i<=MaxRun; i++) {
      TIter next_element(flist);
      runno = to_string(i) + ".root";
      while((file = (TSystemFile*)next_element())) {
        temp = file->GetName();
        if(!file->IsDirectory()&&temp.EndsWith(runno.c_str())){
          counter++;
          fname = dir+temp;
          cout<<"Found file: "<<fname.Data();
          filelist.push_back(fname);
          break;
        }
      }
    }
    if(counter>0) {
      delete flist;
      return 1;
    } else {
      cerr<<"Unable to find files with matching run name in directory; check input.txt"<<endl;
      delete flist;
      return 0;
    }
  } else {
    cerr<<"Unable to find any files in directory; check name given to input.txt"<<endl;
    delete flist;
    return 0;
  }
}

int RunMusher::Merge(string outname) {
  TFile *output = new TFile(outname.c_str(), "RECREATE");
  TChain *chain = new TChain("SPSTree", "SPSTree");
  
  if(MaxRun == LITERALMAX) {
    if(GrabAllFiles()) { 
      for(unsigned int i=0; i<filelist.size(); i++) {
        chain->Add(filelist[i].Data());
      }
      cout<<"Merging runs into single file..."<<endl;
      chain->Merge(output,0,"fast");
      cout<<"Finished merging"<<endl;
      return 1;
    } else {
      return 0;
    }
  } else {
    if(GrabFilesInRange()) {
      for(unsigned int i=0; i<filelist.size(); i++) {
        chain->Add(filelist[i]);
      }
      cout<<"Merging runs "<<MinRun<<" to "<<MaxRun<<" into a single file..."<<endl;
      chain->Merge(output,0,"fast");
      cout<<"Finished merging"<<endl;
      return 1;
    } else {
      return 0;
    }
  }
  if(output->IsOpen()) output->Close();
  return 0;
}
