#pragma once

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass&);
    ~LightClass();

    void SetAmbientColor(float, float, float, float);
    void SetDiffuseColor(float, float, float, float);
    void SetDirection(float, float, float);
    void SetSpecularcolor(float, float, float, float);
    void SetSpecularPower(float);

    XMFLOAT4 GetAmbientColor() const;
    XMFLOAT4 GetDiffuseColor() const;
    XMFLOAT3 GetDirection() const;
    XMFLOAT4 GetSpecularColor() const;
    float GetSpecularPower() const;

private:
    XMFLOAT4 m_ambientColor;
    XMFLOAT4 m_diffuseColor;
    XMFLOAT3 m_direction;
    XMFLOAT4 m_specularColor;
    float m_specularPower;
};