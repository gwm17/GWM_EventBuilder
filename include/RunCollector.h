/*RunCollector.h
 *Class that searches through a directory looking for files of a specified format.
 *Stores all filenames in a vector which can be accessed by other functions/classes for
 *further use. Can also use Merge() to combine all files using hadd into a single file.
 *Merge() is NOT RECOMMENDED in the analyzer program.
 *
 *Created Jan2020 by GWM
 */

#ifndef RUNCOLLECTOR_H
#define RUNCOLLECTOR_H

#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TCollection.h>
#include <TList.h>
#include <cstdlib>
#include <cstdio>

using namespace std;

class RunCollector {
  public:
    RunCollector(string dirname, string prefix, string suffix);
    RunCollector(string dirname, string prefix, string suffix, int min, int max);
    ~RunCollector();
    int Merge_hadd(string outname);
    int Merge_TChain(string outname);
    int GrabAllFiles();
    int GrabFilesInRange();
    vector<TString> filelist;

  private:
    TString dir;
    TString run;
    TString end;
    Int_t MaxRun, MinRun;  //user run limits
    const Int_t LITERALMAX = 1000; //class run limit
};

#endif
