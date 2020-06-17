
#include "libs/time.h"


using namespace std::chrono;

// global
high_resolution_clock::time_point __init_time;


float get_time()
{
	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<float>	dur = duration_cast<duration<float>>(now - __init_time);
	return dur.count();
}


// Timer

Timer::Timer(int frames) : m_frameLock(frames > 0),
	m_frameLock_time(1.0f / frames)
{
	privius = high_resolution_clock::now();
}


void Timer::set_frame_lock(int frames)
{
	m_frameLock = frames > 0;
	m_frameLock_time = 1.0f / frames;
}



void Timer::update()
{
	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<float>	dur = duration_cast<duration<float>>(now - privius);
	privius = now;
	elapsed = dur.count();
	m_Frames++;

	if (m_frameLock && elapsed < m_frameLock_time)
	{
		float dif = m_frameLock_time - elapsed;
		elapsed = m_frameLock_time;
		std::this_thread::sleep_for(dur);
	}

	m_timeSum += elapsed;
	if (m_timeSum > 1.0f)
	{
		FPS = m_Frames;
		FrameTimeAVG = m_timeSum / FPS;

		m_timeSum = 0;
		m_Frames = 0;
	}
}
