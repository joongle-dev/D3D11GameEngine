#include "Framework.h"
#include "Time.h"


Time::Time(Context* context) :
	Subsystem<Time>(context), 
	m_elapsed(0.0f), m_frametime(0.0001f)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_previous);
}

Time::~Time() {}

void Time::Update()
{
	UINT64 current;
	QueryPerformanceCounter((LARGE_INTEGER*)&current);

	//Limit framerate
	float worktime = (current - m_previous) / (float)m_frequency;
	float sleeptime = std::max(m_frametime - worktime, 0.0f);
	Sleep(sleeptime * 1000.0f);

	m_elapsed = worktime + sleeptime;

	QueryPerformanceCounter((LARGE_INTEGER*)&m_previous);
}