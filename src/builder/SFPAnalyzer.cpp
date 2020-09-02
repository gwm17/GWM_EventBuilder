/*SFPAnalyzer.cpp
 *Class designed to analyze coincidence events. Currently only implemented for focal plane
 *data. Additional changes for SABRE would include this file and the sructure ProcessedEvent
 *in DataStructs.h. Based on code written by S. Balak, K. Macon, and E. Good.
 *
 *Gordon M. Oct. 2019
 *
 *Refurbished and updated Jan 2020 by GWM. Now uses both focal plane and SABRE data
 */
#include "EventBuilder.h"
#include "SFPAnalyzer.h"

using namespace std;

/*Constructor takes in kinematic parameters for generating focal plane weights*/
SFPAnalyzer::SFPAnalyzer(int zt, int at, int zp, int ap, int ze, int ae, double ep,
                         double angle, double b) {
  zfp = Delta_Z(zt, at, zp, ap, ze, ae, ep, angle, b);
  event_address = new CoincEvent();
  GetWeights();
}

SFPAnalyzer::~SFPAnalyzer() {
  delete event_address;
}

void SFPAnalyzer::Reset() {
  pevent = blank; //set output back to blank
}

/*Use functions from FP_kinematics to calculate weights for xavg
 *While this seems kind of funny, it is mathematically equivalent to making a line
 *from the two focal plane points and finding the intersection with 
 *the kinematic focal plane
 */
void SFPAnalyzer::GetWeights() {
  w1 = (Wire_Dist()/2.0-zfp)/Wire_Dist();
  w2 = 1.0-w1;
  cout<<"w1: "<<w1<<" w2: "<<w2<<endl;
}

/*2D histogram fill wrapper for use with THashTable (faster)*/
void SFPAnalyzer::MyFill(string name, int binsx, double minx, double maxx, double valuex,
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

/*1D histogram fill wrapper for use with THashTable (faster)*/
void SFPAnalyzer::MyFill(string name, int binsx, double minx, double maxx, double valuex) {
  TH1F *histo = (TH1F*) rootObj->FindObject(name.c_str());
  if(histo != NULL) {
    histo->Fill(valuex);
  } else {
    TH1F *h = new TH1F(name.c_str(), name.c_str(), binsx, minx, maxx);
    h->Fill(valuex);
    rootObj->Add(h);
  }
}

/*Bulk of the work done here*/
void SFPAnalyzer::Run(const char *input, const char *output) {
  TFile* inputFile = new TFile(input, "READ");
  TTree* inputTree = (TTree*) inputFile->Get("SortTree");
  inputTree->SetBranchAddress("event", &event_address);

  TFile* outputFile = new TFile(output, "RECREATE");
  TTree* outputTree = new TTree("SPSTree", "SPSTree");
  rootObj = new THashTable();
  rootObj->SetOwner(false);//Stops THashTable from owning its members; prevents double delete

  outputTree->Branch("event", &pevent);
  Float_t place;
  Float_t blentries = inputTree->GetEntries();
  cout<<setprecision(2);
  for(long double i=0; i<inputTree->GetEntries(); i++) {
    inputTree->GetEntry(i);
    place = ((long double)i)/blentries*100;
    /*Non-continuous progress update*/
    if(fmod(place, 10.0) == 0) cout<<"\rPercent of file processed: "<<ceil(place)<<"%"<<flush;
    Reset();
    /*Focal plane parts*/
    if(!event_address->focalPlane.anodeF.empty()) {
      pevent.anodeFront = event_address->focalPlane.anodeF[0].Long;
      pevent.anodeFrontTime = event_address->focalPlane.anodeF[0].Time;
    }
    if(!event_address->focalPlane.anodeB.empty()) {
      pevent.anodeBack = event_address->focalPlane.anodeB[0].Long;
      pevent.anodeBackTime = event_address->focalPlane.anodeB[0].Time;
    }
    if(!event_address->focalPlane.scintL.empty()) {
      pevent.scintLeft = event_address->focalPlane.scintL[0].Long;
      pevent.scintLeftTime = event_address->focalPlane.scintL[0].Time;
    }
    if(!event_address->focalPlane.scintR.empty()) {
      pevent.scintRight = event_address->focalPlane.scintR[0].Long;
      pevent.scintRightTime = event_address->focalPlane.scintR[0].Time;
    }
    if(!event_address->focalPlane.cathode.empty()) {
      pevent.cathode = event_address->focalPlane.cathode[0].Long;
      pevent.cathodeTime = event_address->focalPlane.cathode[0].Time;
    }

    /*Delay lines and all that*/
    if(!event_address->focalPlane.delayFR.empty()) {
      pevent.delayFrontRightE = event_address->focalPlane.delayFR[0].Long;
    }
    if(!event_address->focalPlane.delayFL.empty()) {
      pevent.delayFrontLeftE = event_address->focalPlane.delayFL[0].Long;
    }
    if(!event_address->focalPlane.delayBR.empty()) {
      pevent.delayBackRightE = event_address->focalPlane.delayBR[0].Long;
    }
    if(!event_address->focalPlane.delayBL.empty()) {
      pevent.delayBackLeftE = event_address->focalPlane.delayBL[0].Long;
    }
    if(!event_address->focalPlane.delayFL.empty() && !event_address->focalPlane.delayFR.empty()) { 
      pevent.fp1_tdiff = (event_address->focalPlane.delayFL[0].Time-event_address->focalPlane.delayFR[0].Time)*0.5;
      pevent.fp1_tsum = (event_address->focalPlane.delayFL[0].Time+event_address->focalPlane.delayFR[0].Time);
      pevent.fp1_tcheck = (pevent.fp1_tsum)/2.0-pevent.anodeFrontTime;
      pevent.delayFrontMaxTime = max(event_address->focalPlane.delayFL[0].Time, event_address->focalPlane.delayFR[0].Time);
      pevent.x1 = pevent.fp1_tdiff*1.0/1.98; //position from time, based on total delay
      //pevent.x1 = 0.52*pevent.fp1_tdiff - 0.128; //position from time, based on delay chips
      MyFill("x1",1200,-300,300,pevent.x1);
      MyFill("x1 vs anodeBack",600,-300,300,pevent.x1,512,0,4096,pevent.anodeBack);
    }
    if(!event_address->focalPlane.delayBL.empty() && !event_address->focalPlane.delayBR.empty()) {
      pevent.fp2_tdiff = (event_address->focalPlane.delayBL[0].Time-event_address->focalPlane.delayBR[0].Time)*0.5;
      pevent.fp2_tsum = (event_address->focalPlane.delayBL[0].Time+event_address->focalPlane.delayBR[0].Time);
      pevent.fp2_tcheck = (pevent.fp2_tsum)/2.0-pevent.anodeBackTime;
      pevent.delayBackMaxTime = max(event_address->focalPlane.delayBL[0].Time, event_address->focalPlane.delayBR[0].Time);
      pevent.x2 = pevent.fp2_tdiff*1.0/2.10; //position from time, based on total delay
      //pevent.x2 = 0.48*pevent.fp2_tdiff - 2.365; //position from time, based on delay chips
      MyFill("x2",1200,-300,300,pevent.x2);
      MyFill("x2 vs anodeBack",600,-300,300,pevent.x2,512,0,4096,pevent.anodeBack);
    }
    /*SABRE data*/
    for(int j=0; j<5; j++) {
      if(!event_address->sabreArray[j].rings.empty()) {
        pevent.sabreRingE[j] = event_address->sabreArray[j].rings[0].Long;
        pevent.sabreRingChannel[j] = event_address->sabreArray[j].rings[0].Ch;
        pevent.sabreRingTime[j] = event_address->sabreArray[j].rings[0].Time;
      }
      if(!event_address->sabreArray[j].wedges.empty()) {
        pevent.sabreWedgeE[j] = event_address->sabreArray[j].wedges[0].Long;
        pevent.sabreWedgeChannel[j] = event_address->sabreArray[j].wedges[0].Ch;
        pevent.sabreWedgeTime[j] = event_address->sabreArray[j].wedges[0].Time;
      }
      /*Aaaand passes on all of the rest. 4/24/20 GWM*/
      pevent.sabreArray[j] = event_address->sabreArray[j];
    }

    /*Make some histograms and xavg*/
    MyFill("anodeBack vs scintLeft",512,0,4096,pevent.scintLeft,512,0,4096,pevent.anodeBack);
    if(pevent.x1 != -1e6 && pevent.x2 != -1e6) {
      pevent.xavg = pevent.x1*w1+pevent.x2*w2;
      MyFill("xavg",1200,-300,300,pevent.xavg);
      if((pevent.x2-pevent.x1) > 0) {
        pevent.theta = atan((pevent.x2-pevent.x1)/36.0);
      } else if((pevent.x2-pevent.x1) < 0) {
        pevent.theta = TMath::Pi() + atan((pevent.x2-pevent.x1)/36.0);
      } else {
        pevent.theta = TMath::Pi()/2.0;
      }
      MyFill("xavg vs theta",600,-300,300,pevent.xavg,314,0,3.14,pevent.theta);
      MyFill("x1 vs x2",600,-300,300,pevent.x1,600,-300,300,pevent.x2);
    }
    if(pevent.anodeFrontTime != -1 && pevent.scintRightTime != -1) {
      pevent.fp1_y = pevent.anodeFrontTime-pevent.scintRightTime;
    }
    if(pevent.anodeBackTime != -1 && pevent.scintRightTime != -1) {
      pevent.fp2_y = pevent.anodeBackTime-pevent.scintRightTime;
    }
    outputTree->Fill();
  }
  cout<<endl;
  outputFile->cd();
  rootObj->Write();
  delete rootObj;
  outputTree->Write(outputTree->GetName(), TObject::kOverwrite);
  outputFile->Close();
  inputFile->Close();
  delete outputFile;
  delete inputFile;
}
