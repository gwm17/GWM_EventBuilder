#include "CompassUnpacker.h"

using namespace std;

CompassUnpacker::CompassUnpacker(string file_name){
  fatalFlag = false;
  TString test(file_name.c_str());
  if(test.EndsWith(".tar.gz")) {
    archiveFlag = true;
    inputArchive = archive_read_new();
    archive_read_support_compression_all(inputArchive);
    archive_read_support_filter_all(inputArchive);
    archive_read_support_format_all(inputArchive);
    archiveWarning = archive_read_open_filename(inputArchive, file_name.c_str(), 10240);
    if(archiveWarning == ARCHIVE_FATAL) {
      cerr<<"Unable to open archive! Check file name in input."<<endl;
      fatalFlag = true;
    } else if (archiveWarning == ARCHIVE_WARN) {
      cerr<<"Archive opened with warnings!"<<endl;
      cerr<<"Warning given as: "<<archive_error_string(inputArchive)<<endl;
    } else if(archiveWarning == ARCHIVE_OK) {
      cout<<"Succesfully opened archive: "<<file_name<<endl;
      cout<<"Beginning unpacking..."<<endl;
    }
    archiveWarning = archive_read_next_header(inputArchive, &entry);
    cout<<"Number of files in archive: "<<archive_file_count(inputArchive)<<endl;
    if(archiveWarning != ARCHIVE_OK) {
      cerr<<"Unable to read first header, archive may be not correct format!"<<endl;
      fatalFlag = true;
    }
  } else {
    archiveFlag = false;
    inputArchive = NULL;
    inputFile.open(file_name, ios::binary);
    if(inputFile.is_open()) {
      cout<<"Succesfully opened file: "<<file_name<<endl;
      cout<<"Beginning unpacking..."<<endl;
    } else {
      cerr<<"Unable to open file! Check file name in input."<<endl;
      fatalFlag = true;
    }
  }
}

CompassUnpacker::~CompassUnpacker() {
  if(inputArchive) {
    archiveWarning = archive_read_free(inputArchive);
    if(archiveWarning != ARCHIVE_OK) {
      cerr<<"Error in releasing archive from memory"<<endl;
    }
  }
}

/*******TESTING*********/
void CompassUnpacker::VomitInfo() {
  cout<<"Dumping out all data:"<<endl;
  cout<<"Timestamp: "<<hit.timestamp<<" Energy: "<<hit.lgate<<" EnergyShort: "<<hit.sgate;
  cout<<" Board: "<<hit.board<<" Channel: "<<hit.channel<<endl;
  if(hit.board == 400 || hit.channel == 400) {
    cout<<"Illegal board/channel value, this one is dumped."<<endl;
  }
}

void CompassUnpacker::ResetHit() {
  hit = empty;
}

bool CompassUnpacker::GetNextHit() {
  bool eof;
  if(archiveFlag) {
   bool eoa;
   eof = ParseArchiveCompassHit();
   if(eof) {
     eoa = MoveToNextFile();
   } else {
     eoa = false;
   }
   return eoa; 
  } else {
    eof = ParseFileCompassHit();
    return eof;
  }
}

bool CompassUnpacker::ParseArchiveCompassHit() {
  bool eof = false;
  eof |= !archive_read_data(inputArchive,reinterpret_cast<void*>(&hit.board),2);
  eof |= !archive_read_data(inputArchive,reinterpret_cast<void*>(&hit.channel),2);
  eof |= !archive_read_data(inputArchive,reinterpret_cast<void*>(&hit.timestamp),8);
  eof |= !archive_read_data(inputArchive,reinterpret_cast<void*>(&hit.lgate),2);
  eof |= !archive_read_data(inputArchive,reinterpret_cast<void*>(&hit.sgate),2);
  eof |= !archive_read_data(inputArchive,reinterpret_cast<void*>(&hit.flags),4);
  eof |= !archive_read_data(inputArchive,reinterpret_cast<void*>(&hit.Ns),4);
  return eof;
}

bool CompassUnpacker::ParseFileCompassHit() {
  bool eof = false;
  eof |= !inputFile.read(reinterpret_cast<char*>(&hit.board),2);
  eof |= !inputFile.read(reinterpret_cast<char*>(&hit.channel),2);
  eof |= !inputFile.read(reinterpret_cast<char*>(&hit.timestamp),8);
  eof |= !inputFile.read(reinterpret_cast<char*>(&hit.lgate),2);
  eof |= !inputFile.read(reinterpret_cast<char*>(&hit.sgate),2);
  eof |= !inputFile.read(reinterpret_cast<char*>(&hit.flags),4);
  eof |= !inputFile.read(reinterpret_cast<char*>(&hit.Ns),4);
  return eof;
}


bool CompassUnpacker::MoveToNextFile() {
  archiveWarning = archive_read_next_header(inputArchive, &entry);
  switch(archiveWarning) {
    case(ARCHIVE_EOF):
      cout<<"Reached end of archive."<<endl;
      return true;
    case(ARCHIVE_WARN): 
      cerr<<"Received warning on accesing archive header"<<endl;
      cerr<<"Warning value: "<<archive_error_string(inputArchive)<<endl;
      cerr<<"Continuing operations..."<<endl;
      return false;
    case(ARCHIVE_FATAL):
      cerr<<"Received fatal signal from archive!"<<endl;
      cerr<<"Killing operations."<<endl;
      return true;
    case(ARCHIVE_OK):
      cout<<"Parsing next file in archive: "<<archive_entry_pathname(entry)<<endl;
      return false;
    case(ARCHIVE_FAILED):
      cerr<<"Undefined archive entry, unable to parse."<<endl;
      cerr<<"Killing operations"<<endl;
      return true;
  }
  cerr<<"Undefined archive behavior... Killing operations."<<endl;
  return true;
}

void CompassUnpacker::Run(string output_name) {
  if(fatalFlag) {
    cerr<<"Fatal error detected, ending program abnormally."<<endl;
    return;
  } else {
    TFile *outputFile = new TFile(output_name.c_str(), "RECREATE");
    TTree *outputTree = new TTree("Data", "Data");
    outputTree->Branch("Energy", &hit.lgate);
    outputTree->Branch("EnergyShort", &hit.sgate);
    outputTree->Branch("Timestamp", &hit.timestamp);
    outputTree->Branch("Channel", &hit.channel);
    outputTree->Branch("Board", &hit.board);
    outputTree->Branch("Flags", &hit.flags);

    bool endFlag = false;
    while(!endFlag) {
      ResetHit();
      endFlag = GetNextHit();
//      VomitInfo(); /***TESTING***/
      if(hit.board != 400 || hit.channel != 400) outputTree->Fill(); 
    }
    outputFile->cd();
    outputTree->Write(outputTree->GetName(), TObject::kOverwrite);
    outputFile->Close();
  }
}


