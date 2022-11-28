#include "Stdafx.h"
#include "LightClass.h"

LightClass::LightClass()
{
    
}

LightClass::LightClass(const LightClass&)
{
    
}

LightClass::~LightClass()
{
    
}

void LightClass::SetDiffuseColor(const float red, const float green, const float blue, const float alpha)
{
    m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDirection(const float pitch, const float yaw, const float roll)
{
    m_direction = XMFLOAT3(pitch, yaw, roll);
}

XMFLOAT4 LightClass::GetDiffuseColor() const
{
    return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection() const
{
    return m_direction;
}
