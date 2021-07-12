/*
	ShiftMap.h
	New class to act a go-between for timestamp shifts to channels. Takes in a
	formated file containing data for shifts and then stores them in an unordered_map.
	Key is a global compass channel (board#*16 + channel). Shifts in ps.

	Note: Timestamps are now shifted in binary conversion. This means that shifts *MUST*
	be stored as Long64_t types. No decimals!

	Written by G.W. McCann Oct. 2020
*/
#ifndef SHIFTMAP_H
#define SHIFTMAP_H

class ShiftMap {
public:
	ShiftMap();
	ShiftMap(const std::string& filename);
	~ShiftMap();
	void SetFile(const std::string& filename);
	inline bool IsSet() { return is_set; };
	inline std::string GetFilename() { return m_filename; };
	Long64_t GetShift(int gchan);

private:
	void ParseFile();

	std::string m_filename;
	bool is_set;

	std::unordered_map<int, Long64_t> m_map;

};

#endif
