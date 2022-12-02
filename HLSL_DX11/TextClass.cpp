#include "stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"


TextClass::TextClass()
= default;

TextClass::TextClass(const TextClass& other): m_baseViewMatrix(), m_sentence1(nullptr), m_sentence2(nullptr), m_key(nullptr)
{
}

TextClass::~TextClass()
= default;

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const HWND hwnd, const int screenWidth,
                           const int screenHeight, const XMMATRIX baseViewMatrix)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // 기본 뷰 행렬 저장
    m_baseViewMatrix = baseViewMatrix;

    // 폰트 객체 생성
    m_font = new FontClass;
    if (!m_font)
        return false;

    // 폰트 객체 초기화
    constexpr char fontdata[] = "../HLSL_DX11/FontData/Fontdata.txt";
    WCHAR fontdds[] = L"../HLSL_DX11/FontData/Font.dds";
    bool result = m_font->Initialize(device, fontdata, fontdds);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
        return false;
    }

    // 폰트 셰이더 객체 생성
    m_fontShader = new FontShaderClass;
    if (!m_fontShader)
        return false;

    // 폰트 셰이더 객체 초기화
    result = m_fontShader->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // 첫 문장 초기화
    result = InitializeSentence(&m_sentence1, 16, device);
    if (!result)
        return false;

    // 문장 Vertex Buffer를 새 문자열 정보로 업데이트
    constexpr char tp1[] = "God Damn";
    result = UpdateSentence(m_sentence1, tp1, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
    if (!result)
        return false;

    // 둘째 문장 초기화
    result = InitializeSentence(&m_sentence2, 16, device);
    if (!result)
        return false;

    // 문장 Vertex Buffer 새 문자열 정보로 업데이트
    constexpr char tp2[] = "Goodbye";
    result = UpdateSentence(m_sentence2, tp2, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext);
    if (!result)
        return false;

    result = InitializeSentence(&m_key, 16, device);
    if (!result)
        return false;

    constexpr char tp3[] = "Init";
    result = UpdateSentence(m_key, tp3, 20, 60, 1.0f, 1.0f, 1.0f, deviceContext);
    if (!result)
        return false;

    return true;
}


void TextClass::Shutdown()
{
    ReleaseSentence(&m_sentence1);
    ReleaseSentence(&m_sentence2);
    ReleaseSentence(&m_key);

    if (m_fontShader)
    {
        m_fontShader->Shutdown();
        delete m_fontShader;
        m_fontShader = nullptr;
    }

    if (m_font)
    {
        m_font->Shutdown();
        delete m_font;
        m_font = nullptr;
    }
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, const XMMATRIX worldMatrix, const XMMATRIX orthoMatrix) const
{
    // 첫 문장 렌더링
    bool result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
    if (!result)
        return false;

    // 둘째 문장 렌더링
    result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
    if (!result)
        return false;

    result = RenderSentence(deviceContext, m_key, worldMatrix, orthoMatrix);
    if (!result)
        return false;

    return true;
}


bool TextClass::InitializeSentence(SentenceType** sentence, const int maxLength, ID3D11Device* device)
{
    // 새로운 Sentence 객체 생성
    *sentence = new SentenceType;
    if (!*sentence)
        return false;

    // Sentence Buffer 초기화
    (*sentence)->vertexBuffer = nullptr;
    (*sentence)->indexBuffer = nullptr;

    // 문장 최대 길이 설정
    (*sentence)->maxLength = maxLength;

    // Vertex 배열의 Vertex 수 설정 (한 글자 당 1 Quad = 2 Triangle = 6 Vertices)
    (*sentence)->vertexCount = 6 * maxLength;

    // 인덱스 배열의 수 설정
    (*sentence)->indexCount = (*sentence)->vertexCount;

    // Vertex 배열 생성
    auto vertices = new VertexType[(*sentence)->vertexCount];
    if (!vertices)
        return false;

    // 인덱스 배열 생성
    auto indices = new unsigned long[(*sentence)->indexCount];
    if (!indices)
        return false;

    // Vertex 배열 0으로 초기화
    memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

    // 인덱스 배열 초기화
    for (int i = 0; i < (*sentence)->indexCount; i++)
        indices[i] = i;

    // 동적 Vertex Buffer의 Description 작성
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Subresource 구조에 Vertex Data에 대한 포인터 제공
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Vertex Buffer 생성
    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
    if (FAILED(result))
        return false;

    // 정적 인덱스 Buffer의 Description 작성
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Subresource 구조에 인덱스 Data에 대한 포인터 제공
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 Buffer 생성
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
    if (FAILED(result))
        return false;

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, const int positionX, const int positionY, const float red,
                               const float green, const float blue,
                               ID3D11DeviceContext* deviceContext) const
{
    // 문장 색 지정
    sentence->red = red;
    sentence->green = green;
    sentence->blue = blue;

    // Buffer 오버플로우 확인 (글자 수가 maxLength보다 크면 false 반환)
    if (static_cast<int>(strlen(text)) > sentence->maxLength)
        return false;

    // Vertex 배열 생성
    auto vertices = new VertexType[sentence->vertexCount];
    if (!vertices)
        return false;

    // Vertex 배열 0으로 초기화
    memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

    // 렌더링 시작할 화면에서 X, Y 픽셀 위치 계산
    const auto drawX = static_cast<float>(((m_screenWidth / 2) * -1) + positionX);
    const auto drawY = static_cast<float>((m_screenHeight / 2) - positionY);

    // 폰트 클래스를 사용, 문장 텍스트와 문장 렌더링 위치에서 Vertex 배열 생성
    m_font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    // Vertex Buffer를 쓰기 위해 잠금.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    const HRESULT result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    // Vertex Buffer의 데이터를 가리키는 포인터 획득
    const auto verticesPtr = static_cast<VertexType*>(mappedResource.pData);

    // 데이터를 Vertex Buffer에 복사
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

    // Vertex Buffer 잠금 해제.
    deviceContext->Unmap(sentence->vertexBuffer, 0);

    delete[] vertices;
    vertices = nullptr;

    return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
    if (*sentence)
    {
        if ((*sentence)->vertexBuffer)
        {
            (*sentence)->vertexBuffer->Release();
            (*sentence)->vertexBuffer = nullptr;
        }

        if ((*sentence)->indexBuffer)
        {
            (*sentence)->indexBuffer->Release();
            (*sentence)->indexBuffer = nullptr;
        }

        delete *sentence;
        *sentence = nullptr;
    }
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, const SentenceType* sentence, const XMMATRIX worldMatrix,
                               const XMMATRIX orthoMatrix) const
{
    // Vertex Buffer 간격 및 오프셋 변수 생성
    constexpr unsigned int stride = sizeof(VertexType);
    constexpr unsigned int offset = 0;

    // 렌더링할 수 있도록 입력 어셈블러에서 Vertex Buffer를 활성으로 설정
    deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // 렌더링할 수 있도록 입력 어셈블러에서 인덱스 Buffer를 활성으로 설정
    deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Vertex Buffer에서 렌더링해야 하는 Primitive 유형을 삼각형으로 설정
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 입력된 문장 색상으로 픽셀 색상 벡터 생성
    const auto pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

    // 폰트 셰이더를 사용하여 텍스트 렌더링
    const bool result = m_fontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
        m_font->GetTexture(), pixelColor);
    if (!result)
        return false;

    return true;
}

bool TextClass::SetMousePosition(const int mouseX, const int mouseY, ID3D11DeviceContext* deviceContext) const
{
    // mouseX 정수 -> 문자열
    char tpStr[16] = {0,};
    _itoa_s(mouseX, tpStr, 10);

    char mouseStr[16] = {0,};
    strcpy_s(mouseStr, "Mouse X: ");
    strcat_s(mouseStr, tpStr);

    if (!UpdateSentence(m_sentence1, mouseStr, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext))
        return false;

    // mouseY 정수 -> 문자열
    _itoa_s(mouseY, tpStr, 10);

    strcpy_s(mouseStr, "Mouse Y: ");
    strcat_s(mouseStr, tpStr);

    if (!UpdateSentence(m_sentence2, mouseStr, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext))
        return false;

    return true;
}

bool TextClass::SetFps(ID3D11DeviceContext* deviceContext, int fps) const
{
    // FPS를 10000 미만으로 자름
    if (fps > 9999) fps = 9999;

    char tpStr[16] = {0,};
    _itoa_s(fps, tpStr, 10);

    char fpsStr[16] = {0,};
    strcpy_s(fpsStr, "FPS: ");
    strcat_s(fpsStr, tpStr);

    float red = 0;
    float green = 0;
    float blue = 0;

    if (fps >= 60)
    {
        red = 0.0f;
        green = 1.0f;
        blue = 0.0f;
    }
    else if (fps >= 30)
    {
        red = 1.0f;
        green = 1.0f;
        blue = 0.0f;
    }
    else
    {
        red = 1.0f;
        green = 0.0f;
        blue = 0.0f;
    }

    return UpdateSentence(m_sentence1, fpsStr, 20, 20, red, green, blue, deviceContext);
}

bool TextClass::SetCpu(ID3D11DeviceContext* deviceContext, const int cpu) const
{
    char tpStr[16] = {0,};
    _itoa_s(cpu, tpStr, 10);

    char cpuStr[16] = {0,};
    strcpy_s(cpuStr, "CPU: ");
    strcat_s(cpuStr, tpStr);
    strcat_s(cpuStr, "%");

    return UpdateSentence(m_sentence2, cpuStr, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
}

bool TextClass::KeyInput(ID3D11DeviceContext* deviceContext, const char input) const
{
    const char tp[16] = {input,};

    if (!UpdateSentence(m_key, tp, 20, 60, 1.0f, 0.0f, 0.0f, deviceContext))
        return false;

    return true;
}

bool TextClass::SetRenderCount(ID3D11DeviceContext* deviceContext, const int count) const
{
    char tpStr[16] = {0,};
    _itoa_s(count, tpStr, 10);

    char countStr[16] = {0,};
    strcpy_s(countStr, "Total: ");
    strcat_s(countStr, tpStr);

    return UpdateSentence(m_key, countStr, 20, 60, 1.0f, 1.0f, 1.0f, deviceContext);
}
