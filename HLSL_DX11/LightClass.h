#pragma once

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass&);
    ~LightClass();

    void setAmbientColor(float, float, float, float);
    void SetDiffuseColor(float, float, float, float);
    void SetDirection(float, float, float);

    XMFLOAT4 GetAmbientColor() const;
    XMFLOAT4 GetDiffuseColor() const;
    XMFLOAT3 GetDirection() const;

private:
    XMFLOAT4 m_ambientColor;
    XMFLOAT4 m_diffuseColor;
    XMFLOAT3 m_direction;
};