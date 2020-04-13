#include "EventBuilder.h"
#include "FastSort.h"

FastSort::FastSort(float si_windowSize, float ion_windowSize, string mapfile) {
  si_coincWindow = si_windowSize/1.0e3; //given in pico s, want in nano s
  ion_coincWindow = ion_windowSize/1.0e3; 
  event_address = NULL;

  if(!FillSabreMap(mapfile, smap)) illegalMap = 1;
  else illegalMap = 0;
}

FastSort::~FastSort() {
  delete event_address;
}

void FastSort::ResetSABRE() {
  for(int i=0; i<5; i++) {
    fastEvent.sabreArray[i] = sblank;
  }
}

void FastSort::ResetFocalPlane() {
  fastEvent.focalPlane = fpblank;
}

/*Assign a set of ion chamber data to the scintillator*/
void FastSort::ProcessFocalPlane(unsigned int scint_index, unsigned int ionch_index) {
  fastEvent.focalPlane.scintL.push_back(slowEvent.focalPlane.scintL[scint_index]);

  /*In order to have a coincidence window, one must choose a portion of the ion chamber to form a requirement.
   *In this case, I chose one of the anodes. But in principle you could also choose any other part of the ion
   *chamber
   */
  if(slowEvent.focalPlane.anodeF.size() > ionch_index) { //Front anode required to move on`
    fastEvent.focalPlane.anodeF.push_back(slowEvent.focalPlane.anodeF[ionch_index]);

    float anodeRelTime = fabs(fastEvent.focalPlane.anodeF[0].Time - fastEvent.focalPlane.scintL[0].Time);
    if(anodeRelTime > ion_coincWindow) return; //Window check

    if(slowEvent.focalPlane.delayFL.size() > ionch_index) {
      fastEvent.focalPlane.delayFL.push_back(slowEvent.focalPlane.delayFL[ionch_index]);
    }
    if(slowEvent.focalPlane.delayFR.size() > ionch_index) {
      fastEvent.focalPlane.delayFR.push_back(slowEvent.focalPlane.delayFR[ionch_index]);
    }
    if(slowEvent.focalPlane.delayBR.size() > ionch_index) {
      fastEvent.focalPlane.delayBR.push_back(slowEvent.focalPlane.delayBR[ionch_index]);
    }
    if(slowEvent.focalPlane.delayBL.size() > ionch_index) {
      fastEvent.focalPlane.delayBL.push_back(slowEvent.focalPlane.delayBL[ionch_index]);
    }
    if(slowEvent.focalPlane.scintR.size() > ionch_index) {
      fastEvent.focalPlane.scintR.push_back(slowEvent.focalPlane.scintR[ionch_index]);
    }
    if(slowEvent.focalPlane.anodeB.size() > ionch_index) {
      fastEvent.focalPlane.anodeB.push_back(slowEvent.focalPlane.anodeB[ionch_index]);
    }
    if(slowEvent.focalPlane.cathode.size() > ionch_index) {
      fastEvent.focalPlane.cathode.push_back(slowEvent.focalPlane.cathode[ionch_index]);
    }
  }
}

/*Assign a set of SABRE data that falls within the coincidence window*/
void FastSort::ProcessSABRE(unsigned int scint_index) {
  for(int i=0; i<5; i++) { //loop over SABRE silicons
    vector<DetectorHit> rings;
    vector<DetectorHit> wedges;
    /*Dump sabre data that doesnt fall within the fast coincidence window with the scint*/
    for(unsigned int j=0; j<slowEvent.sabreArray[i].rings.size(); j++) {
      float sabreRelTime = fabs(slowEvent.sabreArray[i].rings[j].Time - slowEvent.focalPlane.scintL[scint_index].Time);
      if(sabreRelTime < si_coincWindow) {
        rings.push_back(slowEvent.sabreArray[i].rings[j]);
      }
    }
    for(unsigned int j=0; j<slowEvent.sabreArray[i].wedges.size(); j++) {
      float sabreRelTime = fabs(slowEvent.sabreArray[i].wedges[j].Time - slowEvent.focalPlane.scintL[scint_index].Time);
      if(sabreRelTime < si_coincWindow) {
        wedges.push_back(slowEvent.sabreArray[i].wedges[j]);
      }
    }
  
    /*Now we need to dump unpaired SABRE (i.e. require both ring and wedge for good hit)
     *Since SABRE is sorted by descending enrgy and the standard method of pairing is to pair
     *highest energy front to highest energy back and so forth, all we need to do is dump the extras
     *from either vector
     */
    int unpairedSi = rings.size() -wedges.size(); 
    if(unpairedSi<0) { //too many wedges
      int depth = rings.size();
      if(depth == 0) {
        wedges.resize(0);
      } else {
        wedges.erase(wedges.begin()+depth, wedges.end()); //erase is [start, stop)
      }
      if(wedges.size() != rings.size()) {
        cerr<<endl<<"Failed si vector at wedge reduction!"<<endl;
        cerr<<"Ring size: "<<rings.size()<<" wedge size: "<<wedges.size()<<endl;
      } 
    } else if (unpairedSi>0) { //too many rings
      int depth = wedges.size();
      if(depth == 0) {
        rings.resize(0);
      } else {
        rings.erase(rings.begin()+depth, rings.end());
      }
      if(rings.size() != wedges.size()) {
        cerr<<endl<<"Failed si vector at ring reduction!"<<endl;
        cerr<<"Ring size: "<<rings.size()<<" wedge size: "<<wedges.size()<<endl;
      }
    }
    fastEvent.sabreArray[i].rings = rings;
    fastEvent.sabreArray[i].wedges = wedges;
  }
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
  for(ULong64_t i=0; i<intree->GetEntries(); i++) {
    intree->GetEntry(i);
    place = ((long double)i)/blentries*100;
    if(fmod(place,10.0) == 0) {
      cout<<"\rPercent of file processed: "<<place<<"%"<<flush;
    }
    slowEvent = *event_address;
    /*To know how many ion chamber sets there are, only need 
      to know the max size of all the parts*/
    unsigned int sizeArray[7];
    sizeArray[0] = slowEvent.focalPlane.delayFL.size();
    sizeArray[1] = slowEvent.focalPlane.delayFR.size();
    sizeArray[2] = slowEvent.focalPlane.delayBL.size();
    sizeArray[3] = slowEvent.focalPlane.delayBR.size();
    sizeArray[4] = slowEvent.focalPlane.anodeF.size();
    sizeArray[5] = slowEvent.focalPlane.anodeB.size();
    sizeArray[6] = slowEvent.focalPlane.cathode.size();
    unsigned int maxSize = *max_element(sizeArray, sizeArray+7);
    //loop over scints
    for(unsigned int i=0; i<slowEvent.focalPlane.scintL.size(); i++) {
      ResetSABRE();
      ProcessSABRE(i);
      //loop over ion chamber
      //NOTE: as written, this dumps data that does not have an ion chamber hit!
      //If you want scint/SABRE singles, move the fill outside of this loop
      for(unsigned int j=0; j<maxSize; j++) {
        ResetFocalPlane();
        ProcessFocalPlane(i, j);
        outtree->Fill();
      }
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
