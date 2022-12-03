#include "Stdafx.h"
#include <random>
#include <functional>
#include <time.h>
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
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    m_modelCount = numModels;

    m_modelInfoList = new ModelInfoType[m_modelCount];
    if (!m_modelInfoList) return false;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, DIVISION);
    auto generator = std::bind(dis, gen);
    
    for (int i = 0; i < m_modelCount; ++i)
    {
        red = static_cast<float>(generator()) / static_cast<float>(DIVISION);
        green = static_cast<float>(generator()) / static_cast<float>(DIVISION);
        blue = static_cast<float>(generator()) / static_cast<float>(DIVISION);
        // printf("%f %f %f\n", red, green, blue);
    
        m_modelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);
    
        m_modelInfoList[i].positionX = ((static_cast<float>(generator()) - static_cast<float>(generator())) / static_cast<float>(DIVISION)) *
            10.0f;
        m_modelInfoList[i].positionY = ((static_cast<float>(generator()) - static_cast<float>(generator())) / static_cast<float>(DIVISION)) *
            10.0f;
        m_modelInfoList[i].positionZ = (((static_cast<float>(generator()) - static_cast<float>(generator())) / static_cast<float>(DIVISION))
            * 10.0f) + 5.0f;
    }

    // srand(static_cast<unsigned int>(time(nullptr)));
    //
    // for(int i = 0; i < m_modelCount; ++i)
    // {
    //     red = static_cast<float>(rand()) / RAND_MAX;
    //     green = static_cast<float>(rand()) / RAND_MAX;
    //     blue = static_cast<float>(rand()) / RAND_MAX;
    //
    //     m_modelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);
    //
    //     m_modelInfoList[i].positionX = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 10.0f;
    //     m_modelInfoList[i].positionY = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 10.0f;
    //     m_modelInfoList[i].positionZ = (((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 10.0f) + 5.0f;
    // }

    return true;
}

void ModelListClass::Shutdown()
{
    if (m_modelInfoList)
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
