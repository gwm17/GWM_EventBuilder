/*
	ShiftMap.h
	New class to act a go-between for timestamp shifts to channels. Takes in a
	formated file containing data for shifts and then stores them in an unordered_map.
	Key is a global compass channel (board#*16 + channel). Shifts in ps.

	Note: Timestamps are now shifted in binary conversion. This means that shifts *MUST*
	be stored as Long64_t types. No decimals!

	Written by G.W. McCann Oct. 2020
*/
#include "EventBuilder.h"
#include "ShiftMap.h"

ShiftMap::ShiftMap() :
	m_filename(""), is_set(false)
{
}

ShiftMap::ShiftMap(const std::string& filename) :
	m_filename(filename), is_set(false)
{
	ParseFile();
}

ShiftMap::~ShiftMap() {}

void ShiftMap::SetFile(const std::string& filename) {
	m_filename = filename;
	ParseFile();
}

Long64_t ShiftMap::GetShift(int gchan) {
	if(!is_set) return 0.0;
	
	auto iter = m_map.find(gchan);
	if(iter == m_map.end()) {
		return 0.0;
	} else return iter->second;
}

void ShiftMap::ParseFile() {
	std::ifstream input(m_filename);
	if(!input.is_open()) return;

	int board, channel, gchan;
	Long64_t shift;
	std::string junk, temp;

	std::getline(input, junk);
	std::getline(input, junk);

	while(input>>board) {
		input>>temp;
		input>>shift;
		if(temp == "all") { //keyword to set all channels in this board to same shift
			for(int i=0; i<16; i++) {
				gchan = board*16 + i;
				m_map[gchan] = shift;
			}
		} else {
			channel = stoi(temp);
			gchan = channel + board*16;
			m_map[gchan] = shift;
		}
	}

	is_set = true;
}
