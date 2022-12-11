#pragma once

class PositionClass
{
public:
    PositionClass();
    PositionClass(const PositionClass&);
    ~PositionClass();

    void SetFrameTime(float);
    void GetRotationY(float&);
    void GetRotationX(float&);

    void TurnLeft(bool);
    void TurnRight(bool);
    void TurnUp(bool);
    void TurnDown(bool);

private:
    float m_frameTime = 0.0f;
    float m_rotationX = 0.0f;
    float m_rotationY = 0.0f;
    float m_leftTurnSpeed = 0.0f;
    float m_rightTurnSpeed = 0.0f;
    float m_upTurnSpeed = 0.0f;
    float m_downTurnSpeed = 0.0f;
};