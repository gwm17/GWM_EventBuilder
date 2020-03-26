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
  edefile = NULL;
  dexfile = NULL;
  exfile = NULL;
  xxfile = NULL;
  EdECut = NULL, ExCut = NULL, dExCut = NULL, x1x2Cut = NULL;
  chain = new TChain("SPSTree");
}

/*UNUSED*/
SFPCleaner::SFPCleaner(bool tf) {
  rootObj = new THashTable();
  rootObj->SetOwner(false);
  event_address = new ProcessedEvent();
  treeFlag = tf;
  edefile = NULL;
  dexfile = NULL;
  exfile = NULL;
  xxfile = NULL;
  EdECut = NULL, ExCut = NULL, dExCut = NULL, x1x2Cut = NULL;
}

SFPCleaner::~SFPCleaner() {
  delete event_address;
  if(edefile != NULL && edefile->IsOpen()) edefile->Close();
  if(dexfile != NULL && dexfile->IsOpen()) dexfile->Close();
  if(exfile != NULL && exfile->IsOpen()) exfile->Close();
  if(xxfile != NULL && xxfile->IsOpen()) xxfile->Close();
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
int SFPCleaner::SetCuts(string edename, string dexname, string exname, string xxname) {
  edefile = new TFile(edename.c_str(), "READ");
  if(edefile->IsOpen()) {
    EdECut = (TCutG*) edefile->Get("CUTG");
    EdECut->SetName("EdECut");
    rootObj->Add(EdECut);
  }
  dexfile = new TFile(dexname.c_str(), "READ");
  if(dexfile->IsOpen()) {
    dExCut = (TCutG*) dexfile->Get("CUTG");
    dExCut->SetName("dExCut");
    rootObj->Add(dExCut);
  }
  exfile = new TFile(exname.c_str(), "READ");
  if(exfile->IsOpen()) {
    ExCut = (TCutG*) exfile->Get("CUTG");
    ExCut->SetName("ExCut");
    rootObj->Add(ExCut);
  }
  xxfile = new TFile(xxname.c_str(), "READ");
  if(xxfile->IsOpen()) {
    x1x2Cut = (TCutG*) xxfile->Get("CUTG");
    x1x2Cut->SetName("x1x2Cut");
    rootObj->Add(x1x2Cut);
  }
  if(EdECut != NULL && dExCut != NULL && x1x2Cut != NULL && ExCut != NULL) {
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
    MyFill("x1_x2_NoCuts",600,-300,300,ev.x1,600,-300,300,ev.x2);

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
    MyFill("sabreFrontMult_NoCuts",10,0,10,ev.sabreFrontMult);
    MyFill("sabreBackMult_NoCuts",10,0,10,ev.sabreBackMult);

    /****Timing relative to back anode****/
    if(ev.anodeBackTime != -1 && ev.scintLeftTime != -1) {
      Double_t sabreRelFT = ev.sabreFrontTime - ev.anodeBackTime;
      Double_t sabreRelBT = ev.sabreBackTime - ev.anodeBackTime;
      Double_t anodeRelFT = ev.anodeFrontTime - ev.anodeBackTime;
      Double_t delayRelFT = ev.delayFrontMaxTime - ev.anodeBackTime;
      Double_t delayRelBT = ev.delayBackMaxTime - ev.anodeBackTime;
      MyFill("sabreRelFrontTime_NoCuts",1000,-3000,3500, sabreRelFT);
      MyFill("sabreRelBackTime_NoCuts",1000,-3000,3500, sabreRelBT);
      MyFill("anodeRelFrontTime_NoCuts",1000,-3000,3500, anodeRelFT);
      MyFill("delayRelFrontTime_NoCuts",1000,-3000,-3500,delayRelFT);
      MyFill("delayRelBackTime_NoCuts",1000,-3000,-3500,delayRelBT);
      if(ev.sabreFrontE != -1 && sabreRelFT<1.0 && sabreRelFT>-1.0) {
        MyFill("xavg_sabrefcoinc_NoCuts",600,-300,300, ev.xavg);
      }
      Double_t sabreRelFT_toScint = ev.sabreFrontTime - ev.scintLeftTime;
      Double_t sabreRelBT_toScint = ev.sabreBackTime - ev.scintLeftTime;
      Double_t anodeRelT = ev.anodeBackTime - ev.scintLeftTime;
      Double_t delayRelFT_toScint = ev.delayFrontMaxTime - ev.scintLeftTime;
      Double_t delayRelBT_toScint = ev.delayBackMaxTime - ev.scintLeftTime;
      MyFill("anodeRelTime_toScint",1000,-3000,3500,anodeRelT);
      MyFill("sabreRelFrontTime_toScint",1000,-3000,3500,sabreRelFT_toScint);
      MyFill("sabreRelBackTime_toScint",1000,-3000,3500,sabreRelBT_toScint);
      MyFill("delayRelBackTime_toScint",1000,-3000,3500,delayRelBT_toScint);
      MyFill("delayRelFrontTime_toScint",1000,-3000,3500,delayRelFT_toScint);
      MyFill("sabreRelFTScint_sabreRelFTAnode",500,-3000,3500,sabreRelFT_toScint,500,-3000,3500,sabreRelFT);
      MyFill("sabreRelFTScint_sabreFrontChannel",500,-3000,3500,sabreRelFT_toScint,144,0,144,ev.sabreChannelFront);
      MyFill("sabreRelFTAnode_sabreFrontChannel",500,-3000,3500,sabreRelFT,144,0,144,ev.sabreChannelFront);
      MyFill("sabreRelBTScint_sabreBackChannel",500,-3000,3500,sabreRelBT_toScint,144,0,144,ev.sabreChannelBack);
      MyFill("sabreRelFT_sabreRelBT",500,-3000,3500,sabreRelFT,500,-3000,3500,sabreRelBT);
      MyFill("sabreRelFT_sabreRelBT_scint",500,-3000,3500,sabreRelFT_toScint,500,-3000,3500,sabreRelBT_toScint);
      MyFill("sabreRelFTScint_anodeRelT",500,-3000,3500,sabreRelFT_toScint,500,-3000,3500,anodeRelT);
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
  if(EdECut->IsInside(ev.scintLeft, ev.delayBackRightE) && x1x2Cut->IsInside(ev.x1, ev.x2)) {
    MyFill("x1_bothplanes_edecut",600,-300,300,ev.x2);
    MyFill("x2_bothplanes_edecut",600,-300,300,ev.x2);
    MyFill("xavg_bothplanes_edecut",600,-300,300,ev.xavg);
    MyFill("x1_x2_edecut",600,-300,300,ev.x1, 600,-300,300,ev.x2);
    
    MyFill("x1_delayBackRightE_edecut",600,-300,300,ev.x1,512,0,4096,ev.delayBackRightE);
    MyFill("x2_delayBackRightE_edecut",600,-300,300,ev.x2,512,0,4096,ev.delayBackRightE);
    MyFill("xavg_delayBackRightE_edecut",600,-300,300,ev.xavg,512,0,4096,ev.delayBackRightE);

    Double_t delayBackAvgE = (ev.delayBackRightE+ev.delayBackLeftE)/2.0;
    MyFill("x1_delayBackAvgE_edecut",600,-300,300,ev.x1,512,0,4096,delayBackAvgE);
    MyFill("x2_delayBackAvgE_edecut",600,-300,300,ev.x2,512,0,4096,delayBackAvgE);
    MyFill("xavg_delayBackAvgE_edecut",600,-300,300,ev.xavg,512,0,4096,delayBackAvgE);
    Double_t delayFrontAvgE = (ev.delayFrontRightE+ev.delayFrontLeftE)/2.0;
    MyFill("x1_delayFrontAvgE_edecut",600,-300,300,ev.x1,512,0,4096,delayFrontAvgE);
    MyFill("x2_delayFrontAvgE_edecut",600,-300,300,ev.x2,512,0,4096,delayFrontAvgE);
    MyFill("xavg_delayFrontAvgE_edecut",600,-300,300,ev.xavg,512,0,4096,delayFrontAvgE);

    MyFill("scintLeft_anodeBack_edecut",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeBack);
    MyFill("scintLeft_anodeFront_edecut",512,0,4096,ev.scintLeft,512,0,4096,ev.anodeFront);
    MyFill("sabreFrontMult_edecut",10,0,10,ev.sabreFrontMult);
    MyFill("sabreBackMult_edecut",10,0,10,ev.sabreBackMult);

    /****Timing relative to back anode****/
    if(ev.anodeBackTime != -1) {
      Double_t sabreRelFT = ev.sabreFrontTime - ev.anodeBackTime;
      Double_t sabreRelBT = ev.sabreBackTime - ev.anodeBackTime;
      Double_t anodeRelFT = ev.anodeFrontTime - ev.anodeBackTime;
      Double_t anodeRelBT = ev.anodeBackTime - ev.anodeBackTime;
      MyFill("sabreRelFrontTime_edecut",1000,-3000,3500, sabreRelFT);
      MyFill("sabreRelBackTime_edecut",1000,-3000,3500, sabreRelBT);
      MyFill("anodeRelBackTime_edecut",1000,-3000,3500, anodeRelBT);
      MyFill("anodeRelFrontTime_edecut",1000,-3000,3500, anodeRelFT);
      Double_t anodeRelFT_toScint = ev.anodeFrontTime-ev.scintLeftTime;
      MyFill("anodeRelTime_toScint_edecut",1000,-3000,3500,anodeRelFT_toScint);
      if(ev.sabreFrontE != -1 && sabreRelFT<1.0 && sabreRelFT>-1.0) {
        MyFill("xavg_edecut_sabrefcoinc_strict",600,-300,300, ev.xavg);
      }
    } else {
      MyFill("noscinttime_counter_edecut",2,0,1,1);
    }
    
    if(ev.sabreFrontE != -1) {
      MyFill("sabreFrontE_edecut",4096,0,4096,ev.sabreFrontE);
      MyFill("xavg_edecut_sabrefcoinc",600,-300,300,ev.xavg);
      MyFill("xavg_sabreFrontE_edecut",600,-300,300,ev.xavg,512,0,4096,ev.sabreFrontE);
    }
    if(ev.sabreBackE != -1) {
      MyFill("sabreBackE_edecut",4096,0,4096,ev.sabreBackE);
      MyFill("xavg_sabreBackE_edecut",600,-300,300,ev.xavg,512,0,4096,ev.sabreBackE);
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
