#ifndef EVBMAINFRAME_H
#define EVBMAINFRAME_H

#include <TGClient.h>
#include <TGWindow.h>
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <TGMenu.h>
#include <TGTextViewStream.h>
#include <TGProgressBar.h>
#include <TTimer.h>
#include <TGComboBox.h>
#include "GWMEventBuilder.h"


class EVBMainFrame : public TGMainFrame {
public:
	EVBMainFrame(const TGWindow* p, UInt_t w, UInt_t h);
	virtual ~EVBMainFrame();
	void CloseWindow();
	void HandleMenuSelection(int id);
	void DoOpenROOTdir();
	void DoOpenBINdir();
	void DoOpenCMapfile();
	void DoOpenSMapfile();
	void DoOpenScalerfile();
	void DoOpenCutfile();
	void DoRun();
	void HandleTypeSelection(int box, int entry);
	bool SetParameters();
	void DisplayROOTdir(const char* dir);
	void DisplayBINdir(const char* dir);
	void DisplayCMap(const char* file);
	void DisplaySMap(const char* file);
	void DisplayScaler(const char* file);
	void DisplayCut(const char* file);
	void SaveConfig(const char* file);
	void LoadConfig(const char* file);
	void UpdateROOTdir();
	void UpdateBINdir();
	void UpdateCMap();
	void UpdateSMap();
	void UpdateScaler();
	void UpdateCut();
	void RunArchive(const char* dir, int number);
	void RunPlot(const char* file);
	void RunMerge(const char* dir, const char* file);
	void DisableAllInput();
	void EnableAllInput();


	enum WidgetId {
		ROOTDIR,
		BINDIR,
		CMAP,
		SMAP,
		SCALER,
		CUT,
		PLOTF,
		BFIELD,
		BKE,
		THETA,
		ZT,
		AT,
		ZP,
		AP,
		ZE,
		AE,
		SLOWWIND,
		FASTWIND_IC,
		FASTWIND_SABRE,
		TYPEBOX,
		RMIN,
		RMAX,
		M_LOAD_CONFIG,
		M_SAVE_CONFIG
	};

	ClassDef(EVBMainFrame, 0);

private:
	TGTextButton *fRunButton, *fOpenROOTButton, *fOpenBINButton, *fOpenCMapButton, *fOpenSMapButton, *fOpenScalerButton, *fOpenCutButton;
	TGTextEntry *fROOTField, *fBINField;
	TGTextEntry *fCMapField, * fSMapField;
	TGTextEntry *fScalerField, *fCutField;
	TGComboBox *fTypeBox;

	TGNumberEntryField *fZTField, *fATField, *fZPField, *fAPField, *fZEField, *fAEField;
	TGNumberEntryField *fBField, *fBKEField, *fThetaField;
	TGNumberEntryField *fSlowWindowField, *fFastICField, *fFastSABREField;
	TGNumberEntryField *fRMinField, *fRMaxField;

	TGHProgressBar* fProgressBar;

	TGPopupMenu *fFileMenu;

	GWMEventBuilder fBuilder;

	int counter;
	UInt_t MAIN_W, MAIN_H;


};
#endif
