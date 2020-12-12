#pragma once
#include <chrono>

namespace RT
{
	class Timer
	{
	public:
		Timer();
		/**
		 * \brief get the elapsed time since either creation or last reset time
		 * \return time in seconds since last frame
		 */
		float GetElapsedTime();
		/**
		 * \brief get the elapsed time since either creation or last reset time
		 * \return time in ms since last frame
		 */
		float GetElapsedTimeInMS();
		/**
		 * \brief Reset the Timer beginning point
		 */
		void ResetTimer();
	private:

		std::chrono::high_resolution_clock::time_point m_begin;
	};
}

