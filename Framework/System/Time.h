#pragma once
#include "Subsystem.h"

class Time final : public Subsystem<Time>
{
public:
	Time(class Context* context);
	~Time();

	Time(const Time&) = delete;
	Time& operator=(const Time&) = delete;

	void Update();

	inline const float Elapsed() const { return m_elapsed; }
	inline void LockFramerate(const UINT framerate) { m_frametime = 1.0f / framerate; }
	
private:
	UINT64 m_frequency;
	UINT64 m_previous;
	float m_elapsed;

	float m_frametime;
};