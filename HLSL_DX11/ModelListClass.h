#pragma once

class ModelListClass
{
private:
    struct ModelInfoType
    {
        XMFLOAT4 color;
        float positionX, positionY, positionZ;
    };

public:
    ModelListClass();
    ModelListClass(const ModelListClass&);
    ~ModelListClass();

    bool Initialize(int);
    void Shutdown();

    int GetModelCount() const;
    void GetData(int, float&, float&, float&, XMFLOAT4&) const;

private:
    int m_modelCount = 0;
    ModelInfoType* m_modelInfoList = nullptr;
};