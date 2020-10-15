/*
	CompassRun.cpp
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020
*/
#include "EventBuilder.h"
#include "CompassRun.h"
#include "RunCollector.h"

CompassRun::CompassRun() :
	directory("")
{

}

CompassRun::CompassRun(std::string& dir) :
	directory(dir)
{

}

CompassRun::CompassRun(const char* dir) :
	directory(dir)
{

}

CompassRun::~CompassRun() {}

bool CompassRun::GetBinaryFiles() {
	std::string prefix = "";
	std::string suffix = ".bin"; //binaries
	RunCollector grabber(directory, prefix, suffix);
	grabber.GrabAllFiles();
	m_files.clear(); //so that the CompassRun can be reused
	m_files.reserve(grabber.filelist.size());
	for(auto& entry : grabber.filelist) {
		m_files.emplace_back(entry.Data());
		m_files[m_files.size()-1].AttachShiftMap(&m_smap);
		//Any time we have a file that fails to be found, we terminate the whole process
		if(!m_files[m_files.size() - 1].IsOpen()) {
			return false;
		}
	}

	return true;
}

/*
	GetHitsFromFiles() is the function which actually retrieves and sorts the data from the individual
	files. There are several tricks which allow this to happen. First is that, after sorting, it is impossible
	to determine which file the data originally came from (short of parsing the name of the file against board/channel).
	However, we need to let the file know that we want it to pull the next hit. To do this, a pointer to the UsedFlag of the file
	is retrieved along with the data. This flag is flipped so that on the next hit cycle a new hit is pulled. Second is the use
	of a rolling start index. Once a file has gone EOF, we no longer need it. If this is the first file in the list, we can just skip
	that index all together. In this way, the loop can go from N times to N-1 times.
*/
bool CompassRun::GetHitsFromFiles() {

	std::pair<CompassHit, bool*> earliestHit = make_pair(CompassHit(), nullptr);
	for(unsigned int i=startIndex; i<m_files.size(); i++) {
		if(m_files[i].CheckHitHasBeenUsed()) {
			m_files[i].GetNextHit();
		}

		if(m_files[i].IsEOF()) {
			if(i == startIndex) startIndex++;
			continue;
		} else if(i == startIndex) {
			earliestHit = make_pair(m_files[i].GetCurrentHit(), m_files[i].GetUsedFlagPtr());
		} else if(m_files[i].GetCurrentHit().timestamp < earliestHit.first.timestamp) {
			earliestHit = make_pair(m_files[i].GetCurrentHit(), m_files[i].GetUsedFlagPtr());
		}
	}

	if(earliestHit.second == nullptr) return false; //Make sure that there actually was a hit
	hit = earliestHit.first;
	*earliestHit.second = true;
	return true;
}

void CompassRun::Convert(std::string& name) {
	TFile* output = TFile::Open(name.c_str(), "RECREATE");
	TTree* outtree = new TTree("Data", "Data");

	outtree->Branch("Board", &hit.board);
	outtree->Branch("Channel", &hit.channel);
	outtree->Branch("Energy", &hit.lgate);
	outtree->Branch("EnergyShort", &hit.sgate);
	outtree->Branch("Timestamp", &hit.timestamp);
	outtree->Branch("Flags", &hit.flags);

	if(!m_smap.IsSet()) {
		std::cerr<<"Bad shift map at CompassRun::Convert()."<<std::endl;
		std::cerr<<"Shifts will be locked to 0"<<std::endl;
	}

	std::cout<<"Getting a mess of files..."<<std::endl;
	if(!GetBinaryFiles()) {
		std::cerr<<"Unable to open a file!"<<std::endl;
		return;
	}

	std::cout<<"Orgainizing data"<<std::endl;
	startIndex = 0; //Reset the startIndex
	while(true) {
		if(!GetHitsFromFiles()) break;
		outtree->Fill();
	}
	std::cout<<"Data orgainzed."<<std::endl;

	output->cd();
	outtree->Write(outtree->GetName(), TObject::kOverwrite);
	output->Close();
	std::cout<<"Data written."<<std::endl;
}

