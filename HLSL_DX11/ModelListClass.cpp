#include "Stdafx.h"
#include <random>
#include "ModelListClass.h"

ModelListClass::ModelListClass()
= default;

ModelListClass::ModelListClass(const ModelListClass&)
{
    
}

ModelListClass::~ModelListClass()
= default;

bool ModelListClass::Initialize(const int numModels)
{
    float red = 0.0f; float green = 0.0f; float blue = 0.0f;

    m_modelCount = numModels;

    m_modelInfoList = new ModelInfoType[m_modelCount];
    if(!m_modelInfoList) return false;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, INT_MAX);

    for(int i = 0; i < m_modelCount; ++i)
    {
        red = dis(gen) / INT_MAX;
        green = dis(gen) / INT_MAX;
        blue = dis(gen) / INT_MAX;

        m_modelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

        m_modelInfoList[i].positionX = ((dis(gen) - dis(gen)) / INT_MAX) * 10.0f;
        m_modelInfoList[i].positionY = ((dis(gen) - dis(gen)) / INT_MAX) * 10.0f;
        m_modelInfoList[i].positionZ = (((dis(gen) - dis(gen)) / INT_MAX) * 10.0f) + 5.0f;
    }

    return true;
}

void ModelListClass::Shutdown()
{
    if(m_modelInfoList)
    {
        delete[] m_modelInfoList;
        m_modelInfoList = nullptr;
    }
}

int ModelListClass::GetModelCount() const
{
    return m_modelCount;
}

void ModelListClass::GetData(const int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color) const
{
    positionX = m_modelInfoList[index].positionX;
    positionY = m_modelInfoList[index].positionY;
    positionZ = m_modelInfoList[index].positionZ;

    color = m_modelInfoList[index].color;
}
