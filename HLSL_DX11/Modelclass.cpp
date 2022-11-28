#include "Stdafx.h"
#include "TextureClass.h"
#include "Modelclass.h"

ModelClass::ModelClass()
= default;

ModelClass::ModelClass(const ModelClass&)
{
    
}

ModelClass::~ModelClass()
= default;

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
    if(!InitializeBuffers(device)) return false;

    return LoadTexture(device, deviceContext, textureFilename);
}


void ModelClass::Shutdown()
{
    ReleaseTexture();
    
    ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext) const
{
    RenderBuffers(deviceContext);
}


int ModelClass::GetIndexCount() const
{
    return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture() const
{
    return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    m_vertexCount = 4;
    m_indexCount = 6;

    auto vertices = new VertexType[m_vertexCount];
    if(!vertices) return false;

    auto indices = new unsigned long[m_indexCount];
    if(!indices) return false;

    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

    vertices[1].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
    vertices[2].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
    vertices[3].texture = XMFLOAT2(1.0f, 0.0f);

    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;
    indices[3] = 1;
    indices[4] = 2;
    indices[5] = 3;

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result)) return false;

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
    if(FAILED(result)) return false;

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void ModelClass::ShutdownBuffers()
{
    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    
    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) const
{
    constexpr UINT stride = sizeof(VertexType);
    constexpr UINT offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
    m_Texture = new TextureClass;
    if(!m_Texture) return false;

    return m_Texture->Initialize(device, deviceContext, filename);
}

void ModelClass::ReleaseTexture()
{
    if(m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = nullptr;
    }
}
