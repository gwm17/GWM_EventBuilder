/*RunCollector.cpp
 *Class that searches through a directory looking for files of a specified format.
 *Stores all filenames in a vector which can be accessed by other functions/classes for
 *further use. Can also use Merge() to combine all files using hadd into a single file.
 *Merge() is NOT RECOMMENDED in the analyzer program.
 *
 *Created Jan2020 by GWM
 */
#include "RunCollector.h"

using namespace std;

RunCollector::RunCollector(string dirname) {
  dir = dirname.c_str();
  run = ""; 
  cout<<"Searching directory: "<<dir.Data()<<" for segments of run: "<<run.Data()<<endl; 
  MinRun = 0; MaxRun = LITERALMAX; //if max and min not specified, the class limit is used
}

RunCollector::RunCollector(string dirname, int min, int max) {
  dir = dirname.c_str();
  run = ""; 
  cout<<"Searching directory "<<dir.Data()<<" for segments of run: "<<run.Data();
  MinRun = min; MaxRun = max;
  cout<<" from run no. "<<MinRun<<" to run no. "<<MaxRun<<endl;
}

RunCollector::~RunCollector() {}

/*Stores all filenames in a given directory that match format*/
int RunCollector::GrabAllFiles() {
  TSystemDirectory sysdir(dir.Data(), dir.Data());
  TList *flist = sysdir.GetListOfFiles();
  int counter = 0;
  if(flist) { //Make sure list is real. If not, means no directory
    TSystemFile *file;
    TString fname, temp;
    TIter next_element(flist); //List iterator
    while((file = (TSystemFile*)next_element())) {
      temp = file->GetName();
      if(!file->IsDirectory() && temp.BeginsWith(run.Data()) && temp.EndsWith(".tar.gz")) {
        counter++;
        fname = dir+temp; //need fullpath for other functions /*no longer true, just useful for cout*/
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

/*Grabs all files within a specified run range*/
int RunCollector::GrabFilesInRange() {
  TSystemDirectory sysdir(dir.Data(), dir.Data());
  TList *flist = sysdir.GetListOfFiles();
  int counter = 0;
  if(flist) {
    TSystemFile *file;
    TString fname, temp;
    string runno;
    for(int i=MinRun; i<=MaxRun; i++) {//loop over range
      TIter next_element(flist);//list iterator
      runno = "_"+to_string(i) + ".tar.gz"; //suffix is now #.tar.gz
      while((file = (TSystemFile*)next_element())) {//look through directory until file found
        temp = file->GetName();
        if(!file->IsDirectory()&&temp.BeginsWith(run.Data())&&temp.EndsWith(runno.c_str())){
          counter++;
          fname = dir+temp;
          cout<<"Found file: "<<fname.Data()<<endl;
          filelist.push_back(fname);
          break; //if we find the file, break out of iterator loop
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

int RunCollector::Merge(string outname) {
  if(MaxRun == LITERALMAX) {
    if(GrabAllFiles()) { 
      TString clump = "hadd "+outname;
      for(unsigned int i=0; i<filelist.size(); i++) {
        clump += " "+filelist[i];
      }
      cout<<"Merging runs into single file..."<<endl;
      system(clump.Data());
      cout<<"Finished merging"<<endl;
      return 1;
    } else {
      return 0;
    }
  } else {
    if(GrabFilesInRange()) {
      TString clump = "hadd "+outname;
      for(unsigned int i=0; i<filelist.size(); i++) {
        clump += " "+filelist[i];
      }
      cout<<"Merging runs "<<MinRun<<" to "<<MaxRun<<" into a single file..."<<endl;
      system(clump.Data());
      cout<<"Finished merging"<<endl;
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}
