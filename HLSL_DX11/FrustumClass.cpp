#include "Stdafx.h"
#include "FrustumClass.h"

FrustumClass::FrustumClass()
= default;

FrustumClass::FrustumClass(const FrustumClass&): m_planes{}
{
}

FrustumClass::~FrustumClass()
= default;

// 프러스텀 이해 https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
void FrustumClass::ConstructFrustum(const float screenDepth, XMMATRIX projectionMatrix, const XMMATRIX viewMatrix)
{
    // 투영 행렬 XMFLOAT4X4 변환
    XMFLOAT4X4 pMatrix{};
    XMStoreFloat4x4(&pMatrix, projectionMatrix);

    // 프러스텀에서 최소 Z 거리를 계산
    const float zMinimum = -pMatrix._43 / pMatrix._33;
    const float r = screenDepth / (screenDepth - zMinimum);

    // 업데이트 값을 투영 행렬에 다시 설정
    pMatrix._33 = r;
    pMatrix._43 = -r * zMinimum;
    projectionMatrix = XMLoadFloat4x4(&pMatrix);

    // 뷰 행렬과 투영 행렬을 곱해 프러스텀 행렬 생성
    const XMMATRIX finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

    XMFLOAT4X4 matrix{};
    XMStoreFloat4x4(&matrix, finalMatrix);

    // 프러스텀 앞 평면 계산
    auto x = static_cast<float>(matrix._14 + matrix._13);
    auto y = static_cast<float>(matrix._24 + matrix._23);
    auto z = static_cast<float>(matrix._34 + matrix._33);
    auto w = static_cast<float>(matrix._44 + matrix._43);
    m_planes[0] = XMVectorSet(x, y, z, w);
    m_planes[0] = XMPlaneNormalize(m_planes[0]);

    // 프러스텀 뒤 평면 계산
    x = static_cast<float>(matrix._14 - matrix._13);
    y = static_cast<float>(matrix._24 - matrix._23);
    z = static_cast<float>(matrix._34 - matrix._33);
    w = static_cast<float>(matrix._44 - matrix._43);
    m_planes[1] = XMVectorSet(x, y, z, w);
    m_planes[1] = XMPlaneNormalize(m_planes[1]);

    // 프러스텀 왼쪽 평면 계산
    x = static_cast<float>(matrix._14 + matrix._11);
    y = static_cast<float>(matrix._24 + matrix._21);
    z = static_cast<float>(matrix._34 + matrix._31);
    w = static_cast<float>(matrix._44 + matrix._41);
    m_planes[2] = XMVectorSet(x, y, z, w);
    m_planes[2] = XMPlaneNormalize(m_planes[2]);

    // 프러스텀 오른쪽 평면 계산
    x = static_cast<float>(matrix._14 - matrix._11);
    y = static_cast<float>(matrix._24 - matrix._21);
    z = static_cast<float>(matrix._34 - matrix._31);
    w = static_cast<float>(matrix._44 - matrix._41);
    m_planes[3] = XMVectorSet(x, y, z, w);
    m_planes[3] = XMPlaneNormalize(m_planes[3]);

    // 프러스텀 위 평면 계산
    x = static_cast<float>(matrix._14 - matrix._12);
    y = static_cast<float>(matrix._24 - matrix._22);
    z = static_cast<float>(matrix._34 - matrix._32);
    w = static_cast<float>(matrix._44 - matrix._42);
    m_planes[4] = XMVectorSet(x, y, z, w);
    m_planes[4] = XMPlaneNormalize(m_planes[4]);

    // 프러스텀 아래 평면 계산
    x = static_cast<float>(matrix._14 + matrix._12);
    y = static_cast<float>(matrix._24 + matrix._22);
    z = static_cast<float>(matrix._34 + matrix._32);
    w = static_cast<float>(matrix._44 + matrix._42);
    m_planes[5] = XMVectorSet(x, y, z, w);
    m_planes[5] = XMPlaneNormalize(m_planes[5]);
}

bool FrustumClass::CheckPoint(const float x, const float y, const float z)
{
    // 뷰 프러스텀에 점이 있는지 확인
    for (const auto& m_plane : m_planes)
    {
        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet(x, y, z, 1.0f))) < 0.0f)
            return false;
    }

    return true;
}

bool FrustumClass::CheckCube(const float xCenter, const float yCenter, const float zCenter, const float radius)
{
    // 뷰 프러스텀에 큐브의 한 점이 있는지 확인
    for (const auto& m_plane : m_planes)
    {
        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
            continue;

        return false;
    }

    return true;
}

bool FrustumClass::CheckSphere(const float xCenter, const float yCenter, const float zCenter, const float radius)
{
    // 구의 반경이 뷰 프러스텀 안에 있는지 확인
    for (const auto& m_plane : m_planes)
    {
        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet(xCenter, yCenter, zCenter, 1.0f))) < -radius)
            return false;
    }

    return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
    for (const auto& m_plane : m_planes)
    {
        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
            continue;

        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
            continue;

        return false;
    }

    return true;
}
