#include "EventBuilder.h"
#include "Stopwatch.h"

Stopwatch::Stopwatch() {
	start_time = Clock::now();
	stop_time = start_time;
}

Stopwatch::~Stopwatch() {}

void Stopwatch::Start() {
	start_time = Clock::now();
}

void Stopwatch::Stop() {
	stop_time = Clock::now();
}

double Stopwatch::GetElapsedSeconds() {
	return std::chrono::duration_cast<std::chrono::duration<double>>(stop_time-start_time).count();
}

double Stopwatch::GetElapsedMilliseconds() {
	return std::chrono::duration_cast<std::chrono::duration<double>>(stop_time-start_time).count()*1000.0;
}