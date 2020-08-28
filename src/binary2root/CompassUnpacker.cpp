#include "EventBuilder.h"
#include "CompassUnpacker.h"


CompassUnpacker::CompassUnpacker(){
  fatalFlag = true;
  archiveFlag = false;
  inputArchive = NULL;
}

CompassUnpacker::~CompassUnpacker() {
  if(inputArchive) {
    archiveWarning = archive_read_free(inputArchive);
    if(archiveWarning != ARCHIVE_OK) {
      std::cerr<<"Error in releasing archive from memory"<<std::endl;
    }
  }
}

bool CompassUnpacker::AttachToArchive(std::string& archive_name) {
  fatalFlag = true;
  TString test(archive_name.c_str());
  if(test.EndsWith(".tar.gz") && inputArchive == NULL) {
    archiveFlag = true;
    inputArchive = archive_read_new();
    archive_read_support_filter_all(inputArchive);
    archive_read_support_format_all(inputArchive);
    archiveWarning = archive_read_open_filename(inputArchive, archive_name.c_str(), 10240);
    if(archiveWarning == ARCHIVE_FATAL) {
      std::cerr<<"Unable to open archive! Check file name in input."<<std::endl;
    } else if (archiveWarning == ARCHIVE_WARN) {
      std::cerr<<"Archive opened with warnings!"<<std::endl;
      std::cerr<<"Warning given as: "<<archive_error_string(inputArchive)<<std::endl;
      fatalFlag = false;
    } else if(archiveWarning == ARCHIVE_OK) {
      std::cout<<"Succesfully opened archive: "<<archive_name<<std::endl;
      std::cout<<"Beginning unpacking..."<<std::endl;
      fatalFlag = false;
    }
    archiveWarning = archive_read_next_header(inputArchive, &entry);
    std::cout<<"Number of files in archive: "<<archive_file_count(inputArchive)<<std::endl;
    if(archiveWarning != ARCHIVE_OK) {
      std::cerr<<"Unable to read first header, archive may be not correct format!"<<std::endl;
    }
  } else {
    std::cerr<<"Formating is incorrect! Make sure that this is a .tar.gz archive and that the previous archive was freed"<<std::endl;
  }
  return !fatalFlag;
}

bool CompassUnpacker::ReleaseCurrentArchive() {
  if(inputArchive) {
    archiveWarning = archive_read_free(inputArchive);
    inputArchive = NULL;
    if(archiveWarning != ARCHIVE_OK) {
      std::cerr<<"Error in releasing archive from memory! Stopping binary2root"<<std::endl;
      return false;
    }
  }
  return true;
}

bool CompassUnpacker::AttachToFile(std::string& file_name) {
  fatalFlag = true;
  archiveFlag = false;
  inputArchive = NULL;
  if(!inputFile.is_open()) {
    inputFile.open(file_name, std::ios::binary);
    if(inputFile.is_open()) {
      std::cout<<"Succesfully opened file: "<<file_name<<std::endl;
      std::cout<<"Beginning unpacking..."<<std::endl;
      fatalFlag = false;
    } else {
      std::cerr<<"Unable to open file! Check file name in input."<<std::endl;
    }
  } else {
    std::cerr<<"Previous file was not closed! Stopping binary2root"<<std::endl;
  }
  return !fatalFlag;
}

bool CompassUnpacker::ReleaseCurrentFile() {
  if(inputFile.is_open()) {
    inputFile.close();
  }
  return true;
}

/*******TESTING*********/
void CompassUnpacker::VomitInfo() {
  std::cout<<"Dumping out all data:"<<std::endl;
  std::cout<<"Timestamp: "<<hit.timestamp<<" Energy: "<<hit.lgate<<" EnergyShort: "<<hit.sgate;
  std::cout<<" Board: "<<hit.board<<" Channel: "<<hit.channel<<std::endl;
  if(hit.board == 400 || hit.channel == 400) {
    std::cout<<"Illegal board/channel value, this one is dumped."<<std::endl;
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
      std::cout<<"Reached end of archive."<<std::endl;
      return true;
    case(ARCHIVE_WARN): 
      std::cerr<<"Received warning on accesing archive header"<<std::endl;
      std::cerr<<"Warning value: "<<archive_error_string(inputArchive)<<std::endl;
      std::cerr<<"Continuing operations..."<<std::endl;
      return false;
    case(ARCHIVE_FATAL):
      std::cerr<<"Received fatal signal from archive!"<<std::endl;
      std::cerr<<"Killing operations."<<std::endl;
      return true;
    case(ARCHIVE_OK):
      std::cout<<"Parsing next file in archive: "<<archive_entry_pathname(entry)<<std::endl;
      return false;
    case(ARCHIVE_FAILED):
      std::cerr<<"Undefined archive entry, unable to parse."<<std::endl;
      std::cerr<<"Killing operations"<<std::endl;
      return true;
  }
  std::cerr<<"Undefined archive behavior... Killing operations."<<std::endl;
  return true;
}

void CompassUnpacker::Run(std::string output_name) {
  if(fatalFlag) {
    std::cerr<<"Fatal error detected, ending program abnormally."<<std::endl;
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


