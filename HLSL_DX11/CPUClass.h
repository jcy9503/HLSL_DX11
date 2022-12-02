#pragma once

#pragma comment(lib, "pdh.lib")
#include <Pdh.h>

class CpuClass
{
public:
    CpuClass();
    CpuClass(const CpuClass&);
    ~CpuClass();

    void Initialize();
    void Shutdown() const;
    void Frame();
    int GetCpuPercentage() const;

private:
    bool m_canReadCpu = true;
    HQUERY m_queryHandle;
    HCOUNTER m_counterHandle;
    unsigned long m_lastSampleTime = 0;
    long m_cpuUsage = 0;
};
