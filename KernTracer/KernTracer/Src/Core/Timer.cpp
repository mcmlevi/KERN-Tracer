#include <pch.h>
#include "Core/Timer.h"



RT::Timer::Timer()
{
	ResetTimer();
}

void RT::Timer::ResetTimer()
{
	m_begin = std::chrono::high_resolution_clock::now();
}


using namespace std::chrono;
float RT::Timer::GetElapsedTime()
{
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end - m_begin);
	return static_cast<float>(time_span.count());
}

float RT::Timer::GetElapsedTimeInMS()
{
	return GetElapsedTime() * 1000.f;
}

