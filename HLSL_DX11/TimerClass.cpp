#include "Stdafx.h"
#include "TimerClass.h"

TimerClass::TimerClass()
= default;

TimerClass::TimerClass(const TimerClass&)
{
}

TimerClass::~TimerClass()
= default;

bool TimerClass::Initialize()
{
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));
    if (m_frequency == 0) return false;

    m_ticksPerMs = static_cast<float>(m_frequency / 1000);

    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTime));

    return true;
}

void TimerClass::Frame()
{
    INT64 currentTime = 0;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

    const auto timeDifference = static_cast<float>(currentTime - m_startTime);

    m_frameTime = timeDifference / m_ticksPerMs;
    m_startTime = currentTime;
}

float TimerClass::GetTime() const
{
    return m_frameTime;
}
