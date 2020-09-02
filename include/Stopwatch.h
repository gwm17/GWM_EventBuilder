#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

class Stopwatch {

public:
	Stopwatch();
	~Stopwatch();
	void Start();
	void Stop();
	double GetElapsedSeconds();
	double GetElapsedMilliseconds();

private:
	using Time = std::chrono::high_resolution_clock::time_point;
	using Clock = std::chrono::high_resolution_clock;

	Time start_time, stop_time;
};

#endif