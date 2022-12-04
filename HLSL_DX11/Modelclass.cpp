#include "Stdafx.h"
#include "TextureArrayClass.h"
#include "Modelclass.h"

#include <fstream>

ModelClass::ModelClass()
= default;

ModelClass::ModelClass(const ModelClass& other)
{
    
}

ModelClass::~ModelClass()
= default;

bool ModelClass::Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2, WCHAR* textureFilename3)
{
    if(!LoadModel(modelFilename)) return false;
    if(!InitializeBuffers(device)) return false;

    return LoadTextures(device, textureFilename1, textureFilename2, textureFilename3);
}

void ModelClass::Shutdown()
{
    ReleaseTexture();
    
    ShutdownBuffers();

    ReleaseModel();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext) const
{
    RenderBuffers(deviceContext);
}


int ModelClass::GetIndexCount() const
{
    return m_indexCount;
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray() const
{
    return m_textureArray->GetTextureArray();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    auto vertices = new VertexType[m_vertexCount];
    if(!vertices) return false;

    auto indices = new unsigned long[m_indexCount];
    if(!indices) return false;

    for(int i = 0; i < m_vertexCount; ++i)
    {
        vertices[i].position    = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
        vertices[i].texture     = XMFLOAT2(m_model[i].tu, m_model[i].tv);
        //vertices[i].normal      = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
        indices[i] = i;
    }

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

bool ModelClass::LoadTextures(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3)
{
    m_textureArray = new TextureArrayClass;
    if(!m_textureArray) return false;
    
    return m_textureArray->Initialize(device, filename1, filename2, filename3);
}

void ModelClass::ReleaseTexture()
{
    if(m_textureArray)
    {
        m_textureArray->Shutdown();
        delete m_textureArray;
        m_textureArray = nullptr;
    }
}

bool ModelClass::LoadModel(const char* filename)
{
    std::ifstream fin;
    fin.open(filename);

    if(fin.fail()) return false;

    char input = 0;
    fin.get(input);
    while(input != ':')
        fin.get(input);

    fin >> m_vertexCount;
    m_indexCount = m_vertexCount;

    m_model = new ModelType[m_vertexCount];
    if(!m_model) return false;

    fin.get(input);
    while(input != ':') fin.get(input);
    fin.get(input); fin.get(input);

    for(int i = 0; i < m_vertexCount; ++i)
    {
        fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
        fin >> m_model[i].tu >> m_model[i].tv;
        fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
    }

    fin.close();

    return true;
}

void ModelClass::ReleaseModel()
{
    if(m_model)
    {
        delete[] m_model;
        m_model = nullptr;
    }
}
