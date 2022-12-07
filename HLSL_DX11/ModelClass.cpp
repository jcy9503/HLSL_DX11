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
    // 모델 데이터를 로드
    if (!LoadModel(modelFilename))
    {
        return false;
    }

    // 모델의 법선, 접선 및 이항 벡터를 계산
    CalculateModelVectors();

    // 정점 및 인덱스 버퍼 초기화
    if (!InitializeBuffers(device))
    {
        return false;
    }

    // 텍스처 로드
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
    // 정점 배열 생성
    auto vertices = new VertexType[m_vertexCount];
    if (!vertices) return false;

    // 인덱스 배열 생성
    auto indices = new unsigned long[m_indexCount];
    if (!indices) return false;

    // 정점 배열과 인덱스 배열에 데이터 로드
    for (int i = 0; i < m_vertexCount; i++)
    {
        vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
        vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
        vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
        vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
        vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);

        indices[i] = i;
    }

    // 정적 정점 버퍼의 구조체 설정
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // subresource 구조에 정점 데이터에 대한 포인터 설정
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 정점 버퍼 생성
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
    {
        return false;
    }

    // 정적 인덱스 버퍼의 구조체 설정
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // 인덱스 데이터를 가리키는 보조 리소스 구조체 작성
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼 생성
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
    // 정점 버퍼의 단위와 오프셋 설정
    constexpr UINT stride = sizeof(VertexType);
    constexpr UINT offset = 0;

    // 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 정점 버퍼로 그릴 기본형을 삼각형으로 설정
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1)
{
    // 텍스처 배열 오브젝트 생성
    m_TextureArray = new TextureArrayClass;
    if (!m_TextureArray)
    {
        return false;
    }

    // 텍스처 배열 오브젝트 초기화
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

    // 버텍스 카운트의 값까지 읽는다.
    char input = 0;
    fin.get(input);
    while (input != ':') fin.get(input);

    // 버텍스 카운트를 읽는다.
    fin >> m_vertexCount;

    // 인덱스의 수를 정점 수와 같게 설정
    m_indexCount = m_vertexCount;

    // 읽어 들인 정점 개수를 사용하여 모델 생성
    m_model = new ModelType[m_vertexCount];
    if (!m_model) return false;

    // 데이터의 시작 부분까지 읽는다.
    fin.get(input);
    while (input != ':') fin.get(input);
    fin.get(input);
    fin.get(input);

    // 버텍스 데이터를 읽는다.
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
    
    // 모델의 면 수 계산
    const int faceCount = m_vertexCount / 3;
    
    int index = 0;

    // 모든면을 살펴보고 접선, 비공식 및 법선 벡터를 계산
    for (int i = 0; i < faceCount; i++)
    {
        TempVertexType vertex3{};
        TempVertexType vertex2{};
        TempVertexType vertex1{};
        // 해당 면에 대한 세 개의 정점을 가져옵니다.
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

        // 표면의 탄젠트와 바이 노멀을 계산
        CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

        // 접선과 binormal을 사용하여 새 법선을 계산
        CalculateNormal(tangent, binormal, normal);

        // 모델 구조에서 면의 법선, 접선 및 바이 노멀을 저장
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


    // 현재 표면의 두 벡터를 계산
    vector1[0] = vertex2.x - vertex1.x;
    vector1[1] = vertex2.y - vertex1.y;
    vector1[2] = vertex2.z - vertex1.z;

    vector2[0] = vertex3.x - vertex1.x;
    vector2[1] = vertex3.y - vertex1.y;
    vector2[2] = vertex3.z - vertex1.z;

    // tu 및 tv 텍스처 공간 벡터를 계산
    tuVector[0] = vertex2.tu - vertex1.tu;
    tvVector[0] = vertex2.tv - vertex1.tv;

    tuVector[1] = vertex3.tu - vertex1.tu;
    tvVector[1] = vertex3.tv - vertex1.tv;

    // 탄젠트 / 바이 노멀 방정식의 분모를 계산 - 역행렬 상수
    const float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

    // 교차 곱을 계산하고 계수로 곱하여 접선과 비 구식을 얻음.
    tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
    tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
    tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

    binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
    binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
    binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

    // 이 법선의 길이 계산
    float length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    // 법선을 표준화 한 다음 저장
    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    // 이 법선의 길이 계산
    length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

    // 법선을 표준화 한 다음 저장
    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;
}


void ModelClass::CalculateNormal(const VectorType tangent, const VectorType binormal, VectorType& normal) const
{
    // 법선 벡터를 줄 수있는 접선과 binormal의 외적을 계산
    normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
    normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
    normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

    // 법선의 길이를 계산
    const float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

    // 법선을 표준화
    normal.x = normal.x / length;
    normal.y = normal.y / length;
    normal.z = normal.z / length;
}
