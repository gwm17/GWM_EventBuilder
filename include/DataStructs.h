/*DataStructs.h
 *Data structures for analysis. To be implemented as a dictionary for ROOT in LinkDef
 *Based on: FocalPlane_SABRE.h
 *Gordon M. Oct. 2019
 */
#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <TROOT.h>
#include <string>

using namespace std;

struct DPPChannel {
  ULong64_t Timestamp = -1;
  UShort_t Channel = -1 , Board = -1, Energy = -1, EnergyShort = -1;
  string Name = "";
  UInt_t Flags = -1;
};

struct CoincEvent {
  Double_t delayTimeFL=-1, delayTimeFR=-1, delayTimeBL=-1, delayTimeBR=-1;
  Double_t anodeTimeF=-1, anodeTimeB=-1, scintTimeL=-1, scintTimeR=-1, 
           cathodeTime=-1;
  Double_t delayLongFL=-1, delayLongFR=-1, delayLongBL=-1, delayLongBR=-1;
  Double_t anodeLongF=-1, anodeLongB=-1, scintLongL=-1, scintLongR=-1,
           cathodeLong=-1;
  Double_t delayShortFL=-1, delayShortFR=-1, delayShortBL=-1, delayShortBR=-1;
  Double_t anodeShortF=-1, anodeShortB=-1, scintShortL=-1, scintShortR=-1,
           cathodeShort=-1;
  Double_t sabreLongF=-1, sabreLongB=-1, sabreTimeF=-1, sabreTimeB=-1;
  Double_t sabreChF=-1, sabreChB=-1;
};

struct ProcessedEvent {
  Double_t fp1_tdiff = -1e6, fp2_tdiff = -1e6, fp1_tsum = -1, fp2_tsum = -1,
           fp1_tcheck = -1, fp2_tcheck = -1;
  Double_t fp1_y, fp2_y;
  Double_t anodeFront = -1, anodeBack = -1, scintRight = -1, scintLeft = -1;
  Double_t cathode = -1;
  Double_t xavg = -1e6, x1 = -1e6, x2 = -1e6;
  Double_t theta = -1e6;
  Double_t sabreFrontE = -1, sabreBackE = -1;
  Int_t sabreChannelFront = -1, sabreChannelBack = -1;
  Double_t delayFrontRightE = -1, delayFrontLeftE = -1;
  Double_t delayBackRightE = -1, delayBackLeftE = -1;
  Double_t sabreFrontTime = -1, sabreBackTime = -1;
  Double_t anodeFrontTime = -1, anodeBackTime = -1;
  Double_t scintRightTime = -1, scintLeftTime = -1;
};

#endif
