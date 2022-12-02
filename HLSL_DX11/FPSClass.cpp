#include "Stdafx.h"
#include <mmsystem.h>
#include "FPSClass.h"

FpsClass::FpsClass()
= default;

FpsClass::FpsClass(const FpsClass&)
{
    
}

FpsClass::~FpsClass()
= default;

void FpsClass::Initialize()
{
    m_fps = 0;
    m_count = 0;
    m_startTime = timeGetTime();
}

void FpsClass::Frame()
{
    ++m_count;

    if(timeGetTime() >= (m_startTime + 1000))
    {
        m_fps = m_count;
        m_count = 0;

        m_startTime = timeGetTime();
    }
}

int FpsClass::GetFps() const
{
    return m_fps;
}
