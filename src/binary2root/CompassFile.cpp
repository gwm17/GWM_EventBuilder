/*
	CompassFile.cpp
	Wrapper class around a shared pointer to an ifstream. Here the shared pointer is used
	to overcome limitations of the ifstream class, namely that it is written such that ifstream
	cannot be modified by move semantics. Contains all information needed to parse a single binary
	CompassFile. Currently has a class wide defined buffer size; may want to make this user input
	in the future.

	Written by G.W. McCann Oct. 2020
*/
#include "EventBuilder.h"
#include "CompassFile.h"

CompassFile::CompassFile() :
	m_filename(""), hitBuffer(bufsize,0), bufferIter(nullptr), bufferEnd(nullptr), m_smap(nullptr), hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()), eofFlag(false)
{

}

CompassFile::CompassFile(std::string& filename) :
	m_filename(""), hitBuffer(bufsize,0), bufferIter(nullptr), bufferEnd(nullptr), m_smap(nullptr), hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()), eofFlag(false)
{
	Open(filename);
}

CompassFile::CompassFile(const char* filename) :
	m_filename(""), hitBuffer(bufsize,0), bufferIter(nullptr), bufferEnd(nullptr), m_smap(nullptr), hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()), eofFlag(false)
{
	Open(filename);
}

CompassFile::~CompassFile() {
	Close();
}

void CompassFile::Open(std::string& filename) {
	eofFlag = false;
	hitUsedFlag = true;
	m_filename = filename;
	m_file->open(m_filename, std::ios::binary | std::ios::in);
}

void CompassFile::Open(const char* filename) {
	eofFlag = false;
	hitUsedFlag = true;
	m_filename = filename;
	m_file->open(m_filename, std::ios::binary | std::ios::in);
}

void CompassFile::Close() {
	if(IsOpen()) {
		m_file->close();
	}
}

/*
	GetNextHit() is the function which... gets the next hit
	Has to check if the buffer needs refilled/filled for the first time
	Upon pulling a hit, sets the UsedFlag to false, letting the next level know
	that the hit should be free game.

	If the file cannot be opened, signals as though file is EOF
*/
bool CompassFile::GetNextHit() {
	if(!IsOpen()) return true;

	if(bufferIter == nullptr || bufferIter == bufferEnd) {
		GetNextBuffer();
	}

	if(!IsEOF()) {
		ParseNextHit();
		hitUsedFlag = false;
	}

	return eofFlag;
}

/*
	GetNextBuffer() ... self-explanatory name
	Note tht this is where the EOF flag is set. The EOF is only singaled
	after the LAST buffer is completely read (i.e literally no more data). ifstream sets its eof 
	bit upon pulling the last buffer, but this class waits until that entire
	last buffer is read to singal EOF (the true end of file). 
*/
void CompassFile::GetNextBuffer() {

	if(m_file->eof()) {
		eofFlag = true;
		return;
	}

	m_file->read(hitBuffer.data(), hitBuffer.size());

	bufferIter = hitBuffer.data();
	bufferEnd = bufferIter + m_file->gcount(); //one past the last datum

}

void CompassFile::ParseNextHit() {

	m_currentHit.board = *((UShort_t*)bufferIter);
	bufferIter += 2;
	m_currentHit.channel = *((UShort_t*)bufferIter);
	bufferIter += 2;
	m_currentHit.timestamp = *((ULong64_t*)bufferIter);
	bufferIter += 8;
	m_currentHit.lgate = *((UShort_t*)bufferIter);
	bufferIter += 2;
	m_currentHit.sgate = *((UShort_t*)bufferIter);
	bufferIter += 2;
	m_currentHit.flags = *((UInt_t*)bufferIter);
	bufferIter += 4;
	m_currentHit.Ns = *((UInt_t*)bufferIter);
	bufferIter += 4;

	if(m_smap != nullptr) { //memory safety
		int gchan = m_currentHit.channel + m_currentHit.board*16;
		m_currentHit.timestamp += m_smap->GetShift(gchan);
	}

}