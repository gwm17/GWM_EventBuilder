#ifndef COMPASS_UNPACKER_H
#define COMPASS_UNPACKER_H

#include "archive.h"
#include "archive_entry.h"
#include "CompassHit.h"


class CompassUnpacker {

  public:
    CompassUnpacker();
    ~CompassUnpacker();
    void VomitInfo(); /*for testing*/
    bool AttachToArchive(std::string& archive_name);
    bool AttachToFile(std::string& file_name);
    bool ReleaseCurrentArchive();
    bool ReleaseCurrentFile(); 
    void Run(std::string output_name);

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

    std::ifstream inputFile;
    struct archive *inputArchive;
    struct archive_entry *entry;
};

#endif
