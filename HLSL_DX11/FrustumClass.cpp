#include "Stdafx.h"
#include "FrustumClass.h"

FrustumClass::FrustumClass()
= default;

FrustumClass::FrustumClass(const FrustumClass&): m_planes{}
{
}

FrustumClass::~FrustumClass()
= default;

// �������� ���� https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
void FrustumClass::ConstructFrustum(const float screenDepth, XMMATRIX projectionMatrix, const XMMATRIX viewMatrix)
{
    // ���� ��� XMFLOAT4X4 ��ȯ
    XMFLOAT4X4 pMatrix{};
    XMStoreFloat4x4(&pMatrix, projectionMatrix);

    // �������ҿ��� �ּ� Z �Ÿ��� ���
    const float zMinimum = -pMatrix._43 / pMatrix._33;
    const float r = screenDepth / (screenDepth - zMinimum);

    // ������Ʈ ���� ���� ��Ŀ� �ٽ� ����
    pMatrix._33 = r;
    pMatrix._43 = -r * zMinimum;
    projectionMatrix = XMLoadFloat4x4(&pMatrix);

    // �� ��İ� ���� ����� ���� �������� ��� ����
    const XMMATRIX finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

    XMFLOAT4X4 matrix{};
    XMStoreFloat4x4(&matrix, finalMatrix);

    // �������� �� ��� ���
    auto x = static_cast<float>(matrix._14 + matrix._13);
    auto y = static_cast<float>(matrix._24 + matrix._23);
    auto z = static_cast<float>(matrix._34 + matrix._33);
    auto w = static_cast<float>(matrix._44 + matrix._43);
    m_planes[0] = XMVectorSet(x, y, z, w);
    m_planes[0] = XMPlaneNormalize(m_planes[0]);

    // �������� �� ��� ���
    x = static_cast<float>(matrix._14 - matrix._13);
    y = static_cast<float>(matrix._24 - matrix._23);
    z = static_cast<float>(matrix._34 - matrix._33);
    w = static_cast<float>(matrix._44 - matrix._43);
    m_planes[1] = XMVectorSet(x, y, z, w);
    m_planes[1] = XMPlaneNormalize(m_planes[1]);

    // �������� ���� ��� ���
    x = static_cast<float>(matrix._14 + matrix._11);
    y = static_cast<float>(matrix._24 + matrix._21);
    z = static_cast<float>(matrix._34 + matrix._31);
    w = static_cast<float>(matrix._44 + matrix._41);
    m_planes[2] = XMVectorSet(x, y, z, w);
    m_planes[2] = XMPlaneNormalize(m_planes[2]);

    // �������� ������ ��� ���
    x = static_cast<float>(matrix._14 - matrix._11);
    y = static_cast<float>(matrix._24 - matrix._21);
    z = static_cast<float>(matrix._34 - matrix._31);
    w = static_cast<float>(matrix._44 - matrix._41);
    m_planes[3] = XMVectorSet(x, y, z, w);
    m_planes[3] = XMPlaneNormalize(m_planes[3]);

    // �������� �� ��� ���
    x = static_cast<float>(matrix._14 - matrix._12);
    y = static_cast<float>(matrix._24 - matrix._22);
    z = static_cast<float>(matrix._34 - matrix._32);
    w = static_cast<float>(matrix._44 - matrix._42);
    m_planes[4] = XMVectorSet(x, y, z, w);
    m_planes[4] = XMPlaneNormalize(m_planes[4]);

    // �������� �Ʒ� ��� ���
    x = static_cast<float>(matrix._14 + matrix._12);
    y = static_cast<float>(matrix._24 + matrix._22);
    z = static_cast<float>(matrix._34 + matrix._32);
    w = static_cast<float>(matrix._44 + matrix._42);
    m_planes[5] = XMVectorSet(x, y, z, w);
    m_planes[5] = XMPlaneNormalize(m_planes[5]);
}

bool FrustumClass::CheckPoint(const float x, const float y, const float z)
{
    // �� �������ҿ� ���� �ִ��� Ȯ��
    for (const auto& m_plane : m_planes)
    {
        if (XMVectorGetX(XMPlaneDotCoord(m_plane, XMVectorSet(x, y, z, 1.0f))) < 0.0f)
            return false;
    }

    return true;
}

bool FrustumClass::CheckCube(const float xCenter, const float yCenter, const float zCenter, const float radius)
{
    // �� �������ҿ� ť���� �� ���� �ִ��� Ȯ��
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
    // ���� �ݰ��� �� �������� �ȿ� �ִ��� Ȯ��
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
