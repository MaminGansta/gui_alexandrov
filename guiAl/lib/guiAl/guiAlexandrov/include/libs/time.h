#pragma once

#include <chrono>
#include <thread>


namespace gui
{

	using namespace std::chrono;

	extern high_resolution_clock::time_point __init_time;

	float get_time();


	struct Timer
	{
		high_resolution_clock::time_point privius;

		int FPS = 0;
		float FrameTimeAVG = 0.0f;

		float m_timeSum = 0.0f;
		int m_Frames = 0;

		bool m_frameLock = false;
		float m_frameLock_time = 0.0f;

		float elapsed = 0.0f;


		// Set max frames per second
		Timer(int frames = 0);

		// put zero for unlock
		void set_frame_lock(int frames);

		void update();
	};

}