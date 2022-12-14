#include "stdafx.h"
#include "cameraclass.h"


CameraClass::CameraClass()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);;
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
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
    XMFLOAT3 up, position, lookAt;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    XMVECTOR upVector = XMLoadFloat3(&up);

    position = m_position;

    const XMVECTOR positionVector = XMLoadFloat3(&position);

    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

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

void CameraClass::RenderReflection(float height)
{
    XMFLOAT3 up, position, lookAt;

    // 위 벡터 설정
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    
    const XMVECTOR upVector = XMLoadFloat3(&up);

    // 반사 카메라 위치 설정
    position.x = m_position.x;
    position.y = -m_position.y + (height * 2.0f);
    position.z = m_position.z;

    const XMVECTOR positionVector = XMLoadFloat3(&position);
    
    const float radians = m_rotation.y * RADIAN;

    lookAt.x = sinf(radians) + m_position.x;
    lookAt.y = position.y;
    lookAt.z = cosf(radians) + m_position.z;

    const XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

    m_reflectionViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

XMMATRIX CameraClass::GetReflectionViewMatrix() const
{
    return m_reflectionViewMatrix;
}
