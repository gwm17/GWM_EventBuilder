#ifndef COMPASS_UNPACKER_H
#define COMPASS_UNPACKER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "archive.h"
#include "archive_entry.h"
#include <TROOT.h>
#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include "CompassHit.h"

using namespace std;

class CompassUnpacker {

  public:
    CompassUnpacker(string file_name);
    ~CompassUnpacker();
    void VomitInfo(); /*for testing*/ 
    void Run(string output_name);

  private:
    void ResetHit();
    bool GetNextHit();
    bool ParseArchiveCompassHit();
    bool ParseFileCompassHit();
    bool MoveToNextFile();

    CompassHit hit, empty;
    bool archiveFlag;
    int archiveWarning;
    bool fatalFlag;
    size_t fileSize;
    size_t archiveSize;

    ifstream inputFile;
    struct archive *inputArchive;
    struct archive_entry *entry;
};

#endif
