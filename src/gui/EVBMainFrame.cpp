#include "EventBuilder.h"
#include "EVBMainFrame.h"
#include "FileViewFrame.h"
#include <TGLabel.h>
#include <TGTextBuffer.h>
#include <TApplication.h>
#include <TSystem.h>

EVBMainFrame::EVBMainFrame(const TGWindow* p, UInt_t w, UInt_t h) : 
	TGMainFrame(p, w, h, kHorizontalFrame)
{
	SetCleanup(kDeepCleanup);
	MAIN_W = w; MAIN_H = h;

	//Organization hints
	TGLayoutHints *fchints = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,5,5,5,5);
	TGLayoutHints *fhints = new TGLayoutHints(kLHintsExpandX|kLHintsCenterY,5,5,5,5);
	TGLayoutHints *lchints = new TGLayoutHints(kLHintsTop|kLHintsLeft,5,5,5,5);
	TGLayoutHints *lhints = new TGLayoutHints(kLHintsCenterY|kLHintsLeft,5,5,5,5);
	TGLayoutHints *bhints = new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,5,5);

	//Make the containers and link up all signals/slots

	TGVerticalFrame *ConsoleFrame = new TGVerticalFrame(this, w*0.5, h);
	TGLabel *consLabel = new TGLabel(ConsoleFrame, "Console");
	fConsole = new TGTextViewostream(ConsoleFrame, w, h*0.3);
	ConsoleFrame->AddFrame(consLabel, lchints);
	ConsoleFrame->AddFrame(fConsole, fchints);

	fBuilder.SetStream(fConsole);

	TGVerticalFrame *InputFrame = new TGVerticalFrame(this, w*0.5, h);

	TGVerticalFrame *NameFrame = new TGVerticalFrame(InputFrame, w*0.5, h);

	TGHorizontalFrame *ROOTFrame = new TGHorizontalFrame(NameFrame, w*0.5, h*0.1);
	TGLabel* rootLabel = new TGLabel(ROOTFrame, "ROOT Directory:");
	fROOTField = new TGTextEntry(ROOTFrame, new TGTextBuffer(120), ROOTDIR);
	fROOTField->Resize(w*0.25, fROOTField->GetDefaultHeight());
	fROOTField->Connect("ReturnPressed()","EVBMainFrame",this,"UpdateROOTdir()");
	fOpenROOTButton = new TGTextButton(ROOTFrame, "Open");
	fOpenROOTButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenROOTdir()");
	ROOTFrame->AddFrame(rootLabel, lhints);
	ROOTFrame->AddFrame(fROOTField, fhints);
	ROOTFrame->AddFrame(fOpenROOTButton, bhints);

	TGHorizontalFrame *BINFrame = new TGHorizontalFrame(NameFrame, w*0.5, h*0.1);
	TGLabel* binlabel = new TGLabel(BINFrame, "BIN Archive Directory:");
	fBINField = new TGTextEntry(BINFrame, new TGTextBuffer(120), BINDIR);
	fBINField->Resize(w*0.25, fBINField->GetDefaultHeight());
	fBINField->Connect("ReturnPressed()","EVBMainFrame",this,"UpdateBINdir()");
	fOpenBINButton = new TGTextButton(BINFrame, "Open");
	fOpenBINButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenBINdir()");
	BINFrame->AddFrame(binlabel, lhints);
	BINFrame->AddFrame(fBINField, fhints);
	BINFrame->AddFrame(fOpenBINButton, bhints);

	TGHorizontalFrame *CMapFrame = new TGHorizontalFrame(NameFrame, w*0.5, h*0.1);
	TGLabel* cmaplabel = new TGLabel(CMapFrame, "Channel Map File:");
	fCMapField = new TGTextEntry(CMapFrame, new TGTextBuffer(120), CMAP);
	fCMapField->Resize(w*0.25, fCMapField->GetDefaultHeight());
	fCMapField->Connect("ReturnPressed()","EVBMainFrame",this,"UpdateCMap()");
	fOpenCMapButton = new TGTextButton(CMapFrame, "Open");
	fOpenCMapButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenCMapfile()");
	CMapFrame->AddFrame(cmaplabel, lhints);
	CMapFrame->AddFrame(fCMapField, fhints);
	CMapFrame->AddFrame(fOpenCMapButton, bhints);

	TGHorizontalFrame *SMapFrame = new TGHorizontalFrame(NameFrame, w*0.5, h*0.1);
	TGLabel* smaplabel = new TGLabel(SMapFrame, "Board Shift File:");
	fSMapField = new TGTextEntry(SMapFrame, new TGTextBuffer(120), SMAP);
	fSMapField->Resize(w*0.25, fSMapField->GetDefaultHeight());
	fSMapField->Connect("ReturnPressed()","EVBMainFrame",this,"UpdateSMap()");
	fOpenSMapButton = new TGTextButton(SMapFrame, "Open");
	fOpenSMapButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenSMapfile()");
	SMapFrame->AddFrame(smaplabel, lhints);
	SMapFrame->AddFrame(fSMapField, fhints);
	SMapFrame->AddFrame(fOpenSMapButton, bhints);

	NameFrame->AddFrame(ROOTFrame, fhints);
	NameFrame->AddFrame(BINFrame, fhints);
	NameFrame->AddFrame(CMapFrame, fhints);
	NameFrame->AddFrame(SMapFrame, fhints);


	TGHorizontalFrame *ParamMagFrame = new TGHorizontalFrame(InputFrame, w*0.5, h*0.1);
	TGLabel *bkelabel = new TGLabel(ParamMagFrame, "Beam KE (MeV):");
	fBKEField = new TGNumberEntryField(ParamMagFrame, BKE, 0, TGNumberEntry::kNESRealFour, TGNumberEntry::kNEANonNegative);
	TGLabel *bfieldlabel = new TGLabel(ParamMagFrame, "B-Field (G):");
	fBField = new TGNumberEntryField(ParamMagFrame, BFIELD, 0, TGNumberEntry::kNESRealFour, TGNumberEntry::kNEANonNegative);
	TGLabel *thetalabel = new TGLabel(ParamMagFrame, "Angle (deg):");
	fThetaField = new TGNumberEntryField(ParamMagFrame, THETA, 0, TGNumberEntry::kNESRealFour, TGNumberEntry::kNEANonNegative);
	ParamMagFrame->AddFrame(bkelabel, lhints);
	ParamMagFrame->AddFrame(fBKEField, fhints);
	ParamMagFrame->AddFrame(bfieldlabel, lhints);
	ParamMagFrame->AddFrame(fBField, fhints);
	ParamMagFrame->AddFrame(thetalabel, lhints);
	ParamMagFrame->AddFrame(fThetaField, fhints);

	TGHorizontalFrame *ParamNucFrame = new TGHorizontalFrame(InputFrame, w*0.5, h*0.1);
	TGLabel *ztlabel = new TGLabel(ParamNucFrame, "ZT:");
	fZTField = new TGNumberEntryField(ParamNucFrame, ZT, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	TGLabel *atlabel = new TGLabel(ParamNucFrame, "AT:");
	fATField = new TGNumberEntryField(ParamNucFrame, AT, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	TGLabel *zplabel = new TGLabel(ParamNucFrame, "ZP:");
	fZPField = new TGNumberEntryField(ParamNucFrame, ZP, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	TGLabel *aplabel = new TGLabel(ParamNucFrame, "AP:");
	fAPField = new TGNumberEntryField(ParamNucFrame, AP, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	TGLabel *zelabel = new TGLabel(ParamNucFrame, "ZE:");
	fZEField = new TGNumberEntryField(ParamNucFrame, ZE, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	TGLabel *aelabel = new TGLabel(ParamNucFrame, "AE:");
	fAEField = new TGNumberEntryField(ParamNucFrame, AE, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	ParamNucFrame->AddFrame(ztlabel, lhints);
	ParamNucFrame->AddFrame(fZTField, fhints);
	ParamNucFrame->AddFrame(atlabel, lhints);
	ParamNucFrame->AddFrame(fATField, fhints);
	ParamNucFrame->AddFrame(zplabel, lhints);
	ParamNucFrame->AddFrame(fZPField, fhints);
	ParamNucFrame->AddFrame(aplabel, lhints);
	ParamNucFrame->AddFrame(fAPField, fhints);
	ParamNucFrame->AddFrame(zelabel, lhints);
	ParamNucFrame->AddFrame(fZEField, fhints);
	ParamNucFrame->AddFrame(aelabel, lhints);
	ParamNucFrame->AddFrame(fAEField, fhints);

	TGHorizontalFrame *Window1Frame = new TGHorizontalFrame(InputFrame, w*0.5, h*0.1);
	TGLabel *slowlabel = new TGLabel(Window1Frame, "Slow Coincidence Window (ps):");
	fSlowWindowField = new TGNumberEntryField(Window1Frame, SLOWWIND, 0, TGNumberEntry::kNESReal, TGNumberEntry::kNEANonNegative);
	Window1Frame->AddFrame(slowlabel, lhints);
	Window1Frame->AddFrame(fSlowWindowField, fhints);

	TGHorizontalFrame *Window2Frame = new TGHorizontalFrame(InputFrame, w*0.5, h*0.1);
	TGLabel *fasticlabel = new TGLabel(Window2Frame, "Fast Coincidence Window IC (ps):");
	fFastICField = new TGNumberEntryField(Window2Frame, FASTWIND_IC, 0, TGNumberEntry::kNESReal, TGNumberEntry::kNEANonNegative);
	TGLabel *fastsabrelabel = new TGLabel(Window2Frame, "Fast Coincidence Window SABRE (ps):");
	fFastSABREField = new TGNumberEntryField(Window2Frame, FASTWIND_SABRE, 0, TGNumberEntry::kNESReal, TGNumberEntry::kNEANonNegative);
	Window2Frame->AddFrame(fasticlabel, lhints);
	Window2Frame->AddFrame(fFastICField, fhints);
	Window2Frame->AddFrame(fastsabrelabel, lhints);
	Window2Frame->AddFrame(fFastSABREField, fhints);

	TGHorizontalFrame *RunFrame = new TGHorizontalFrame(InputFrame, w*0.5, h*0.1);
	TGLabel *typelabel = new TGLabel(RunFrame, "Operation Type:");
	fTypeBox = new TGComboBox(RunFrame, TYPEBOX);
	fTypeBox->AddEntry("Build Events", GWMEventBuilder::BUILD_ALL);
	fTypeBox->AddEntry("Build Slow Events", GWMEventBuilder::BUILD_SLOW);
	fTypeBox->AddEntry("Build Fast Events", GWMEventBuilder::BUILD_FAST);
	fTypeBox->AddEntry("Analyze Slow Events", GWMEventBuilder::ANALYZE_SLOW);
	fTypeBox->AddEntry("Analyze Fast Events", GWMEventBuilder::ANALYZE_FAST);
	fTypeBox->AddEntry("Convert Archive to ROOT", GWMEventBuilder::CONVERT);
	fTypeBox->AddEntry("Archive BIN Files", GWMEventBuilder::ARCHIVE);
	fTypeBox->AddEntry("Merge ROOT Files", GWMEventBuilder::MERGE);
	fTypeBox->AddEntry("Make Histograms", GWMEventBuilder::PLOT);
	fTypeBox->Resize(200,20);
	fTypeBox->Connect("Selected(Int_t, Int_t)","EVBMainFrame",this,"HandleTypeSelection(Int_t,Int_t)");
	TGLabel *rminlabel = new TGLabel(RunFrame, "Min Run:");
	fRMinField = new TGNumberEntryField(RunFrame, RMIN, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	TGLabel *rmaxlabel = new TGLabel(RunFrame, "Max Run:");
	fRMaxField = new TGNumberEntryField(RunFrame, RMAX, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	fRunButton = new TGTextButton(RunFrame, "Run!");
	fRunButton->SetState(kButtonDisabled);
	fRunButton->Connect("Clicked()","EVBMainFrame",this,"DoRun()");
	RunFrame->AddFrame(typelabel, lhints);
	RunFrame->AddFrame(fTypeBox, fhints);
	RunFrame->AddFrame(rminlabel, lhints);
	RunFrame->AddFrame(fRMinField, fhints);
	RunFrame->AddFrame(rmaxlabel, lhints);
	RunFrame->AddFrame(fRMaxField, fhints);
	RunFrame->AddFrame(fRunButton, bhints);

	InputFrame->AddFrame(NameFrame, fhints);
	InputFrame->AddFrame(ParamMagFrame, fhints);
	InputFrame->AddFrame(ParamNucFrame, fhints);
	InputFrame->AddFrame(Window1Frame, fhints);
	InputFrame->AddFrame(Window2Frame, fhints);
	InputFrame->AddFrame(RunFrame, fhints);

	//fUpdateTimer = new TTimer(100);
	//fUpdateTimer->Connect("Timeout()","EVBMainFrame",this,"UpdateConsole()");

	AddFrame(ConsoleFrame, fchints);
	AddFrame(InputFrame, fchints);

	SetWindowName("GWM Event Builder");
	MapSubwindows();
	Resize();
	MapWindow();
	//fUpdateTimer->TurnOn();

}

EVBMainFrame::~EVBMainFrame() {
	Cleanup();
	//delete fUpdateTimer;
	delete this;
}

void EVBMainFrame::CloseWindow() {
	//fUpdateTimer->TurnOff();
	gApplication->Terminate();
}

void EVBMainFrame::HandleMenuSelection(int id) {}

void EVBMainFrame::DoOpenROOTdir() {
	new FileViewFrame(gClient->GetRoot(), this, MAIN_W*0.5, MAIN_H*0.25, this, ROOTDIR);
}

void EVBMainFrame::DoOpenBINdir() {
	new FileViewFrame(gClient->GetRoot(), this, MAIN_W*0.5, MAIN_H*0.25, this, BINDIR);
}

void EVBMainFrame::DoOpenCMapfile() {
	new FileViewFrame(gClient->GetRoot(), this, MAIN_W*0.5, MAIN_H*0.25, this, CMAP);	
}

void EVBMainFrame::DoOpenSMapfile() {
	new FileViewFrame(gClient->GetRoot(), this, MAIN_W*0.5, MAIN_H*0.25, this, SMAP);	
}

void EVBMainFrame::DoRun() {
	fConsole->Clear();

	DisableAllInput();

	SetParameters();

	int type = fTypeBox->GetSelected();
	fBuilder.SetAnalysisType(type);
	//fWorkerThread->Run(this);
	switch(type) {
		case GWMEventBuilder::ARCHIVE : 
		{
			break;
		}
		case GWMEventBuilder::PLOT :
		{
			break;
		}
		case GWMEventBuilder::CONVERT :
		{
			break;
		}
		case GWMEventBuilder::MERGE :
		{
			break;
		}
		default :
		{
			fBuilder.BuildEvents();
		}
	}
	//fWorkerThread->SetCancelAsynchronous();

	EnableAllInput();
}

void EVBMainFrame::HandleTypeSelection(int box, int entry) {
	fRunButton->SetState(kButtonUp);
}

bool EVBMainFrame::SetParameters() {
	fBuilder.SetRunRange(fRMinField->GetIntNumber(), fRMaxField->GetIntNumber());
	fBuilder.SetSlowCoincidenceWindow(fSlowWindowField->GetNumber());
	fBuilder.SetFastWindowIonChamber(fFastICField->GetNumber());
	fBuilder.SetFastWindowSABRE(fFastSABREField->GetNumber());
	bool test = fBuilder.SetKinematicParameters(fZTField->GetIntNumber(), fATField->GetIntNumber(),
												fZPField->GetIntNumber(), fAPField->GetIntNumber(),
												fZEField->GetIntNumber(), fAEField->GetIntNumber(),
												fBField->GetNumber(), fThetaField->GetNumber(),
												fBKEField->GetNumber());
	return test;
}

void EVBMainFrame::DisplayROOTdir(const char* dir) {
	fROOTField->SetText(dir);
	fBuilder.SetROOTDirectory(dir);
}

void EVBMainFrame::DisplayBINdir(const char* dir) {
	fBINField->SetText(dir);
	fBuilder.SetBinaryDirectory(dir);
}

void EVBMainFrame::DisplayCMap(const char* file) {
	fCMapField->SetText(file);
	fBuilder.SetChannelMap(file);
}

void EVBMainFrame::DisplaySMap(const char* file) {
	fSMapField->SetText(file);
	fBuilder.SetBoardShiftFile(file);
}

void EVBMainFrame::UpdateROOTdir() {
	const char* dir = fROOTField->GetText();
	fBuilder.SetROOTDirectory(dir);
}

void EVBMainFrame::UpdateBINdir() {
	const char* dir = fBINField->GetText();
	fBuilder.SetBinaryDirectory(dir);
}

void EVBMainFrame::UpdateSMap() {
	const char* file = fSMapField->GetText();
	fBuilder.SetBoardShiftFile(file);
}

void EVBMainFrame::UpdateCMap() {
	const char* file = fCMapField->GetText();
	fBuilder.SetChannelMap(file);
}

void EVBMainFrame::RunArchive(const char* dir, int number) {}

void EVBMainFrame::RunPlot(const char* file, const char* cutlist) {}

void EVBMainFrame::RunMerge(const char* file, const char* dir) {}

void EVBMainFrame::DisableAllInput() {
	fRunButton->SetState(kButtonDisabled);
	fOpenROOTButton->SetState(kButtonDisabled);
	fOpenBINButton->SetState(kButtonDisabled);
	fOpenCMapButton->SetState(kButtonDisabled);
	fOpenSMapButton->SetState(kButtonDisabled);

	fROOTField->SetState(false);
	fBINField->SetState(false);
	fCMapField->SetState(false);
	fSMapField->SetState(false);

	fTypeBox->SetEnabled(false);

	fZTField->SetState(false);
	fATField->SetState(false);
	fZPField->SetState(false);
	fAPField->SetState(false);
	fZEField->SetState(false);
	fAEField->SetState(false);

	fBField->SetState(false);
	fBKEField->SetState(false);
	fThetaField->SetState(false);

	fRMaxField->SetState(false);
	fRMinField->SetState(false);

	fSlowWindowField->SetState(false);
	fFastICField->SetState(false);
	fFastSABREField->SetState(false);
}

void EVBMainFrame::EnableAllInput() {
	fRunButton->SetState(kButtonUp);
	fOpenROOTButton->SetState(kButtonUp);
	fOpenBINButton->SetState(kButtonUp);
	fOpenCMapButton->SetState(kButtonUp);
	fOpenSMapButton->SetState(kButtonUp);

	fROOTField->SetState(true);
	fBINField->SetState(true);
	fCMapField->SetState(true);
	fSMapField->SetState(true);

	fTypeBox->SetEnabled(true);

	fZTField->SetState(true);
	fATField->SetState(true);
	fZPField->SetState(true);
	fAPField->SetState(true);
	fZEField->SetState(true);
	fAEField->SetState(true);

	fBField->SetState(true);
	fBKEField->SetState(true);
	fThetaField->SetState(true);

	fRMaxField->SetState(true);
	fRMinField->SetState(true);

	fSlowWindowField->SetState(true);
	fFastICField->SetState(true);
	fFastSABREField->SetState(true);

}

void EVBMainFrame::UpdateConsole() {
	fConsole->ShowBottom();
	fConsole->Update();
}