#include "Stdafx.h"
#include "PositionClass.h"

PositionClass::PositionClass()
= default;

PositionClass::PositionClass(const PositionClass&)
{
}

PositionClass::~PositionClass()
= default;

void PositionClass::SetFrameTime(const float time)
{
    m_frameTime = time;
}

void PositionClass::GetRotationX(float& x)
{
    x = m_rotationX;
}

void PositionClass::GetRotationY(float& y)
{
    y = m_rotationY;
}

void PositionClass::TurnLeft(const bool keydown)
{
    if (keydown)
    {
        m_leftTurnSpeed += m_frameTime * 0.01f;
        if (m_leftTurnSpeed > (m_frameTime * 0.15f))
        {
            m_leftTurnSpeed = m_frameTime * 0.15f;
        }
    }
    else
    {
        m_leftTurnSpeed -= m_frameTime * 0.005f;
        if (m_leftTurnSpeed < 0.0f)
        {
            m_leftTurnSpeed = 0.0f;
        }
    }

    m_rotationY -= m_leftTurnSpeed;
    if (m_rotationY < 0.0f)
        m_rotationY += 360.0f;
}

void PositionClass::TurnRight(const bool keydown)
{
    if (keydown)
    {
        m_rightTurnSpeed += m_frameTime * 0.01f;
        if (m_rightTurnSpeed > (m_frameTime * 0.15f))
        {
            m_rightTurnSpeed = m_frameTime * 0.15f;
        }
    }
    else
    {
        m_rightTurnSpeed -= m_frameTime * 0.005f;
        if (m_rightTurnSpeed < 0.0f)
        {
            m_rightTurnSpeed = 0.0f;
        }
    }
    m_rotationY += m_rightTurnSpeed;
    if (m_rotationY > 360.0f)
        m_rotationY -= 360.0f;
}

void PositionClass::TurnUp(const bool keydown)
{
    if (keydown)
    {
        m_upTurnSpeed += m_frameTime * 0.01f;
        if (m_upTurnSpeed > (m_frameTime * 0.15f))
        {
            m_upTurnSpeed = m_frameTime * 0.15f;
        }
    }
    else
    {
        m_upTurnSpeed -= m_frameTime * 0.005f;
        if (m_upTurnSpeed < 0.0f)
        {
            m_upTurnSpeed = 0.0f;
        }
    }
    m_rotationX -= m_upTurnSpeed;
    if (m_rotationX < 0)
        m_rotationX += 360.0f;
}

void PositionClass::TurnDown(const bool keydown)
{
    if (keydown)
    {
        m_downTurnSpeed += m_frameTime * 0.01f;
        if (m_downTurnSpeed > (m_frameTime * 0.15f))
        {
            m_downTurnSpeed = m_frameTime * 0.15f;
        }
    }
    else
    {
        m_downTurnSpeed -= m_frameTime * 0.005f;
        if (m_downTurnSpeed < 0.0f)
        {
            m_downTurnSpeed = 0.0f;
        }
    }

    m_rotationX += m_downTurnSpeed;
    if (m_rotationX > 360.0f)
        m_rotationX -= 360.0f;
}
