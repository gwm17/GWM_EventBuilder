/*RealTimer.cpp
 *Class used to shift timestamps of the SABRE detector and the scintillator
 *of the focal plane, so that time-order is approximately fp ion-chamber,
 *scintillator, SABRE. This is for ease of event-building.
 *Based on code written by S. Balak, K. Macon, E. Good from LSU
 *
 *Created Jan 2020 by GWM
 */

#include "RealTimer.h"

using namespace std;

/*Take unsigned int inputs and store in signed ints, as this is the type of the timestamp*/
RealTimer::RealTimer(UInt_t si, UInt_t plast) {
  SI_OFFSET = si;
  SCINT_OFFSET = plast;
  if(!FillSabreMap(smap)) illegalMap = 1;
  else illegalMap = 0;
}

RealTimer::~RealTimer(){}

/*Actual work of class*/
void RealTimer::Run(string in, string out) {
  if(illegalMap) {
    cerr<<"Unable to timeshift with illegal map!"<<endl;
    return;
  }

  TFile *input = new TFile(in.c_str(), "READ");
  TTree *intree = (TTree*) input->Get("Data");
   
  intree->SetBranchAddress("Energy",&hit.Energy);
  intree->SetBranchAddress("EnergyShort", &hit.EnergyShort);
  intree->SetBranchAddress("Timestamp", &hit.Timestamp);
  intree->SetBranchAddress("Channel", &hit.Channel);
  intree->SetBranchAddress("Board", &hit.Board);
  intree->SetBranchAddress("Flags", &hit.Flags);

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
  for(Int_t i=0; i<intree->GetEntries(); i++) {
    intree->GetEntry(i);
    place = ((Float_t)i)/blentries*100;
    if(fmod(place,10.0) ==  0) { /*Non-continuous progress update*/
      cout<<"\rFile is "<<place<<"%"<<" complete"<<flush;
    }
    gchan = hit.Board*16+hit.Channel;
    if(smap[gchan].side_pos.first=="FRONT" || smap[gchan].side_pos.first=="BACK"){
      if(hit.Board == 3) { /*GWM temporary measure to account for board offsets... these should be for each board, not just scint & si*/
        hit.Timestamp += (SI_OFFSET-3e5);
      } else {
        hit.Timestamp += SI_OFFSET;
      }
    } else if (hit.Board == 8 && (hit.Channel == 0 || hit.Channel == 1)) {/*Scint*/
      hit.Timestamp += SCINT_OFFSET;
    }
    outtree->Fill();
  }
  cout<<endl;
  cout<<"Timestamps succesfully shifted"<<endl;
  input->Close();
  output->cd();
  outtree->Write(outtree->GetName(), TObject::kOverwrite);
  output->Close();
  delete input;
  delete output;
}
