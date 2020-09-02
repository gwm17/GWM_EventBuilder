/*TimeShifter.cpp
 *Class used to shift timestamps of the SABRE detector and the scintillator
 *of the focal plane, so that time-order is such that the scintllator, SABRE,
 *and a specific piece of the ion chamber (usually an anode) are all coincident
 *This is for ease of event-building and to allow for the narrowest possible
 *coincidence windows.
 *Based on code written by S. Balak, K. Macon, E. Good from LSU
 *
 *Created Jan 2020 by GWM
 */

#include "EventBuilder.h"
#include "TimeShifter.h"

using namespace std;

bool MySort(DPPChannel& i, DPPChannel& j) {
  return (i.Timestamp<j.Timestamp);
}

/*Take unsigned int inputs and store in signed ints, as this is the type of the timestamp*/
TimeShifter::TimeShifter(Int_t plast, string mapfile, string shiftfile) {
  SCINT_OFFSET = plast;
  if(!FillSabreMap(mapfile, smap)) illegalMap = 1;
  else illegalMap = 0;

  shifts.SetShiftMap(shiftfile);
}

TimeShifter::~TimeShifter(){}

/*Actual work of class*/
void TimeShifter::Run(string in, string out) {
  vector<DPPChannel> entry_list;
  if(illegalMap) {
    cerr<<"Unable to timeshift with illegal map!"<<endl;
    return;
  }

  TFile *input = new TFile(in.c_str(), "READ");
  TTree *intree = (TTree*) input->Get("Data");
  
  ULong64_t ts;
  UShort_t es, e, c, b;
  string name;
  UInt_t f; 
  intree->SetBranchAddress("Energy",&e);
  intree->SetBranchAddress("EnergyShort", &es);
  intree->SetBranchAddress("Timestamp", &ts);
  intree->SetBranchAddress("Channel", &c);
  intree->SetBranchAddress("Board", &b);
  intree->SetBranchAddress("Flags", &f);

  TFile *output = new TFile(out.c_str(), "RECREATE");
  TTree *outtree  = new TTree("Data", "RealTime Data");
  
  outtree->Branch("Energy",&hit.Energy);
  outtree->Branch("EnergyShort", &hit.EnergyShort);
  outtree->Branch("Timestamp", &hit.Timestamp);
  outtree->Branch("Channel", &hit.Channel);
  outtree->Branch("Board", &hit.Board);
  outtree->Branch("Flags", &hit.Flags);
  
  Int_t gchan;
  
  cout<<"Shifting the timestamps of SABRE and the scintillator..."<<endl;
  Float_t blentries = intree->GetEntries();
  Float_t place;

  entry_list.reserve(intree->GetEntries());
  for(ULong64_t i=0; i<intree->GetEntries(); i++) {
    intree->GetEntry(i);

    /*Convert out of unsigned land (that we were in for no reason other than I guess bit minimization)*/
    hit.Energy = e;
    hit.EnergyShort = es;
    hit.Channel = c;
    hit.Board = b;
    hit.Timestamp = ts;
    hit.Flags = f;

    place = ((long double)i)/blentries*100;
    if(fmod(place,10.0) ==  0) { /*Non-continuous progress update*/
      cout<<"\rFile is "<<place<<"%"<<" complete"<<flush;
    }
    gchan = hit.Board*16+hit.Channel;
    if(smap[gchan].side_pos.first=="RING" || smap[gchan].side_pos.first=="WEDGE"){
      hit.Timestamp += shifts.GetShift(hit.Board);
    } else if (hit.Board == 8 && (hit.Channel == 0 || hit.Channel == 1)) {/*Scint*/
      hit.Timestamp += SCINT_OFFSET;
    }
    entry_list.push_back(hit);
    //outtree->Fill();
  }
  cout<<endl;
  cout<<"Timestamps succesfully shifted"<<endl;
  input->Close();

  //Time-order data
  sort(entry_list.begin(), entry_list.end(), MySort);
  cout<<"Time-ordering data..."<<endl;
  for(auto& entry: entry_list) {
    hit = entry;
    outtree->Fill();
  }
  cout<<"Data written in order."<<endl;

  output->cd();
  outtree->Write(outtree->GetName(), TObject::kOverwrite);
  output->Close();
  delete input;
  delete output;

}
