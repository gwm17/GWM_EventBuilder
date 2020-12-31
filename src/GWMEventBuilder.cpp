/*
	GWMEventBuilder.cpp
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#include "EventBuilder.h"
#include <cstdlib>
#include "GWMEventBuilder.h"
#include "RunCollector.h"
#include "CompassRun.h"
#include "SlowSort.h"
#include "FastSort.h"
#include "SFPAnalyzer.h"
#include "SFPPlotter.h"

GWMEventBuilder::GWMEventBuilder() :
	m_rmin(0), m_rmax(0), m_ZT(0), m_AT(0), m_ZP(0), m_AP(0), m_ZE(0), m_AE(0), m_ZR(0), m_AR(0),
	m_B(0), m_Theta(0), m_BKE(0), m_binpath("none"), m_rootpath("none"), m_mapfile("none"), m_shiftfile("none"),
	m_cutList("none"), m_plotfile("none"), m_binfilepath("none"), m_mergefile("none"), m_mergeIndir("none"),
	m_SlowWindow(0), m_FastWindowIonCh(0), m_FastWindowSABRE(0), m_pb(nullptr)
{
}

GWMEventBuilder::~GWMEventBuilder() 
{
}

bool GWMEventBuilder::ReadConfigFile(std::string& fullpath) {
	std::cout<<"Reading in configuration from file: "<<fullpath<<std::endl;
	std::ifstream input(fullpath);
	if(!input.is_open()) {
		std::cout<<"Read failed! Unable to open input file!"<<std::endl;
		return false;
	}
	std::string junk;

	std::getline(input, junk);
	input>>junk>>m_rootpath;
	input>>junk>>m_binpath;
	input>>junk>>m_binfilepath;
	input>>junk>>m_mergeIndir;
	input>>junk>>m_mergefile;
	input>>junk>>m_plotfile;
	input>>junk;
	std::getline(input, junk);
	std::getline(input, junk);
	input>>junk>>m_mapfile;
	input>>junk>>m_scalerfile;
	input>>junk>>m_cutList;
	input>>junk>>m_ZT>>junk>>m_AT;
	input>>junk>>m_ZP>>junk>>m_AP;
	input>>junk>>m_ZE>>junk>>m_AE;
	input>>junk>>m_B;
	input>>junk>>m_BKE;
	input>>junk>>m_Theta;
	input>>junk;
	std::getline(input, junk);
	std::getline(input, junk);
	input>>junk>>m_shiftfile;
	input>>junk>>m_SlowWindow;
	input>>junk>>m_FastWindowIonCh;
	input>>junk>>m_FastWindowSABRE;
	input>>junk;
	std::getline(input, junk);
	std::getline(input, junk);
	input>>junk>>m_rmin;
	input>>junk>>m_rmax;

	input.close();

	std::cout<<"Completed."<<std::endl;

	return true;
}

void GWMEventBuilder::WriteConfigFile(std::string& fullpath) {

	std::cout<<"Writing out configuration to file: "<<fullpath<<std::endl;
	std::ofstream output(fullpath);
	if(!output.is_open()) {
		std::cout<<"Write failed! Unable to open output file!"<<std::endl;
		return;
	}

	output<<"-------Data Locations----------"<<std::endl;
	output<<"ROOTDirectory: "<<m_rootpath<<std::endl;
	output<<"BinaryArchiveDirectory: "<<m_binpath<<std::endl;
	output<<"BinaryFileDirectory: "<<m_binfilepath<<std::endl;
	output<<"MergeInputDirectory: "<<m_mergeIndir<<std::endl;
	output<<"MergeOutputFile: "<<m_mergefile<<std::endl;
	output<<"PlotOutputFile: "<<m_plotfile<<std::endl;
	output<<"-------------------------------"<<std::endl;
	output<<"------Experimental Inputs------"<<std::endl;
	output<<"ChannelMapFile: "<<m_mapfile<<std::endl;
	output<<"ScalerFile: "<<m_scalerfile<<std::endl;
	output<<"CutListFile: "<<m_cutList<<std::endl;
	output<<"ZT: "<<m_ZT<<std::endl;
	output<<"AT: "<<m_AT<<std::endl;
	output<<"ZP: "<<m_ZP<<std::endl;
	output<<"AP: "<<m_AP<<std::endl;
	output<<"ZE: "<<m_ZE<<std::endl;
	output<<"AE: "<<m_AE<<std::endl;
	output<<"BField(G): "<<m_B<<std::endl;
	output<<"BeamKE(MeV): "<<m_BKE<<std::endl;
	output<<"Theta(deg): "<<m_Theta<<std::endl;
	output<<"-------------------------------"<<std::endl;
	output<<"-------Timing Information------"<<std::endl;
	output<<"BoardOffsetFile: "<<m_shiftfile<<std::endl;
	output<<"SlowCoincidenceWindow(ps): "<<m_SlowWindow<<std::endl;
	output<<"FastCoincidenceWindow_IonCh(ps): "<<m_FastWindowIonCh<<std::endl;
	output<<"FastCoincidenceWindow_SABRE(ps): "<<m_FastWindowSABRE<<std::endl;
	output<<"-------------------------------"<<std::endl;
	output<<"--------Run Information--------"<<std::endl;
	output<<"MinRun: "<<m_rmin<<std::endl;
	output<<"MaxRun: "<<m_rmax<<std::endl;
	output<<"-------------------------------"<<std::endl;

	output.close();

	std::cout<<"Completed."<<std::endl;

}

void GWMEventBuilder::PlotHistograms() {
	std::string analyze_dir = m_rootpath+"/analyzed/";
	SFPPlotter grammer;
	if(m_cutList != "none") {
		if(!grammer.ReadCutlist(m_cutList)) {
			std::cout<<"Bad cutlist at PlotHistograms"<<std::endl;
			return;
		}
	}
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Generating a histogram file from analyzed files"<<std::endl;
	std::cout<<"Analyzed directory: "<<analyze_dir<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	std::cout<<"Cut List File: "<<m_cutList<<std::endl;
	std::cout<<"Histogram File: "<<m_plotfile<<std::endl;

	if(m_pb) grammer.AttachProgressBar(m_pb);

	if(CollectRuns(analyze_dir, "", ".root", m_rmin, m_rmax)) {
		std::cout<<"Working...";
		grammer.Run(m_currentFiles, m_plotfile);
		std::cout<<" Complete."<<std::endl;
	} else {
		std::cout<<"Unable to find files at PlotHistograms"<<std::endl;
	}
	std::cout<<"-------------------------------------------"<<std::endl;
}

void GWMEventBuilder::Convert2RawRoot() {
	std::string rawroot_dir = m_rootpath+"/raw_root/";
	std::string unpack_dir = m_rootpath+"/temp_binary/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Converting Binary file Archive to ROOT file"<<std::endl;
	std::cout<<"Binary Archive Directory: "<<m_binpath<<std::endl;
	std::cout<<"Temporary Unpack Directory: "<<unpack_dir<<std::endl;
	std::cout<<"Timestamp Shift File: "<<m_shiftfile<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	

	grabber.SetSearchParams(m_binpath, "", ".tar.gz",0,1000);

	std::cout<<"ROOT Output Directory: "<<m_rootpath<<std::endl;

	std::string rawfile, binfile;
	std::string unpack_command, wipe_command;

	CompassRun converter(unpack_dir);
	converter.SetShiftMap(m_shiftfile);
	converter.SetScalerInput(m_scalerfile);
	if(m_pb) converter.AttachProgressBar(m_pb);

	std::cout<<"Beginning conversion..."<<std::endl;

	for(int i=m_rmin; i<=m_rmax; i++) {
		binfile = grabber.GrabFile(i);
		if(binfile == "") continue;
		converter.SetRunNumber(i);
		std::cout<<"Converting file: "<<binfile<<std::endl;

		rawfile = rawroot_dir + "compass_run_"+ to_string(i) + ".root";
		unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
		wipe_command = "rm -r "+unpack_dir+"*.bin";

		system(unpack_command.c_str());
		converter.Convert2RawRoot(rawfile);
		system(wipe_command.c_str());

	}
	std::cout<<std::endl<<"Conversion complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;

}

void GWMEventBuilder::MergeROOTFiles() {
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Merging ROOT files into single ROOT file"<<std::endl;
	std::cout<<"ROOT file directory: "<<m_mergeIndir<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	std::cout<<"Output file: "<<m_mergefile<<std::endl;
	std::string prefix = "";
	std::string suffix = ".root";
	grabber.SetSearchParams(m_mergeIndir, prefix, suffix,m_rmin,m_rmax);
	m_currentFiles.clear();
	std::cout<<"Beginning the merge...";
	if(!grabber.Merge_TChain(m_mergefile)) {
		std::cout<<"Unable to find files at MergeROOTFiles"<<std::endl;
		return;
	}
	std::cout<<" Complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}

void GWMEventBuilder::ArchiveBinaryFiles(int runNum, bool segmented) {
	std::string location = m_binfilepath+"/"+to_string(runNum)+"/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Archiving binary data files"<<std::endl;
	std::cout<<"Binary file directory: "<<location<<std::endl;
	std::cout<<"Run Number: "<<runNum<<std::endl;
	std::cout<<"Binary archive directory: "<<m_binpath<<std::endl;
	std::cout<<"Archives are formated run_<run_number>.tar.gz"<<std::endl;
	if(segmented) {
		std::cout<<"Segmented files are currently not supported, archive not generated"<<std::endl;
		return;
	}
	std::cout<<"Running tar command... Check your terminal..."<<std::endl;
	std::string command = "tar -cvzf "+m_binpath+"run_"+to_string(runNum)+".tar.gz "+location+"*.bin";
	std::system(command.c_str());
	std::cout<<"Finished."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}

bool GWMEventBuilder::CollectRuns(std::string& dir, std::string prefix, std::string suffix, int min, int max, bool ranged) {
	if(dir == grabber.GetSearchDir() && prefix == grabber.GetSearchPrefix() && suffix == grabber.GetSearchSuffix() && min == grabber.GetRunMin() && max == grabber.GetRunMax() &&
		m_currentFiles.size() != 0) {
		return true;
	}

	grabber.SetSearchParams(dir, prefix, suffix, min, max);
	if(ranged) {
		if(grabber.GrabFilesInRange()) {
			m_currentFiles = grabber.filelist;
			return true;
		} else {
			m_currentFiles.clear();
			return false;
		}
	} else {
		if(grabber.GrabAllFiles()) {
			m_currentFiles = grabber.filelist;
			return true;
		} else {
			m_currentFiles.clear();
			return false;
		}
	}
}

void GWMEventBuilder::Convert2SortedRoot() {
	std::string sortroot_dir = m_rootpath+"/sorted/";
	std::string unpack_dir = m_rootpath+"/temp_binary/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Converting Binary file Archive to ROOT file"<<std::endl;
	std::cout<<"Binary Archive Directory: "<<m_binpath<<std::endl;
	std::cout<<"Temporary Unpack Directory: "<<unpack_dir<<std::endl;
	std::cout<<"Timestamp Shift File: "<<m_shiftfile<<std::endl;
	std::cout<<"Channel Map File: "<<m_mapfile<<std::endl;
	std::cout<<"Slow Coincidence Window(ps): "<<m_SlowWindow<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;

	grabber.SetSearchParams(m_binpath,"",".tar.gz",m_rmin,m_rmax);

	std::cout<<"ROOT Output Directory: "<<m_rootpath<<std::endl;

	std::string sortfile, binfile;
	std::string unpack_command, wipe_command;

	CompassRun converter(unpack_dir);
	converter.SetShiftMap(m_shiftfile);
	converter.SetScalerInput(m_scalerfile);
	if(m_pb) converter.AttachProgressBar(m_pb);

	std::cout<<"Beginning conversion..."<<std::endl;

	for(int i=m_rmin; i<= m_rmax; i++) {
		binfile = grabber.GrabFile(i);
		if(binfile == "") continue;
		converter.SetRunNumber(i);
		std::cout<<"Converting file: "<<binfile<<std::endl;

		sortfile = sortroot_dir +"run_"+to_string(i)+ ".root";
		unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
		wipe_command = "rm -r "+unpack_dir+"*.bin";

		system(unpack_command.c_str());
		converter.Convert2SortedRoot(sortfile, m_mapfile, m_SlowWindow);
		system(wipe_command.c_str());

	}
	std::cout<<std::endl<<"Conversion complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}

void GWMEventBuilder::Convert2FastSortedRoot() {
	std::string sortroot_dir = m_rootpath+"/fast/";
	std::string unpack_dir = m_rootpath+"/temp_binary/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Converting Binary file Archive to ROOT file"<<std::endl;
	std::cout<<"Binary Archive Directory: "<<m_binpath<<std::endl;
	std::cout<<"Temporary Unpack Directory: "<<unpack_dir<<std::endl;
	std::cout<<"Timestamp Shift File: "<<m_shiftfile<<std::endl;
	std::cout<<"Channel Map File: "<<m_mapfile<<std::endl;
	std::cout<<"Slow Coincidence Window(ps): "<<m_SlowWindow<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;

	grabber.SetSearchParams(m_binpath,"",".tar.gz",m_rmin,m_rmax);

	std::cout<<"ROOT Output Directory: "<<m_rootpath<<std::endl;

	std::string sortfile, binfile;
	std::string unpack_command, wipe_command;

	CompassRun converter(unpack_dir);
	converter.SetShiftMap(m_shiftfile);
	converter.SetScalerInput(m_scalerfile);
	if(m_pb) converter.AttachProgressBar(m_pb);

	std::cout<<"Beginning conversion..."<<std::endl;

	for(int i=m_rmin; i<=m_rmax; i++) {
		binfile = grabber.GrabFile(i);
		if(binfile == "") continue;
		converter.SetRunNumber(i);
		std::cout<<"Converting file: "<<binfile<<std::endl;

		sortfile = sortroot_dir + "run_" + to_string(i) + ".root";
		unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
		wipe_command = "rm -r "+unpack_dir+"*.bin";

		system(unpack_command.c_str());
		converter.Convert2FastSortedRoot(sortfile, m_mapfile, m_SlowWindow, m_FastWindowSABRE, m_FastWindowIonCh);
		system(wipe_command.c_str());

	}
	std::cout<<std::endl<<"Conversion complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}

void GWMEventBuilder::Convert2SlowAnalyzedRoot() {
	std::string sortroot_dir = m_rootpath+"/analyzed/";
	std::string unpack_dir = m_rootpath+"/temp_binary/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Converting Binary file Archive to ROOT file"<<std::endl;
	std::cout<<"Binary Archive Directory: "<<m_binpath<<std::endl;
	std::cout<<"Temporary Unpack Directory: "<<unpack_dir<<std::endl;
	std::cout<<"Timestamp Shift File: "<<m_shiftfile<<std::endl;
	std::cout<<"Channel Map File: "<<m_mapfile<<std::endl;
	std::cout<<"Slow Coincidence Window(ps): "<<m_SlowWindow<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	
	grabber.SetSearchParams(m_binpath,"",".tar.gz",m_rmin, m_rmax);
	std::cout<<"ROOT Output Directory: "<<m_rootpath<<std::endl;

	std::string sortfile, binfile;
	std::string unpack_command, wipe_command;

	CompassRun converter(unpack_dir);
	converter.SetShiftMap(m_shiftfile);
	converter.SetScalerInput(m_scalerfile);
	if(m_pb) converter.AttachProgressBar(m_pb);

	std::cout<<"Beginning conversion..."<<std::endl;

	for(int i=m_rmin; i<=m_rmax; i++) {
		binfile = grabber.GrabFile(i);
		if(binfile == "") continue;
		converter.SetRunNumber(i);
		std::cout<<"Converting file: "<<binfile<<std::endl;

		sortfile = sortroot_dir + "run_" + to_string(i) + ".root";
		unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
		wipe_command = "rm -r "+unpack_dir+"*.bin";

		system(unpack_command.c_str());
		converter.Convert2SlowAnalyzedRoot(sortfile, m_mapfile, m_SlowWindow, m_ZT, m_AT, m_ZP, m_AP, m_ZE, m_AE, m_BKE, m_B, m_Theta);
		system(wipe_command.c_str());

	}
	std::cout<<std::endl<<"Conversion complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}

void GWMEventBuilder::Convert2FastAnalyzedRoot() {
	std::string sortroot_dir = m_rootpath+"/analyzed/";
	std::string unpack_dir = m_rootpath+"/temp_binary/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Converting Binary file Archive to ROOT file"<<std::endl;
	std::cout<<"Binary Archive Directory: "<<m_binpath<<std::endl;
	std::cout<<"Temporary Unpack Directory: "<<unpack_dir<<std::endl;
	std::cout<<"Timestamp Shift File: "<<m_shiftfile<<std::endl;
	std::cout<<"Channel Map File: "<<m_mapfile<<std::endl;
	std::cout<<"Slow Coincidence Window(ps): "<<m_SlowWindow<<std::endl;
	std::cout<<"Fast Ion Chamber Coincidence Window(ps): "<<m_FastWindowIonCh<<std::endl;
	std::cout<<"Fast SABRE Coincidence Window(ps): "<<m_FastWindowSABRE<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	
	grabber.SetSearchParams(m_binpath,"",".tar.gz",m_rmin,m_rmax);

	std::cout<<"ROOT Output Directory: "<<m_rootpath<<std::endl;

	std::string sortfile, binfile;
	std::string unpack_command, wipe_command;

	CompassRun converter(unpack_dir);
	converter.SetShiftMap(m_shiftfile);
	converter.SetScalerInput(m_scalerfile);
	if(m_pb) converter.AttachProgressBar(m_pb);

	std::cout<<"Beginning conversion..."<<std::endl;

	for(int i=m_rmin; i<=m_rmax; i++) {
		binfile = grabber.GrabFile(i);
		if(binfile == "") continue;
		converter.SetRunNumber(i);
		std::cout<<"Converting file: "<<binfile<<std::endl;

		sortfile = sortroot_dir + "run_" + to_string(i) + ".root";
		unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
		wipe_command = "rm -r "+unpack_dir+"*.bin";

		system(unpack_command.c_str());
		converter.Convert2FastAnalyzedRoot(sortfile, m_mapfile, m_SlowWindow, m_FastWindowSABRE, m_FastWindowIonCh, m_ZT, m_AT, m_ZP, m_AP, m_ZE, m_AE, m_BKE, m_B, m_Theta);
		system(wipe_command.c_str());

	}
	std::cout<<std::endl<<"Conversion complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}

bool GWMEventBuilder::SetKinematicParameters(int zt, int at, int zp, int ap, int ze, int ae, double b, double theta, double bke) {

	if((at + ap - ae) < 0 || (zt + zp - ze) < 0) {
		std::cout<<"Invalid Parameters at SetKinematicParameters"<<std::endl;
		return false;
	}
	m_ZT = zt; m_AT = at; m_ZP = zp; m_AP = ap; m_ZE = ze; m_AE = ae;
	m_B = b; m_Theta = theta; m_BKE = bke;

	m_ZR = (zt + zp - ze);
	m_AR = (at + ap - ae);

	return true;
}