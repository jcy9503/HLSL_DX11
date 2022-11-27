#include "Stdafx.h"
#include "Cameraclass.h"

CameraClass::CameraClass()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CameraClass::CameraClass(const CameraClass&)
{
    
}

CameraClass::~CameraClass()
= default;

void CameraClass::SetPosition(float x, float y, float z)
{
    m_position.x = x; m_position.y = y; m_position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotation.x = x; m_rotation.y = y; m_rotation.z = z;
}

XMFLOAT3 CameraClass::GetPosition() const
{
    return m_position;
}

XMFLOAT3 CameraClass::GetRotation() const
{
    return m_rotation;
}

void CameraClass::Render()
{
    constexpr XMFLOAT3 up{0.0f, 1.0f, 0.0f};
    XMVECTOR upVector = XMLoadFloat3(&up);

    const XMFLOAT3 position = m_position;
    const XMVECTOR positionVector = XMLoadFloat3(&position);

    constexpr XMFLOAT3 lookAt{0.0f, 0.0f, 1.0f};
    XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

    const float pitch = m_rotation.x * RADIAN;
    const float yaw = m_rotation.y * RADIAN;
    const float roll = m_rotation.z * RADIAN;

    const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
    upVector = XMVector3TransformCoord(upVector, rotationMatrix);

    lookAtVector = XMVectorAdd(positionVector, lookAtVector);

    m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix) const
{
    viewMatrix = m_viewMatrix;
}
