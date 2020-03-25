/*TimeSort.cpp
 *Class designed to first time-order raw data, and then based on a given coincidence window
 *sort the raw data into coincidence structures. Utilizes dictionary elements DPPChannel and
 *CoincEvent. Based on work by S. Balak, K. Macon, and E. Good from LSU. 
 *
 *Gordon M. Oct. 2019
 *
 *Refurbished and updated Jan 2020 GWM
 */
#include "TimeSort.h"

/*UNUSED*/
bool MySort(DPPChannel i, DPPChannel j) {
  return (i.Timestamp<j.Timestamp);
}

/*Sort the Sabre Data in order of descending energy*/
bool SabreSort(SabreHit i, SabreHit j) {
  return (i.Long>j.Long);
}

/*Constructor takes input of coincidence window size, and fills sabre channel map*/
TimeSort::TimeSort(float windowSize) {
  coincWindow = windowSize;
  
  if(!FillSabreMap(smap)) illegalMap = 1;
  else illegalMap = 0; 
}

TimeSort::~TimeSort() {
}

/*Reset output structure to blank*/
void TimeSort::Reset() {
  event = blank;
}

/*Function called when a start of a coincidence event is detected*/
void TimeSort::StartEvent() {
  if(hitList.size() != 0) {
    cerr<<"Attempting to initalize hitList when not cleared!! Check processing order."<<endl;
  }
  startTime = hit.Timestamp;
  hitList.push_back(hit);
}

/*Function called when an event outside the coincidence window is detected
 *Process all of the hits in the list, and write them to the sorted tree
 */
bool TimeSort::ProcessEvent() {
  Reset();
  /*Flags to further dump data... Only good event if full focal plane*/
  int dfrFlag=0, dflFlag=0, dbrFlag=0, dblFlag=0;
  int sLFlag=0, sRFlag=0, afFlag=0, abFlag=0;
  int saFFlag=0, saBFlag = 0;
  event.sabreFrontMult = 0, event.sabreBackMult = 0;
  for(unsigned int i=0; i<hitList.size(); i++) {
    shit = sblank; //Have to reset sabre hit every time
    DPPChannel curHit = hitList[i];
    int gchan = curHit.Channel + curHit.Board*16; //global channel
    switch(gchan) {//switch over focal plane
      case delayFL_id:
        event.delayTimeFL = ((Double_t)curHit.Timestamp)/1.0e3;
        event.delayShortFL = curHit.EnergyShort;
        event.delayLongFL = curHit.Energy;
        dflFlag += 1;
        break;
      case delayFR_id:
        event.delayTimeFR = ((Double_t)curHit.Timestamp)/1.0e3;
        event.delayShortFR = curHit.EnergyShort;
        event.delayLongFR = curHit.Energy;
        dfrFlag += 1;
        break;
      case delayBL_id:
        event.delayTimeBL = ((Double_t)curHit.Timestamp)/1.0e3;
        event.delayShortBL = curHit.EnergyShort;
        event.delayLongBL = curHit.Energy;
        dblFlag += 1;
        break;
      case delayBR_id:
        event.delayTimeBR = ((Double_t)curHit.Timestamp)/1.0e3;
        event.delayShortBR = curHit.EnergyShort;
        event.delayLongBR = curHit.Energy;
        dbrFlag += 1;
        break;
      case anodeF_id:
        event.anodeTimeF = ((Double_t)curHit.Timestamp)/1.0e3;
        event.anodeShortF = curHit.EnergyShort;
        event.anodeLongF = curHit.Energy;
        afFlag += 1;
        break;
      case anodeB_id:
        event.anodeTimeB = ((Double_t)curHit.Timestamp)/1.0e3;
        event.anodeShortB = curHit.EnergyShort;
        event.anodeLongB = curHit.Energy;
        abFlag += 1;
        break;
      case scintL_id:
        event.scintTimeL = ((Double_t)curHit.Timestamp)/1.0e3;
        event.scintShortL = curHit.EnergyShort;
        event.scintLongL = curHit.Energy;
        sLFlag += 1;
        break;
      case scintR_id:
        event.scintTimeR = ((Double_t)curHit.Timestamp)/1.0e3;
        event.scintShortR = curHit.EnergyShort;
        event.scintLongR = curHit.Energy;
        sRFlag += 1;
        break;
      case cath_id:
        event.cathodeTime = ((Double_t)curHit.Timestamp)/1.0e3;
        event.cathodeShort = curHit.EnergyShort;
        event.cathodeLong = curHit.Energy;
        break;
    }
    try {/*see if this is a sabre channel*/
      sabrechan sc = smap.at(gchan); //throws out_of_range if not a valid member
      if(sc.side_pos.first == "FRONT" && curHit.Energy<16384 && (curHit.Energy<sc.ECutLo || curHit.Energy>sc.ECutHi) 
         && curHit.Energy>20.0) {
        shit.Long = curHit.Energy;//*gains.GetScaler(gchan);
        shit.Time = (Double_t) curHit.Timestamp/1.0e3;
        shit.Ch = curHit.Channel+curHit.Board*16;
        event.sabreFrontData.push_back(shit);
        event.sabreFrontMult += 1;
        saFFlag++;
      } else if (sc.side_pos.first == "BACK" && curHit.Energy<16384 && 
                 (curHit.Energy<sc.ECutLo || curHit.Energy>sc.ECutHi)){
        shit.Long = curHit.Energy;//*gains.GetScaler(gchan);
        shit.Time = ((Double_t) curHit.Timestamp)/1.0e3;
        shit.Ch = curHit.Channel+curHit.Board*16;
        event.sabreBackData.push_back(shit);
        event.sabreBackMult += 1;
        saBFlag++;
      }
    } catch (out_of_range& orr) {} //dont do anything if not sabre
  }
  //Organize the SABRE data in descending energy order
  sort(event.sabreFrontData.begin(), event.sabreFrontData.end(), SabreSort);
  sort(event.sabreBackData.begin(), event.sabreBackData.end(), SabreSort);

  //Event Building Stats
  if((dflFlag && dfrFlag && dblFlag && dbrFlag && afFlag && abFlag && sLFlag && sRFlag) && (saBFlag || saFFlag)) {
    completeFP_SABRE++;
    if(dflFlag>1 || dfrFlag>1 || dblFlag>1 || dbrFlag>1 || afFlag>1 || abFlag>1 || sLFlag>1 || sRFlag>1) {
      FPextras++;
    }
  } else if (dflFlag && dfrFlag && dblFlag && dbrFlag && afFlag && abFlag && sLFlag && sRFlag) {
    completeFP++;
    if(dflFlag>1 || dfrFlag>1 || dblFlag>1 || dbrFlag>1 || afFlag>1 || abFlag>1 || sLFlag>1 || sRFlag>1) {
      FPextras++;
    }
  } else if (saBFlag || saFFlag) {
    SABREorphans++;
    if(!(sLFlag || sRFlag)) {
      SABREorphans_noscint++;
    }
  } else {
    FPorphans++;
    if((dflFlag || dfrFlag || dblFlag || dbrFlag || afFlag || abFlag) && (sLFlag || sRFlag)) {
      FPorphans_partial++;
    } else if(!(sLFlag || sRFlag)) {
      FPorphans_noscint++;
    } else {
      FPorphans_nogas++;
    }
  }
  //int sum = dflFlag+dfrFlag+dblFlag+dbrFlag+sLFlag+sRFlag+afFlag+abFlag; 
  int sum = 7; //pass everything for testing
  if(sum == 7) {//full focal plane requirement
    totalEvents++;
    return true;
  } else {
    return false;
  }
}

/*Loop over all input events, function called by main*/
void TimeSort::Run(const char *infile_name, const char *outfile_name) {
  if(illegalMap) {
    cerr<<"Unable to process with illegal map!"<<endl;
    return;
  }
  TFile* compFile = new TFile(infile_name, "READ");
  TTree* compassTree = (TTree*) compFile->Get("Data");
  TFile* outputFile = new TFile(outfile_name, "RECREATE");
  TTree* SortTree = new TTree("SortTree","SortTree");

  compassTree->SetBranchAddress("Energy",&hit.Energy);
  compassTree->SetBranchAddress("EnergyShort", &hit.EnergyShort);
  compassTree->SetBranchAddress("Timestamp", &hit.Timestamp);
  compassTree->SetBranchAddress("Channel", &hit.Channel);
  compassTree->SetBranchAddress("Board", &hit.Board);
  compassTree->SetBranchAddress("Flags", &hit.Flags);
  
  SortTree->Branch("event", &event);
  startTime = 0; previousHitTime = 0; //initialize
  
  Int_t blentries = compassTree->GetEntries();
  vector<DPPChannel> sort_list;
  cout<<setprecision(5)<<fixed;
 
  /*SetMaxVirtualSize supposedly increases the tree buffer size to the specified size;
   *behavior is not trivial, and is not reasonable for large files
   */
  compassTree->SetMaxVirtualSize(4000000000);
  compassTree->BuildIndex("0", "Timestamp");//Sort tree based on Timestamp
  /*Next two steps get the index of the sorted tree*/
  TTreeIndex *tindex = (TTreeIndex*) compassTree->GetTreeIndex();
  Long64_t *index = tindex->GetIndex();
  Float_t place;
  completeFP = 0; completeFP_SABRE = 0; SABREorphans = 0; FPorphans = 0; FPextras = 0; totalEvents = 0;
  FPorphans_partial = 0; FPorphans_noscint=0; FPorphans_nogas = 0; SABREorphans_noscint = 0;
  for(Int_t i=0; i<compassTree->GetEntries(); i++) {
    compassTree->GetEntry(index[i]);
    place = ((Float_t)i)/blentries*100;
    if(fmod(place, 10.0) == 0) { //Non-continuous progress update
      cout<<"\rPercent of file processed: "<<place<<"%"<<flush;
    }
    if(hitList.empty()) {//first hit in file starts first event
      StartEvent();
    } else if (hit.Timestamp < previousHitTime) {//out of order check
      cerr<<"This timestamp: "<<hit.Timestamp<<" is out of order with : "<<previousHitTime
          <<". Skipping hit!"<<endl;
    } else if ((float)(hit.Timestamp - startTime)< coincWindow) {
      hitList.push_back(hit);
    } else if (hitList.size()>0) { 
      //if(hitList.size()>=6) { //Need at least 7 things for good event: 4delays, 2anodes, scint
        if(ProcessEvent()) SortTree->Fill(); //if good, fill
      //}
      /*Start next event with the hit in hand*/
      hitList.resize(0);
      StartEvent();
    }
    previousHitTime = hit.Timestamp;
  }
  cout<<endl;
  cout<<"Event Building Statistics"<<endl;
  cout<<"Total Coincidence Events: "<<totalEvents<<endl;
  cout<<"SABRE-Focal Plane Coincidences: "<<completeFP_SABRE<<endl;
  cout<<"Complete Focal Plane Events Sans SABRE: "<<completeFP<<endl;
  cout<<"Orphaned SABRE Events: "<<SABREorphans<<endl;
  cout<<"Orphaned SABRE Events Sans Scint: "<<SABREorphans_noscint<<endl;
  cout<<"Orphaned Focal Plane Data: "<<FPorphans<<endl;
  cout<<"Orphaned FP with Partial: "<<FPorphans_partial<<endl;
  cout<<"Orphaned FP without scint: "<<FPorphans_noscint<<endl;
  cout<<"Orphaned FP without gas: "<<FPorphans_nogas<<endl;
  outputFile->cd();
  SortTree->Write(SortTree->GetName(), TObject::kOverwrite);
  compFile->Close();
  outputFile->Close();
  delete compFile;
  delete outputFile;
}
