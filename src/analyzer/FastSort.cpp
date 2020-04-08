#include "FastSort.h"

FastSort::FastSort(float windowSize) {
  coincWindow = windowSize/1.0e3; //given in pico s, want in nano s
  event_address = NULL;

  if(!FillSabreMap(smap)) illegalMap = 1;
  else illegalMap = 0;
}

FastSort::~FastSort() {
  delete event_address;
}

void FastSort::Reset() {
  fastEvent = blank;
}

void FastSort::ProcessFocalPlane(unsigned long iter) {
  /*Time sequential events. That is the first scint should correspond to the 
   *first delay, first anode, etc.
   */
  fastEvent.scintL = slowEvent.scintL[iter];
  if(slowEvent.delayFL.size() != 0) {
    /* There are simpler methods for this. But,
     * we may need a more complicted builder.
     */
    fastEvent.delayFL = slowEvent.delayFL[0];
    slowEvent.delayFL.erase(slowEvent.delayFL.begin()); 
  }
  if(slowEvent.delayFR.size() != 0) {
    fastEvent.delayFR = slowEvent.delayFR[0];
    slowEvent.delayFR.erase(slowEvent.delayFR.begin());
  }
  if(slowEvent.delayBR.size() != 0) {
    fastEvent.delayBR = slowEvent.delayBR[0];
    slowEvent.delayBR.erase(slowEvent.delayBR.begin());
  }
  if(slowEvent.delayBL.size() != 0) {
    fastEvent.delayBL = slowEvent.delayBL[0];
    slowEvent.delayBL.erase(slowEvent.delayBL.begin());
  }
  if(slowEvent.scintR.size() != 0) {
    fastEvent.scintR = slowEvent.scintR[0];
    slowEvent.scintR.erase(slowEvent.scintR.begin());
  }
  if(slowEvent.anodeF.size() != 0) {
    fastEvent.anodeF = slowEvent.anodeF[0];
    slowEvent.anodeF.erase(slowEvent.anodeF.begin());
  }
  if(slowEvent.anodeB.size() != 0) {
    fastEvent.anodeB = slowEvent.anodeB[0];
    slowEvent.anodeB.erase(slowEvent.anodeB.begin());
  }
  if(slowEvent.cathode.size() != 0) {
    fastEvent.cathode = slowEvent.cathode[0];
    slowEvent.cathode.erase(slowEvent.cathode.begin());
  }
}

void FastSort::ProcessSABRE() {
  vector<SabreHit> fronts;
  vector<SabreHit> backs;
  /*Dump sabre data that doesnt fall within the fast coincidence window with the scint*/
  for(unsigned long i=0; i<slowEvent.sabreFrontData.size(); i++) {
    float sabreRelTime = fabs(slowEvent.sabreFrontData[i].Time - fastEvent.scintL.Time);
    if(sabreRelTime < coincWindow) {
      fronts.push_back(slowEvent.sabreFrontData[i]);
      slowEvent.sabreFrontData.erase(slowEvent.sabreFrontData.begin()+i);
    }
  }
  for(unsigned long i=0; i<slowEvent.sabreBackData.size(); i++) {
    float sabreRelTime = fabs(slowEvent.sabreBackData[i].Time - fastEvent.scintL.Time);
    if(sabreRelTime < coincWindow) {
      backs.push_back(slowEvent.sabreBackData[i]);
      slowEvent.sabreBackData.erase(slowEvent.sabreBackData.begin()+i);
    }
  }
  
  /*Now we need to dump unpaired SABRE (i.e. require both front and back for good hit)
   *Since SABRE is sorted by descending enrgy and the standard method of pairing is to pair
   *highest energy front to highest energy back and so forth, all we need to do is dump the extras
   *from either vector
   */
  int unpairedSi = fronts.size() -backs.size(); 
  if(unpairedSi<0) { //too many backs
    int depth = fronts.size();
    if(depth == 0) {
      backs.resize(0);
    } else {
      backs.erase(backs.begin()+depth, backs.end()); //erase is [start, stop)
    }
    if(backs.size() != fronts.size()) {
      cerr<<endl<<"Failed si vector at back reduction!"<<endl;
      cerr<<"Front size: "<<fronts.size()<<" Back size: "<<backs.size()<<endl;
    } else {
      fastEvent.sabreFrontData = fronts;
      fastEvent.sabreBackData = backs;
    }
  } else if (unpairedSi>0) { //too many fronts
    int depth = backs.size();
    if(depth == 0) {
      fronts.resize(0);
    } else {
      fronts.erase(fronts.begin()+depth, fronts.end());
    }
    if(fronts.size() != backs.size()) {
      cerr<<endl<<"Failed si vector at front reduction!"<<endl;
      cerr<<"Front size: "<<fronts.size()<<" Back size: "<<backs.size()<<endl;
    } else {
      fastEvent.sabreBackData = backs;
      fastEvent.sabreFrontData = fronts;
    }
  } else { //juuuust right
    fastEvent.sabreFrontData = fronts;
    fastEvent.sabreBackData = backs;
  }
  fastEvent.sabreFrontMult = fastEvent.sabreFrontData.size();
  fastEvent.sabreBackMult = fastEvent.sabreBackData.size();
}

void FastSort::Run(const char *infile_name, const char *outfile_name) {
  if(illegalMap) {
    cerr<<"Unable to process with illegal map!"<<endl;
    return;
  }
  TFile *input = new TFile(infile_name, "READ");
  TTree *intree = (TTree*) input->Get("SortTree");
  TFile *output = new TFile(outfile_name, "RECREATE");
  TTree *outtree = new TTree("SortTree","SortTree");

  intree->SetBranchAddress("event",&event_address);
  
  outtree->Branch("event", &fastEvent);
  Float_t blentries = intree->GetEntries();
  cout<<setprecision(5)<<fixed;
  Float_t place;
  for(ULong_t i=0; i<intree->GetEntries(); i++) {
    intree->GetEntry(i);
    place = ((long double)i)/blentries*100;
    if(fmod(place,10.0) == 0) {
      cout<<"\rPercent of file processed: "<<place<<"%"<<flush;
    }
    slowEvent = *event_address;
    for(unsigned int i=0; i<slowEvent.scintL.size(); i++) {
      Reset();
      ProcessFocalPlane(i);
      ProcessSABRE();
      outtree->Fill();
    }
  }
  cout<<endl;
  input->Close();
  output->cd();
  outtree->Write(outtree->GetName(), TObject::kOverwrite);
  output->Close();
  delete output;
  delete input;
}
