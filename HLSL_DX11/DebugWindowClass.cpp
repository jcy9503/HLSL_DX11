#include "Stdafx.h"
#include "DebugWindowClass.h"

DebugWindowClass::DebugWindowClass()
{
}

DebugWindowClass::DebugWindowClass(const DebugWindowClass&)
{
}

DebugWindowClass::~DebugWindowClass()
{
}

bool DebugWindowClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    m_bitmapWidth = bitmapWidth;
    m_bitmapHeight = bitmapHeight;
    m_previousPosX = -1;
    m_previousPosY = -1;

    return InitializeBuffers(device);
}

void DebugWindowClass::Shutdown()
{
    ShutdownBuffers();
}

bool DebugWindowClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
    if (!UpdateBuffers(deviceContext, positionX, positionY))
        return false;

    RenderBuffers(deviceContext);

    return true;
}

int DebugWindowClass::GetIndexCount()
{
    return m_indexCount;
}

bool DebugWindowClass::InitializeBuffers(ID3D11Device* device)
{
    m_vertexCount = 6;
    m_indexCount = m_vertexCount;

    auto vertices = new VertexType[m_vertexCount];
    if (!vertices) return false;

    auto indices = new unsigned long[m_indexCount];
    if (!indices) return false;

    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    for (int i = 0; i < m_indexCount; ++i)
        indices[i] = i;

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result)) return false;

    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result)) return false;

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void DebugWindowClass::ShutdownBuffers()
{
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
}

bool DebugWindowClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
    if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
        return true;

    m_previousPosX = positionX;
    m_previousPosY = positionY;

    const float left = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);
    const float right = left + static_cast<float>(m_bitmapWidth);
    const float top = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);
    const float bottom = top - static_cast<float>(m_bitmapHeight);

    auto vertices = new VertexType[m_vertexCount];
    if (!vertices) return false;

    vertices[0].position = XMFLOAT3(left, top, 0.0f);
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    vertices[3].position = XMFLOAT3(left, top, 0.0f);
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(right, top, 0.0f);
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    const auto verticesPtr = static_cast<VertexType*>(mappedResource.pData);

    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

    deviceContext->Unmap(m_vertexBuffer, 0);

    delete[] vertices;
    vertices = nullptr;

    return true;
}

void DebugWindowClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
