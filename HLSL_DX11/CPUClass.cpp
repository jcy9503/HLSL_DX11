#include "Stdafx.h"
#include "CPUClass.h"

CpuClass::CpuClass()
= default;

CpuClass::CpuClass(const CpuClass&)
{
}

CpuClass::~CpuClass()
= default;

void CpuClass::Initialize()
{
    // CPU ����� Ǯ���ϴ� ���� ��ü ����
    PDH_STATUS status = PdhOpenQuery(nullptr, 0, &m_queryHandle);
    if (status != ERROR_SUCCESS)
        m_canReadCpu = false;

    // �ý����� ��� CPU�� Ǯ���ϵ��� ���� ��ü ����
    status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
    if (status != ERROR_SUCCESS)
        m_canReadCpu = false;

    m_lastSampleTime = GetTickCount();

    m_cpuUsage = 0;
}

void CpuClass::Shutdown() const
{
    if (m_canReadCpu)
        PdhCloseQuery(m_queryHandle);
}

void CpuClass::Frame()
{
    PDH_FMT_COUNTERVALUE value;

    if (m_canReadCpu)
    {
        if ((m_lastSampleTime + 1000) < GetTickCount())
        {
            m_lastSampleTime = GetTickCount();

            PdhCollectQueryData(m_queryHandle);

            PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, nullptr, &value);

            m_cpuUsage = value.longValue;
        }
    }
}

int CpuClass::GetCpuPercentage() const
{
    int usage = 0;

    if (m_canReadCpu)
        usage = static_cast<int>(m_cpuUsage);

    return usage;
}
