
#include "time.h"

using namespace std::chrono;


float get_time()
{
	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<float>	dur = duration_cast<duration<float>>(now - init_time);
	return dur.count();
}


float Speed_test(std::function<void()> target)
{
	float total_time = 0;
	for (int i = 0; i < 10; i++)
	{
		float start = get_time();
		target();
		total_time += get_time() - start;
	}

	return total_time * 0.1f;
}



// Timer

Timer::Timer(float frame_time_lock) : m_frameLock(frame_time_lock > 0.0f),
											 m_frameLock_time(frame_time_lock)
{
	privius = high_resolution_clock::now();
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