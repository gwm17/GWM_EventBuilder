/*
	NewShiftMap.h
	New class to act a go-between for timestamp shifts to channels. Takes in a
	formated file containing data for shifts and then stores them in an unordered_map.
	Key is a global compass channel (board#*16 + channel). Shifts in ps.

	Note: Timestamps are now shifted in binary conversion. This means that shifts *MUST*
	be stored as Long64_t types. No decimals!

	Written by G.W. McCann Oct. 2020
*/
#ifndef NEWSHIFTMAP_H
#define NEWSHIFTMAP_H

class NewShiftMap {
public:
	NewShiftMap();
	NewShiftMap(std::string& filename);
	NewShiftMap(const char* filename);
	~NewShiftMap();
	void SetFile(std::string& filename);
	void SetFile(const char* filename);
	inline bool IsSet() { return is_set; };
	inline const char* GetFilename() { return m_filename.c_str(); };
	Long64_t GetShift(int gchan);

private:
	void ParseFile();

	std::string m_filename;
	bool is_set;

	std::unordered_map<int, Long64_t> m_map;

};

#endif