#pragma once

#include <chrono>
#include <thread>
#include <functional>

using namespace std::chrono;

extern high_resolution_clock::time_point init_time;

float get_time();


float Speed_test(std::function<void()> target);



struct Timer
{
	high_resolution_clock::time_point privius;

	int FPS = 0;
	float FrameTimeAVG = 0.0f;

	float m_timeSum = 0.0f;
	int m_Frames = 0;

	bool m_frameLock;
	float m_frameLock_time = 0.016777f;

	float elapsed = 0.0f;


	Timer(float frame_time_lock = 0.0f);

	void update();
};