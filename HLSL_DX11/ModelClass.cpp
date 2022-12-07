#include "stdafx.h"
#include "TextureArrayClass.h"
#include "ModelClass.h"

#include <fstream>
using namespace std;


ModelClass::ModelClass()
= default;


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
= default;


bool ModelClass::Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename1)
{
    // �� �����͸� �ε�
    if (!LoadModel(modelFilename))
    {
        return false;
    }

    // ���� ����, ���� �� ���� ���͸� ���
    CalculateModelVectors();

    // ���� �� �ε��� ���� �ʱ�ȭ
    if (!InitializeBuffers(device))
    {
        return false;
    }

    // �ؽ�ó �ε�
    return LoadTextures(device, textureFilename1);
}


void ModelClass::Shutdown()
{
    ReleaseTextures();
    
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
    return m_TextureArray->GetTextureArray();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    // ���� �迭 ����
    auto vertices = new VertexType[m_vertexCount];
    if (!vertices) return false;

    // �ε��� �迭 ����
    auto indices = new unsigned long[m_indexCount];
    if (!indices) return false;

    // ���� �迭�� �ε��� �迭�� ������ �ε�
    for (int i = 0; i < m_vertexCount; i++)
    {
        vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
        vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
        vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
        vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
        vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);

        indices[i] = i;
    }

    // ���� ���� ������ ����ü ����
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // subresource ������ ���� �����Ϳ� ���� ������ ����
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // ���� ���� ����
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
    {
        return false;
    }

    // ���� �ε��� ������ ����ü ����
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü �ۼ�
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� ���� ����
    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
    {
        return false;
    }
    
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}


void ModelClass::ShutdownBuffers()
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


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) const
{
    // ���� ������ ������ ������ ����
    constexpr UINT stride = sizeof(VertexType);
    constexpr UINT offset = 0;

    // ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ ����
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ ����
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // ���� ���۷� �׸� �⺻���� �ﰢ������ ����
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1)
{
    // �ؽ�ó �迭 ������Ʈ ����
    m_TextureArray = new TextureArrayClass;
    if (!m_TextureArray)
    {
        return false;
    }

    // �ؽ�ó �迭 ������Ʈ �ʱ�ȭ
    return m_TextureArray->Initialize(device, filename1);
}


void ModelClass::ReleaseTextures()
{
    if (m_TextureArray)
    {
        m_TextureArray->Shutdown();
        delete m_TextureArray;
        m_TextureArray = nullptr;
    }
}

bool ModelClass::LoadModel(const char* filename)
{
    ifstream fin;
    fin.open(filename);
    
    if (fin.fail()) return false;

    // ���ؽ� ī��Ʈ�� ������ �д´�.
    char input = 0;
    fin.get(input);
    while (input != ':') fin.get(input);

    // ���ؽ� ī��Ʈ�� �д´�.
    fin >> m_vertexCount;

    // �ε����� ���� ���� ���� ���� ����
    m_indexCount = m_vertexCount;

    // �о� ���� ���� ������ ����Ͽ� �� ����
    m_model = new ModelType[m_vertexCount];
    if (!m_model) return false;

    // �������� ���� �κб��� �д´�.
    fin.get(input);
    while (input != ':') fin.get(input);
    fin.get(input);
    fin.get(input);

    // ���ؽ� �����͸� �д´�.
    for (int i = 0; i < m_vertexCount; i++)
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
    if (m_model)
    {
        delete[] m_model;
        m_model = nullptr;
    }
}


void ModelClass::CalculateModelVectors() const
{
    VectorType tangent{};
    VectorType binormal{};
    VectorType normal{};
    
    // ���� �� �� ���
    const int faceCount = m_vertexCount / 3;
    
    int index = 0;

    // ������ ���캸�� ����, ����� �� ���� ���͸� ���
    for (int i = 0; i < faceCount; i++)
    {
        TempVertexType vertex3{};
        TempVertexType vertex2{};
        TempVertexType vertex1{};
        // �ش� �鿡 ���� �� ���� ������ �����ɴϴ�.
        vertex1.x = m_model[index].x;
        vertex1.y = m_model[index].y;
        vertex1.z = m_model[index].z;
        vertex1.tu = m_model[index].tu;
        vertex1.tv = m_model[index].tv;
        vertex1.nx = m_model[index].nx;
        vertex1.ny = m_model[index].ny;
        vertex1.nz = m_model[index].nz;
        index++;

        vertex2.x = m_model[index].x;
        vertex2.y = m_model[index].y;
        vertex2.z = m_model[index].z;
        vertex2.tu = m_model[index].tu;
        vertex2.tv = m_model[index].tv;
        vertex2.nx = m_model[index].nx;
        vertex2.ny = m_model[index].ny;
        vertex2.nz = m_model[index].nz;
        index++;

        vertex3.x = m_model[index].x;
        vertex3.y = m_model[index].y;
        vertex3.z = m_model[index].z;
        vertex3.tu = m_model[index].tu;
        vertex3.tv = m_model[index].tv;
        vertex3.nx = m_model[index].nx;
        vertex3.ny = m_model[index].ny;
        vertex3.nz = m_model[index].nz;
        index++;

        // ǥ���� ź��Ʈ�� ���� ����� ���
        CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

        // ������ binormal�� ����Ͽ� �� ������ ���
        CalculateNormal(tangent, binormal, normal);

        // �� �������� ���� ����, ���� �� ���� ����� ����
        m_model[index - 1].nx = normal.x;
        m_model[index - 1].ny = normal.y;
        m_model[index - 1].nz = normal.z;
        m_model[index - 1].tx = tangent.x;
        m_model[index - 1].ty = tangent.y;
        m_model[index - 1].tz = tangent.z;
        m_model[index - 1].bx = binormal.x;
        m_model[index - 1].by = binormal.y;
        m_model[index - 1].bz = binormal.z;

        m_model[index - 2].nx = normal.x;
        m_model[index - 2].ny = normal.y;
        m_model[index - 2].nz = normal.z;
        m_model[index - 2].tx = tangent.x;
        m_model[index - 2].ty = tangent.y;
        m_model[index - 2].tz = tangent.z;
        m_model[index - 2].bx = binormal.x;
        m_model[index - 2].by = binormal.y;
        m_model[index - 2].bz = binormal.z;

        m_model[index - 3].nx = normal.x;
        m_model[index - 3].ny = normal.y;
        m_model[index - 3].nz = normal.z;
        m_model[index - 3].tx = tangent.x;
        m_model[index - 3].ty = tangent.y;
        m_model[index - 3].tz = tangent.z;
        m_model[index - 3].bx = binormal.x;
        m_model[index - 3].by = binormal.y;
        m_model[index - 3].bz = binormal.z;
    }
}


void ModelClass::CalculateTangentBinormal(const TempVertexType vertex1, const TempVertexType vertex2, const TempVertexType vertex3,
                                          VectorType& tangent, VectorType& binormal) const
{
    float vector1[3], vector2[3];
    float tuVector[2], tvVector[2];


    // ���� ǥ���� �� ���͸� ���
    vector1[0] = vertex2.x - vertex1.x;
    vector1[1] = vertex2.y - vertex1.y;
    vector1[2] = vertex2.z - vertex1.z;

    vector2[0] = vertex3.x - vertex1.x;
    vector2[1] = vertex3.y - vertex1.y;
    vector2[2] = vertex3.z - vertex1.z;

    // tu �� tv �ؽ�ó ���� ���͸� ���
    tuVector[0] = vertex2.tu - vertex1.tu;
    tvVector[0] = vertex2.tv - vertex1.tv;

    tuVector[1] = vertex3.tu - vertex1.tu;
    tvVector[1] = vertex3.tv - vertex1.tv;

    // ź��Ʈ / ���� ��� �������� �и� ��� - ����� ���
    const float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

    // ���� ���� ����ϰ� ����� ���Ͽ� ������ �� ������ ����.
    tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
    tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
    tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

    binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
    binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
    binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

    // �� ������ ���� ���
    float length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    // ������ ǥ��ȭ �� ���� ����
    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    // �� ������ ���� ���
    length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

    // ������ ǥ��ȭ �� ���� ����
    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;
}


void ModelClass::CalculateNormal(const VectorType tangent, const VectorType binormal, VectorType& normal) const
{
    // ���� ���͸� �� ���ִ� ������ binormal�� ������ ���
    normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
    normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
    normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

    // ������ ���̸� ���
    const float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

    // ������ ǥ��ȭ
    normal.x = normal.x / length;
    normal.y = normal.y / length;
    normal.z = normal.z / length;
}
