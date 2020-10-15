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
#include "RunCollector.h"
#include "NewShiftMap.h"

class CompassRun {
public:
	CompassRun();
	CompassRun(std::string& dir);
	CompassRun(const char* dir);
	~CompassRun();
	inline void SetDirectory(std::string& dir) { directory = dir; };
	inline void SetDirectory(const char* dir) { directory = dir; };
	inline void SetShiftMap(const char* filename) { m_smap.SetFile(filename); };
	inline void SetShiftMap(std::string& filename) { m_smap.SetFile(filename); };
	void Convert(std::string& name); //Main method


private:
	bool GetBinaryFiles();
	bool GetHitsFromFiles();

	std::string directory;
	std::vector<CompassFile> m_files;
	unsigned int startIndex; //this is the file we start looking at; increases as we finish files.
	NewShiftMap m_smap;
	CompassHit hit;
};

#endif