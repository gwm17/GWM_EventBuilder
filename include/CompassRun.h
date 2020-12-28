/*
	CompassRun.h
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020
*/
#ifndef COMPASSRUN_H
#define COMPASSRUN_H

#include "CompassFile.h"
#include "DataStructs.h"
#include "RunCollector.h"
#include "NewShiftMap.h"
#include <TParameter.h>

class CompassRun {
public:
	CompassRun();
	CompassRun(std::string& dir);
	CompassRun(const char* dir);
	~CompassRun();
	inline void SetDirectory(std::string& dir) { directory = dir; };
	inline void SetDirectory(const char* dir) { directory = dir; };
	inline void SetScalerInput(std::string& filename) { m_scalerinput = filename; };
	inline void SetScalerInput(const char* filename) { m_scalerinput = filename; };
	inline void SetRunNumber(int n) { runNum = n; };
	inline void SetShiftMap(const char* filename) { m_smap.SetFile(filename); };
	inline void SetShiftMap(std::string& filename) { m_smap.SetFile(filename); };
	void Convert2RawRoot(std::string& name);
	void Convert2SortedRoot(std::string& name, std::string& mapfile, double window);
	void Convert2FastSortedRoot(std::string& name, std::string& mapfile, double window, double fsi_window, double fic_window);
	void Convert2SlowAnalyzedRoot(std::string& name, std::string& mapfile, double window,
							  int zt, int at, int zp, int ap, int ze, int ae, double bke, double b, double theta);
	void Convert2FastAnalyzedRoot(std::string& name, std::string& mapfile, double window, double fsi_window, double fic_window,
							  int zt, int at, int zp, int ap, int ze, int ae, double bke, double b, double theta);

private:
	bool GetBinaryFiles();
	bool GetHitsFromFiles();
	void SetScalers();
	void ReadScalerData();

	std::string directory, m_scalerinput;
	std::vector<CompassFile> m_datafiles, m_scalerfiles;
	unsigned int startIndex; //this is the file we start looking at; increases as we finish files.
	NewShiftMap m_smap;
	std::unordered_map<std::string, TParameter<Long64_t>> m_scaler_map; //maps scaler files to the TParameter to be saved

	//Potential branch variables
	CompassHit hit;
	CoincEvent event;
	ProcessedEvent pevent;

	//what run is this
	int runNum;
	unsigned int m_totalHits;

	//Scaler switch
	bool m_scaler_flag;
};

#endif