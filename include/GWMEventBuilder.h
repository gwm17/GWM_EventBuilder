/*
	GWMEventBuilder.h
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#ifndef GWMEVENTBUILDER_H
#define GWMEVENTBUILDER_H

#include "RunCollector.h"

class GWMEventBuilder {
public:
	GWMEventBuilder();
	~GWMEventBuilder();

	bool ReadConfigFile(std::string& filename);
	void WriteConfigFile(std::string& filename);

	void BuildEvents();
	void PlotHistograms();
	void ConvertBin2ROOT();
	void MergeROOTFiles();
	void ArchiveBinaryFiles(int runNum, bool segmented=false);
	void Convert2SortedRoot();
	void Convert2FastSortedRoot();
	void Convert2RawRoot();
	void Convert2SlowAnalyzedRoot();
	void Convert2FastAnalyzedRoot();

	inline void SetAnalysisType(int type) { m_analysisType = type;};
	inline int GetAnalysisType() { return m_analysisType; };

	inline void SetRunRange(int rmin, int rmax) { m_rmin = rmin; m_rmax = rmax; };
	inline void SetBinaryDirectory(const char* fullpath) { m_binpath = fullpath; };
	inline void SetROOTDirectory(const char* fullpath) { m_rootpath = fullpath; };
	inline void SetChannelMap(const char* name) { m_mapfile = name; };
	inline void SetBoardShiftFile(const char* name) { m_shiftfile = name; };
	inline void SetSlowCoincidenceWindow(double window) { m_SlowWindow = window; };
	inline void SetFastWindowIonChamber(double window) { m_FastWindowIonCh = window; };
	inline void SetFastWindowSABRE(double window) { m_FastWindowSABRE = window; };
	inline void SetCutList(const char* name) { m_cutList = name; };
	inline void SetPlotFile(const char* name) { m_plotfile = name; };
	inline void SetBinaryFileDirectory(const char* fullpath) { m_binfilepath = fullpath; };
	inline void SetMergeFile(const char* name) { m_mergefile = name; };
	inline void SetMergeInputDirectory(const char* fullpath) { m_mergeIndir = fullpath; };
	inline void SetScalerFile(const char* fullpath) { m_scalerfile = fullpath; };
	bool SetKinematicParameters(int zt, int at, int zp, int ap, int ze, int ae, double b, double theta, double bke);

	inline int GetRunMin() {return m_rmin;};
	inline int GetRunMax() {return m_rmax;};
	inline const char* GetBinaryDirectory() {return m_binpath.c_str();};
	inline const char* GetROOTDirectory() {return m_rootpath.c_str();};
	inline int GetTargetZ() {return m_ZT;};
	inline int GetTargetA() {return m_AT;};
	inline int GetProjectileZ() {return m_ZP;};
	inline int GetProjectileA() {return m_AP;};
	inline int GetEjectileZ() {return m_ZE;};
	inline int GetEjectileA() {return m_AE;};
	inline int GetResidualZ() {return m_ZR;};
	inline int GetResidualA() {return m_AR;};
	inline double GetBField() {return m_B;};
	inline double GetBeamKE() {return m_BKE;};
	inline double GetTheta() {return m_Theta;};
	inline double GetSlowCoincidenceWindow() { return m_SlowWindow; };
	inline double GetFastWindowIonChamber() { return m_FastWindowIonCh; };
	inline double GetFastWindowSABRE() { return m_FastWindowSABRE; };
	inline const char* GetChannelMap() { return m_mapfile.c_str(); };
	inline const char* GetBoardShiftFile() { return m_shiftfile.c_str(); };
	inline const char* GetCutList() { return m_cutList.c_str(); };
	inline const char* GetPlotFile() { return m_plotfile.c_str(); };
	inline const char* GetBinaryFileDirectory() { return m_binfilepath.c_str(); };
	inline const char* GetMergeFile() { return m_mergefile.c_str(); };
	inline const char* GetMergeInputDirectory() { return m_mergeIndir.c_str(); };
	inline const char* GetScalerFile() { return m_scalerfile.c_str(); };



	inline void SetStream(std::ostream* stream) { m_stream = stream; };

	enum BuildType {
		BUILD_ALL,
		BUILD_SLOW,
		BUILD_FAST,
		ANALYZE_FAST,
		ANALYZE_SLOW,
		ARCHIVE,
		CONVERT,
		CONVERT_S,
		CONVERT_SA,
		CONVERT_F,
		CONVERT_FA,
		MERGE,
		PLOT
	};

private:

	int m_rmin, m_rmax;
	int m_ZT, m_AT, m_ZP, m_AP, m_ZE, m_AE, m_ZR, m_AR;
	double m_B, m_Theta, m_BKE;

	std::string m_binpath, m_rootpath;
	std::string m_mapfile, m_shiftfile;
	std::string m_cutList, m_plotfile;
	std::string m_binfilepath;
	std::string m_mergefile, m_mergeIndir;
	std::string m_scalerfile;

	double m_SlowWindow;
	double m_FastWindowIonCh;
	double m_FastWindowSABRE;

	int m_analysisType;

	std::ostream* m_stream;

	std::vector<TString> m_currentFiles;

	RunCollector grabber;

	bool CollectRuns(std::string& dir, std::string prefix, std::string suffix, int min, int max, bool ranged=true);
	bool GetRun(int runno);

	void BuildFullEvents();
	void BuildSlowEvents();
	void BuildFastEvents();
	void AnalyzeSlowEvents();
	void AnalyzeFastEvents();

};

#endif