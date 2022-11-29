#include "Stdafx.h"
#include "LightClass.h"

LightClass::LightClass()
{
    
}

LightClass::LightClass(const LightClass&)
{
    
}

LightClass::~LightClass()
= default;

void LightClass::SetAmbientColor(const float red, const float green, const float blue, const float alpha)
{
    m_ambientColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDiffuseColor(const float red, const float green, const float blue, const float alpha)
{
    m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDirection(const float pitch, const float yaw, const float roll)
{
    m_direction = XMFLOAT3(pitch, yaw, roll);
}

void LightClass::SetSpecularcolor(const float red, const float green, const float blue, const float alpha)
{
    m_specularColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetSpecularPower(const float power)
{
    m_specularPower = power;
}

XMFLOAT4 LightClass::GetAmbientColor() const
{
    return m_ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor() const
{
    return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection() const
{
    return m_direction;
}

XMFLOAT4 LightClass::GetSpecularColor() const
{
    return m_specularColor;
}

float LightClass::GetSpecularPower() const
{
    return m_specularPower;
}
