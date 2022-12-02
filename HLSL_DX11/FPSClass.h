#pragma once

#pragma comment(lib, "winmm.lib")

class FpsClass
{
public:
    FpsClass();
    FpsClass(const FpsClass&);
    ~FpsClass();

    void Initialize();
    void Frame();
    int GetFps() const;

private:
    int m_fps, m_count;
    unsigned long m_startTime;
};