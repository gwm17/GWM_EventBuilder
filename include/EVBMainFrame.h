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
	void DoRun();
	void HandleTypeSelection(int box, int entry);
	bool SetParameters();
	void DisplayROOTdir(const char* dir);
	void DisplayBINdir(const char* dir);
	void DisplayCMap(const char* file);
	void DisplaySMap(const char* file);
	void UpdateROOTdir();
	void UpdateBINdir();
	void UpdateCMap();
	void UpdateSMap();
	void RunArchive(const char* dir, int number);
	void RunPlot(const char* file, const char* cutlist);
	void RunMerge(const char* dir, const char* file);
	void DisableAllInput();
	void EnableAllInput();
	void UpdateConsole();


	enum WidgetId {
		ROOTDIR,
		BINDIR,
		CMAP,
		SMAP,
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
		RMAX
	};

	ClassDef(EVBMainFrame, 0);

private:
	TGTextButton *fRunButton, *fOpenROOTButton, *fOpenBINButton, *fOpenCMapButton, *fOpenSMapButton;
	TGTextEntry *fROOTField, *fBINField;
	TGTextEntry *fCMapField, * fSMapField;
	TGComboBox *fTypeBox;

	TGNumberEntryField *fZTField, *fATField, *fZPField, *fAPField, *fZEField, *fAEField;
	TGNumberEntryField *fBField, *fBKEField, *fThetaField;
	TGNumberEntryField *fSlowWindowField, *fFastICField, *fFastSABREField;
	TGNumberEntryField *fRMinField, *fRMaxField;

	TGTextViewostream *fConsole;

	//TGPopup *fFileMenu;

	//TTimer *fUpdateTimer;

	GWMEventBuilder fBuilder;

	int counter;
	UInt_t MAIN_W, MAIN_H;


};
#endif