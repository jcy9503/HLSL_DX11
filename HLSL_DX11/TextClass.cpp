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

    // �⺻ �� ��� ����
    m_baseViewMatrix = baseViewMatrix;

    // ��Ʈ ��ü ����
    m_font = new FontClass;
    if (!m_font)
        return false;

    // ��Ʈ ��ü �ʱ�ȭ
    constexpr char fontdata[] = "../HLSL_DX11/FontData/Fontdata.txt";
    WCHAR fontdds[] = L"../HLSL_DX11/FontData/Font.dds";
    bool result = m_font->Initialize(device, fontdata, fontdds);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
        return false;
    }

    // ��Ʈ ���̴� ��ü ����
    m_fontShader = new FontShaderClass;
    if (!m_fontShader)
        return false;

    // ��Ʈ ���̴� ��ü �ʱ�ȭ
    result = m_fontShader->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // ù ���� �ʱ�ȭ
    result = InitializeSentence(&m_sentence1, 16, device);
    if (!result)
        return false;

    // ���� Vertex Buffer�� �� ���ڿ� ������ ������Ʈ
    constexpr char tp1[] = "God Damn";
    result = UpdateSentence(m_sentence1, tp1, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
    if (!result)
        return false;

    // ��° ���� �ʱ�ȭ
    result = InitializeSentence(&m_sentence2, 16, device);
    if (!result)
        return false;

    // ���� Vertex Buffer �� ���ڿ� ������ ������Ʈ
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
    // ù ���� ������
    bool result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
    if (!result)
        return false;

    // ��° ���� ������
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
    // ���ο� Sentence ��ü ����
    *sentence = new SentenceType;
    if (!*sentence)
        return false;

    // Sentence Buffer �ʱ�ȭ
    (*sentence)->vertexBuffer = nullptr;
    (*sentence)->indexBuffer = nullptr;

    // ���� �ִ� ���� ����
    (*sentence)->maxLength = maxLength;

    // Vertex �迭�� Vertex �� ���� (�� ���� �� 1 Quad = 2 Triangle = 6 Vertices)
    (*sentence)->vertexCount = 6 * maxLength;

    // �ε��� �迭�� �� ����
    (*sentence)->indexCount = (*sentence)->vertexCount;

    // Vertex �迭 ����
    auto vertices = new VertexType[(*sentence)->vertexCount];
    if (!vertices)
        return false;

    // �ε��� �迭 ����
    auto indices = new unsigned long[(*sentence)->indexCount];
    if (!indices)
        return false;

    // Vertex �迭 0���� �ʱ�ȭ
    memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

    // �ε��� �迭 �ʱ�ȭ
    for (int i = 0; i < (*sentence)->indexCount; i++)
        indices[i] = i;

    // ���� Vertex Buffer�� Description �ۼ�
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Subresource ������ Vertex Data�� ���� ������ ����
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Vertex Buffer ����
    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
    if (FAILED(result))
        return false;

    // ���� �ε��� Buffer�� Description �ۼ�
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Subresource ������ �ε��� Data�� ���� ������ ����
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� Buffer ����
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
    // ���� �� ����
    sentence->red = red;
    sentence->green = green;
    sentence->blue = blue;

    // Buffer �����÷ο� Ȯ�� (���� ���� maxLength���� ũ�� false ��ȯ)
    if (static_cast<int>(strlen(text)) > sentence->maxLength)
        return false;

    // Vertex �迭 ����
    auto vertices = new VertexType[sentence->vertexCount];
    if (!vertices)
        return false;

    // Vertex �迭 0���� �ʱ�ȭ
    memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

    // ������ ������ ȭ�鿡�� X, Y �ȼ� ��ġ ���
    const auto drawX = static_cast<float>(((m_screenWidth / 2) * -1) + positionX);
    const auto drawY = static_cast<float>((m_screenHeight / 2) - positionY);

    // ��Ʈ Ŭ������ ���, ���� �ؽ�Ʈ�� ���� ������ ��ġ���� Vertex �迭 ����
    m_font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    // Vertex Buffer�� ���� ���� ���.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    const HRESULT result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    // Vertex Buffer�� �����͸� ����Ű�� ������ ȹ��
    const auto verticesPtr = static_cast<VertexType*>(mappedResource.pData);

    // �����͸� Vertex Buffer�� ����
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

    // Vertex Buffer ��� ����.
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
    // Vertex Buffer ���� �� ������ ���� ����
    constexpr unsigned int stride = sizeof(VertexType);
    constexpr unsigned int offset = 0;

    // �������� �� �ֵ��� �Է� ��������� Vertex Buffer�� Ȱ������ ����
    deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // �������� �� �ֵ��� �Է� ��������� �ε��� Buffer�� Ȱ������ ����
    deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Vertex Buffer���� �������ؾ� �ϴ� Primitive ������ �ﰢ������ ����
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // �Էµ� ���� �������� �ȼ� ���� ���� ����
    const auto pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

    // ��Ʈ ���̴��� ����Ͽ� �ؽ�Ʈ ������
    const bool result = m_fontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
        m_font->GetTexture(), pixelColor);
    if (!result)
        return false;

    return true;
}

bool TextClass::SetMousePosition(const int mouseX, const int mouseY, ID3D11DeviceContext* deviceContext) const
{
    // mouseX ���� -> ���ڿ�
    char tpStr[16] = {0,};
    _itoa_s(mouseX, tpStr, 10);

    char mouseStr[16] = {0,};
    strcpy_s(mouseStr, "Mouse X: ");
    strcat_s(mouseStr, tpStr);

    if (!UpdateSentence(m_sentence1, mouseStr, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext))
        return false;

    // mouseY ���� -> ���ڿ�
    _itoa_s(mouseY, tpStr, 10);

    strcpy_s(mouseStr, "Mouse Y: ");
    strcat_s(mouseStr, tpStr);

    if (!UpdateSentence(m_sentence2, mouseStr, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext))
        return false;

    return true;
}

bool TextClass::SetFps(ID3D11DeviceContext* deviceContext, int fps) const
{
    // FPS�� 10000 �̸����� �ڸ�
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
