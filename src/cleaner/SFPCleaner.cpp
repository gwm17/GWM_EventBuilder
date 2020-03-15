/*SFPCleaner.h
 *Class for generating histogram files for SPS-SABRE data
 *Intended use case is generating a TChain of multiple analyzed files and making
 *histograms of the larger data set.
 *
 *Created Jan 2020 by GWM
 */

#include "SFPCleaner.h"

/*Generates storage and initializes pointers*/
SFPCleaner::SFPCleaner() {
  rootObj = new THashTable();
  rootObj->SetOwner(false);//THashTable doesnt own members; avoid double delete
  event_address = new ProcessedEvent();
  treeFlag = false;
  afile = NULL;
  hefile = NULL;
  xfile = NULL;
  alphaCut = NULL, he3Cut = NULL, x1x2Cut = NULL;
  chain = new TChain("SPSTree");
}

/*UNUSED*/
SFPCleaner::SFPCleaner(bool tf) {
  rootObj = new THashTable();
  rootObj->SetOwner(false);
  event_address = new ProcessedEvent();
  treeFlag = tf;
  afile = NULL;
  hefile = NULL;
  xfile = NULL;
  alphaCut = NULL, he3Cut = NULL, x1x2Cut = NULL;
}

SFPCleaner::~SFPCleaner() {
  delete event_address;
  if(afile != NULL && afile->IsOpen()) afile->Close();
  if(hefile != NULL && hefile->IsOpen()) hefile->Close();
  if(xfile != NULL && xfile->IsOpen()) xfile->Close();
}

/*2D histogram fill wrapper*/
void SFPCleaner::MyFill(string name, int binsx, double minx, double maxx, double valuex,
                                      int binsy, double miny, double maxy, double valuey) {
  TH2F *histo = (TH2F*) rootObj->FindObject(name.c_str());
  if(histo != NULL) {
    histo->Fill(valuex, valuey);
  } else {
    TH2F *h = new TH2F(name.c_str(), name.c_str(), binsx, minx, maxx, binsy, miny, maxy);
    h->Fill(valuex, valuey);
    rootObj->Add(h);
  }
}

/*1D histogram fill wrapper*/
void SFPCleaner::MyFill(string name, int binsx, double minx, double maxx, double valuex) {
  TH1F *histo = (TH1F*) rootObj->FindObject(name.c_str());
  if(histo != NULL) {
    histo->Fill(valuex);
  } else {
    TH1F *h = new TH1F(name.c_str(), name.c_str(), binsx, minx, maxx);
    h->Fill(valuex);
    rootObj->Add(h);
  }
}

void SFPCleaner::Reset() {
  event = empty;
}

/*Function for user to set the cutfiles; 
 *has to be used in current design!
 *User will probably want to change number of cuts and their names
 */
int SFPCleaner::SetCuts(string alphaname, string he3name, string x1x2name) {
  afile = new TFile(alphaname.c_str(), "READ");
  if(afile->IsOpen()) {
    alphaCut = (TCutG*) afile->Get("CUTG");
    alphaCut->SetName("alphaCut");
    rootObj->Add(alphaCut);
  }
  hefile = new TFile(he3name.c_str(), "READ");
  if(hefile->IsOpen()) {
    he3Cut = (TCutG*) hefile->Get("CUTG");
    he3Cut->SetName("he3Cut");
    rootObj->Add(he3Cut);
  }
  xfile = new TFile(x1x2name.c_str(), "READ");
  if(xfile->IsOpen()) {
    x1x2Cut = (TCutG*) xfile->Get("CUTG");
    x1x2Cut->SetName("x1x2Cut");
    rootObj->Add(x1x2Cut);
  }
  if(alphaCut != NULL && he3Cut != NULL && x1x2Cut != NULL) {
    return 1;
  } else {
    return 0;
  }
}

/*Makes histograms where only rejection is unset data*/
void SFPCleaner::MakeUncutHistograms(ProcessedEvent ev) {
  if(ev.x1 != -1e6 && ev.x2 != -1e6) {
    MyFill("x1NoCuts_bothplanes",600,-300,300,ev.x2);
    MyFill("x2NoCuts_bothplanes",600,-300,300,ev.x2);
    MyFill("xavgNoCuts_bothplanes",600,-300,300,ev.xavg);
    
    MyFill("x1_delayBackRightE_NoCuts",600,-300,300,ev.x1,512,0,4096,ev.delayBackRightE);
    MyFill("x2_delayBackRightE_NoCuts",600,-300,300,ev.x2,512,0,4096,ev.delayBackRightE);
    MyFill("xavg_delayBackRightE_NoCuts",600,-300,300,ev.xavg,512,0,4096,ev.delayBackRightE);

    Double_t delayBackAvgE = (ev.delayBackRightE+ev.delayBackLeftE)/2.0;
    MyFill("x1_delayBackAvgE_NoCuts",600,-300,300,ev.x1,512,0,4096,delayBackAvgE);
    MyFill("x2_delayBackAvgE_NoCuts",600,-300,300,ev.x2,512,0,4096,delayBackAvgE);
    MyFill("xavg_delayBackAvgE_NoCuts",600,-300,300,ev.xavg,512,0,4096,delayBackAvgE);
    Double_t delayFrontAvgE = (ev.delayFrontRightE+ev.delayFrontLeftE)/2.0;
    MyFill("x1_delayFrontAvgE_NoCuts",600,-300,300,ev.x1,512,0,4096,delayFrontAvgE);
    MyFill("x2_delayFrontAvgE_NoCuts",600,-300,300,ev.x2,512,0,4096,delayFrontAvgE);
    MyFill("xavg_delayFrontAvgE_NoCuts",600,-300,300,ev.xavg,512,0,4096,delayFrontAvgE);

    MyFill("scintLeft_anodeBack_NoCuts",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeBack);
    MyFill("scintLeft_anodeFront_NoCuts",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeFront);

    /****Timing relative to back anode****/
    if(ev.anodeBackTime != -1) {
      Double_t sabreRelFT = ev.sabreFrontTime - ev.anodeBackTime;
      Double_t sabreRelBT = ev.sabreBackTime - ev.anodeBackTime;
      Double_t anodeRelFT = ev.anodeFrontTime - ev.anodeBackTime;
      Double_t anodeRelBT = ev.anodeBackTime - ev.anodeBackTime;
      MyFill("sabreRelFrontTime_NoCuts",1000,-3000,3500, sabreRelFT);
      MyFill("sabreRelBackTime_NoCuts",1000,-3000,3500, sabreRelBT);
      MyFill("anodeRelBackTime_NoCuts",1000,-3000,3500, anodeRelBT);
      MyFill("anodeRelFrontTime_NoCuts",1000,-3000,3500, anodeRelFT);
      if(ev.sabreFrontE != -1 && sabreRelFT<1.0 && sabreRelFT>-1.0) {
        MyFill("xavg_sabrefcoinc_NoCuts",600,-300,300, ev.xavg);
      }
      if(ev.scintLeftTime != -1) {
        Double_t sabreRelFT_toScint = ev.sabreFrontTime - ev.scintLeftTime;
        Double_t anodeRelT = ev.anodeBackTime - ev.scintLeftTime;
        MyFill("sabreRelFrontTime_toScint",1000,-3000,3500,sabreRelFT_toScint);
        MyFill("sabreRelFTScint_sabreRelFTAnode",500,-3000,3500,sabreRelFT_toScint,500,-3000,3500,sabreRelFT);
        MyFill("sabreRelFTScint_anodeRelT",500,-3000,3500,sabreRelFT_toScint,500,-3000,3500,anodeRelT);
      }
    } else {
      MyFill("noscinttime_counter_NoCuts",2,0,1,1);
    }
    
    if(ev.sabreFrontE != -1) {
      MyFill("sabreFrontE_NoCuts",4096,0,4095,ev.sabreFrontE);
      MyFill("sabreChannelFront_sabreFrontE_NoCuts",144,0,144,ev.sabreChannelFront,512,0,4095,ev.sabreFrontE);
    }
    if(ev.sabreBackE != -1) {
      MyFill("sabreBackE_NoCuts",4096,0,4096,ev.sabreBackE);
      MyFill("sabreChannelBack_sabreBackE_NoCuts",144,0,144,ev.sabreChannelBack,512,0,4095,ev.sabreBackE);
    }
  } else if(ev.x1 != -1e6) {
    MyFill("x1NoCuts_only1plane",600,-300,300,ev.x1);
  } else if(ev.x2 != -1e6) {
    MyFill("x2NoCuts_only1plane",600,-300,300,ev.x2);
  } else {
    MyFill("nopos_counter",2,0,1,1);
  }
}

/*Makes histograms with cuts & gates implemented*/
void SFPCleaner::MakeCutHistograms(ProcessedEvent ev) {
  if(alphaCut->IsInside(ev.x1, ev.delayBackRightE) && x1x2Cut->IsInside(ev.x1, ev.x2)) {
    MyFill("x1_bothplanes_acut",600,-300,300,ev.x2);
    MyFill("x2_bothplanes_acut",600,-300,300,ev.x2);
    MyFill("xavg_bothplanes_acut",600,-300,300,ev.xavg);
    
    MyFill("x1_delayBackRightE_acut",600,-300,300,ev.x1,512,0,4096,ev.delayBackRightE);
    MyFill("x2_delayBackRightE_acut",600,-300,300,ev.x2,512,0,4096,ev.delayBackRightE);
    MyFill("xavg_delayBackRightE_acut",600,-300,300,ev.xavg,512,0,4096,ev.delayBackRightE);

    Double_t delayBackAvgE = (ev.delayBackRightE+ev.delayBackLeftE)/2.0;
    MyFill("x1_delayBackAvgE_acut",600,-300,300,ev.x1,512,0,4096,delayBackAvgE);
    MyFill("x2_delayBackAvgE_acut",600,-300,300,ev.x2,512,0,4096,delayBackAvgE);
    MyFill("xavg_delayBackAvgE_acut",600,-300,300,ev.xavg,512,0,4096,delayBackAvgE);
    Double_t delayFrontAvgE = (ev.delayFrontRightE+ev.delayFrontLeftE)/2.0;
    MyFill("x1_delayFrontAvgE_acut",600,-300,300,ev.x1,512,0,4096,delayFrontAvgE);
    MyFill("x2_delayFrontAvgE_acut",600,-300,300,ev.x2,512,0,4096,delayFrontAvgE);
    MyFill("xavg_delayFrontAvgE_acut",600,-300,300,ev.xavg,512,0,4096,delayFrontAvgE);

    MyFill("scintLeft_anodeBack_acut",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeBack);
    MyFill("scintLeft_anodeFront_acut",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeFront);

    /****Timing relative to back anode****/
    if(ev.anodeBackTime != -1) {
      Double_t sabreRelFT = ev.sabreFrontTime - ev.anodeBackTime;
      Double_t sabreRelBT = ev.sabreBackTime - ev.anodeBackTime;
      Double_t anodeRelFT = ev.anodeFrontTime - ev.anodeBackTime;
      Double_t anodeRelBT = ev.anodeBackTime - ev.anodeBackTime;
      MyFill("sabreRelFrontTime_acut",1000,-3000,3500, sabreRelFT);
      MyFill("sabreRelBackTime_acut",1000,-3000,3500, sabreRelBT);
      MyFill("anodeRelBackTime_acut",1000,-3000,3500, anodeRelBT);
      MyFill("anodeRelFrontTime_acut",1000,-3000,3500, anodeRelFT);
      if(ev.sabreFrontE != -1 && sabreRelFT<1.0 && sabreRelFT>-1.0) {
        MyFill("xavg_acut_sabrefcoinc_strict",600,-300,300, ev.xavg);
      }
    } else {
      MyFill("noscinttime_counter_acut",2,0,1,1);
    }
    
    if(ev.sabreFrontE != -1) {
      MyFill("sabreFrontE_acut",4096,0,4096,ev.sabreFrontE);
      MyFill("xavg_acut_sabrefcoinc",600,-300,300,ev.xavg);
      MyFill("xavg_sabreFrontE_acut",600,-300,300,ev.xavg,512,0,4096,ev.sabreFrontE);
    }
    if(ev.sabreBackE != -1) {
      MyFill("sabreBackE_acut",4096,0,4096,ev.sabreBackE);
      MyFill("xavg_sabreBackE_acut",600,-300,300,ev.xavg,512,0,4096,ev.sabreBackE);
    }
  } else if (he3Cut->IsInside(ev.x1, ev.delayBackRightE) && x1x2Cut->IsInside(ev.x1, ev.x2)) {
    MyFill("x1_bothplanes_hcut",600,-300,300,ev.x2);
    MyFill("x2_bothplanes_hcut",600,-300,300,ev.x2);
    MyFill("xavg_bothplanes_hcut",600,-300,300,ev.xavg);
    
    MyFill("x1_delayBackRightE_hcut",600,-300,300,ev.x1,512,0,4096,ev.delayBackRightE);
    MyFill("x2_delayBackRightE_hcut",600,-300,300,ev.x2,512,0,4096,ev.delayBackRightE);
    MyFill("xavg_delayBackRightE_hcut",600,-300,300,ev.xavg,512,0,4096,ev.delayBackRightE);

    Double_t delayBackAvgE = (ev.delayBackRightE+ev.delayBackLeftE)/2.0;
    MyFill("x1_delayBackAvgE_hcut",600,-300,300,ev.x1,512,0,4096,delayBackAvgE);
    MyFill("x2_delayBackAvgE_hcut",600,-300,300,ev.x2,512,0,4096,delayBackAvgE);
    MyFill("xavg_delayBackAvgE_hcut",600,-300,300,ev.xavg,512,0,4096,delayBackAvgE);
    Double_t delayFrontAvgE = (ev.delayFrontRightE+ev.delayFrontLeftE)/2.0;
    MyFill("x1_delayFrontAvgE_hcut",600,-300,300,ev.x1,512,0,4096,delayFrontAvgE);
    MyFill("x2_delayFrontAvgE_hcut",600,-300,300,ev.x2,512,0,4096,delayFrontAvgE);
    MyFill("xavg_delayFrontAvgE_hcut",600,-300,300,ev.xavg,512,0,4096,delayFrontAvgE);

    MyFill("scintLeft_anodeBack_hcut",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeBack);
    MyFill("scintLeft_anodeFront_hcut",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeFront);

    /****Timing relative to back anode****/
    if(ev.scintLeftTime != -1) {
      Double_t sabreRelFT = ev.sabreFrontTime - ev.anodeBackTime;
      Double_t sabreRelBT = ev.sabreBackTime - ev.anodeBackTime;
      Double_t anodeRelFT = ev.anodeFrontTime - ev.anodeBackTime;
      Double_t anodeRelBT = ev.anodeBackTime - ev.anodeBackTime;
      MyFill("sabreRelFrontTime_hcut",1000,-3000,3500, sabreRelFT);
      MyFill("sabreRelBackTime_hcut",1000,-3000,3500, sabreRelBT);
      MyFill("anodeRelBackTime_hcut",1000,-3000,3500, anodeRelBT);
      MyFill("anodeRelFrontTime_hcut",1000,-3000,3500, anodeRelFT);
    } else {
      MyFill("noscinttime_counter_hcut",2,0,1,1);
    }
    
    if(ev.sabreFrontE != -1) {
      MyFill("sabreFrontE_hcut",4096,0,4096,ev.sabreFrontE);
    }
    if(ev.sabreBackE != -1) {
      MyFill("sabreBackE_hcut",4096,0,4096,ev.sabreBackE);
    }
  }
}

/*Single file run concept... Currently UNUSED*/
void SFPCleaner::Run(string input, string output) {
  TFile *infile = new TFile(input.c_str(), "READ");
  TTreeReader myReader("SPSTree", infile);
  TTreeReaderValue<ProcessedEvent> eve(myReader, "event");
  TFile *outfile = new TFile(output.c_str(), "RECREATE");
 
  Float_t blentries = myReader.GetEntries(false); 
  cout<<"Total number of events: "<<(Int_t)blentries<<endl;
  cout<<setprecision(5);
  float place;
  float i=0;
  while(myReader.Next()) {
    i++;
    place = ((float)i)/blentries*100; 
    if(fmod(place, 10.0) == 0.0) {
      cout<<"\rPercent of file processed: "<<place<<"%"<<flush;
    }
    MakeUncutHistograms(*event_address);
    MakeCutHistograms(*event_address);
  }
  cout<<endl;
  infile->Close();
  outfile->cd();
  rootObj->Write();
  delete rootObj;
  outfile->Close();
  delete outfile;
  delete infile;
}

/*Runs a list of files given from a RunMusher/Collector class*/
void SFPCleaner::Run(vector<TString> files, string output) {
  Chain(files);
  chain->SetBranchAddress("event", &event_address);
  TFile *outfile = new TFile(output.c_str(), "RECREATE");
 
  Float_t blentries = chain->GetEntries();
  cout<<"Total number of events: "<<(Int_t)blentries<<endl;
  cout<<setprecision(5);
  float place;
  for(int i=0; i<chain->GetEntries(); i++) {
    chain->GetEntry(i);
    place = ((float)i)/blentries*100; 
    if(fmod(place, 10.0) == 0.0) {/*Non-continuous progress update*/
      cout<<"\rPercent of file processed: "<<place<<"%"<<flush;
    }
    MakeUncutHistograms(*event_address);
    MakeCutHistograms(*event_address);
  }
  cout<<endl;
  outfile->cd();
  rootObj->Write();
  delete rootObj;
  outfile->Close();
  delete outfile;
}

/*Link all files*/
void SFPCleaner::Chain(vector<TString> files) {
  for(unsigned int i=0; i<files.size(); i++) {
    chain->Add(files[i]); 
  }
}
