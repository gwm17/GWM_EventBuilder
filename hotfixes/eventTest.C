#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <THashTable.h>
#include <iostream>
#include <vector>
#include <string>
#include "include/DataStructs.h"
R__LOAD_LIBRARY(lib/libSPSDict.dylib)


void MyFill(THashTable *table, string name, int bins, double min, double max, double value) {
  TH1F* histo = (TH1F*) table->FindObject(name.c_str());
  if(histo == NULL) {
    TH1F* h = new TH1F(name.c_str(), name.c_str(), bins, min, max);
    h->Fill(value);
    table->Add(h);
  } else {
    histo->Fill(value);
  }
}

void MyFill(THashTable *table, string name, int binsx, double minx, double maxx, double valuex,
                                            int binsy, double miny, double maxy, double valuey) 
{
  TH2F* histo = (TH2F*) table->FindObject(name.c_str());
  if(histo == NULL) {
    TH2F* h = new TH2F(name.c_str(), name.c_str(), binsx, minx, maxx, binsy, miny, maxy);
    h->Fill(valuex, valuey);
    table->Add(h);
  } else {
    histo->Fill(valuex, valuey);
  }
}

void eventTest() {
  
  TFile *input = TFile::Open("/Volumes/LaCie/9BMarch2020/sorted/run_99.root", "READ");
  TTree *intree = (TTree*) input->Get("SortTree");

  CoincEvent *event_address = new CoincEvent;
  intree->SetBranchAddress("event", &event_address);

  TFile *output = TFile::Open("/Volumes/LaCie/9BMarch2020/eventtest.root","RECREATE");

  THashTable *hash = new THashTable();

  std::cout<<"Total number of entries: "<<intree->GetEntries()<<std::endl;
  for(Long64_t i=0; i<intree->GetEntries(); i++) {
    std::cout<<"\rNumber of entries proccessed: "<<i<<std::flush;
    intree->GetEntry(i);
    double anodeRelT;
    MyFill(hash,"scintSize_anodeSize",5,0,5,event_address->focalPlane.scintL.size(),5,0,5, event_address->focalPlane.anodeB.size());
    for(unsigned int j=0; j<event_address->focalPlane.scintL.size(); j++) {
      for(unsigned int k=0; k<event_address->focalPlane.anodeB.size(); k++) {
        anodeRelT = event_address->focalPlane.anodeB[k].Time-event_address->focalPlane.scintL[j].Time;
        MyFill(hash,"anodeRelT_all_combos",1000,-3000,3000,anodeRelT);
      }
    }
  }

  input->Close();
  hash->Write();
  output->Close();
  delete event_address;
} 
