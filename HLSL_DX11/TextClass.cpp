#include "stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"


TextClass::TextClass()
= default;

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
= default;

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight,
                           XMMATRIX baseViewMatrix)
{
    // Store the screen width and height.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Store the base view matrix.
    m_baseViewMatrix = baseViewMatrix;

    // Create the font object.
    m_Font = new FontClass;
    if (!m_Font)
    {
        return false;
    }

    // Initialize the font object.
    constexpr char fontdata[] = "../HLSL_DX11/FontData/Fontdata.txt";
    WCHAR fontdds[] = L"../HLSL_DX11/FontData/Font.dds";
    bool result = m_Font->Initialize(device, fontdata, fontdds);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
        return false;
    }

    // Create the font shader object.
    m_FontShader = new FontShaderClass;
    if (!m_FontShader)
    {
        return false;
    }

    // Initialize the font shader object.
    result = m_FontShader->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // Initialize the first sentence.
    result = InitializeSentence(&m_sentence1, 16, device);
    if (!result)
    {
        return false;
    }

    // Now update the sentence vertex buffer with the new string information.
    constexpr char tp1[] = "Hello";
    result = UpdateSentence(m_sentence1, tp1, 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
    if (!result)
    {
        return false;
    }

    // Initialize the first sentence.
    result = InitializeSentence(&m_sentence2, 16, device);
    if (!result)
    {
        return false;
    }


    // Now update the sentence vertex buffer with the new string information.
    constexpr char tp2[] = "Goodbye";
    result = UpdateSentence(m_sentence2, tp2, 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
    if (!result)
    {
        return false;
    }

    return true;
}


void TextClass::Shutdown()
{
    // Release the first sentence.
    ReleaseSentence(&m_sentence1);

    // Release the second sentence.
    ReleaseSentence(&m_sentence2);

    // Release the font shader object.
    if (m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = nullptr;
    }

    // Release the font object.
    if (m_Font)
    {
        m_Font->Shutdown();
        delete m_Font;
        m_Font = nullptr;
    }

    return;
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, const XMMATRIX worldMatrix, const XMMATRIX orthoMatrix) const
{
    // Draw the first sentence.
    bool result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
    if (!result)
    {
        return false;
    }

    // Draw the second sentence.
    result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
    if (!result)
    {
        return false;
    }

    return true;
}


bool TextClass::InitializeSentence(SentenceType** sentence, const int maxLength, ID3D11Device* device)
{
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;


    // Create a new sentence object.
    *sentence = new SentenceType;
    if (!*sentence)
    {
        return false;
    }

    // Initialize the sentence buffers to null.
    (*sentence)->vertexBuffer = nullptr;
    (*sentence)->indexBuffer = nullptr;

    // Set the maximum length of the sentence.
    (*sentence)->maxLength = maxLength;

    // Set the number of vertices in the vertex array.
    (*sentence)->vertexCount = 6 * maxLength;

    // Set the number of indexes in the index array.
    (*sentence)->indexCount = (*sentence)->vertexCount;

    // Create the vertex array.
    auto vertices = new VertexType[(*sentence)->vertexCount];
    if (!vertices)
    {
        return false;
    }

    // Create the index array.
    auto indices = new unsigned long[(*sentence)->indexCount];
    if (!indices)
    {
        return false;
    }

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

    // Initialize the index array.
    for (int i = 0; i < (*sentence)->indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Create the vertex buffer.
    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Release the vertex array as it is no longer needed.
    delete[] vertices;
    vertices = nullptr;

    // Release the index array as it is no longer needed.
    delete[] indices;
    indices = nullptr;

    return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, const int positionX, const int positionY, const float red,
                               const float green, const float blue,
                               ID3D11DeviceContext* deviceContext) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;


    // Store the color of the sentence.
    sentence->red = red;
    sentence->green = green;
    sentence->blue = blue;

    // Get the number of letters in the sentence.
    const int numLetters = static_cast<int>(strlen(text));

    // Check for possible buffer overflow.
    if (numLetters > sentence->maxLength)
    {
        return false;
    }

    // Create the vertex array.
    auto vertices = new VertexType[sentence->vertexCount];
    if (!vertices)
    {
        return false;
    }

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

    // Calculate the X and Y pixel position on the screen to start drawing to.
    const auto drawX = static_cast<float>(((m_screenWidth / 2) * -1) + positionX);
    const auto drawY = static_cast<float>((m_screenHeight / 2) - positionY);

    // Use the font class to build the vertex array from the sentence text and sentence draw location.
    m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    // Lock the vertex buffer so it can be written to.
    HRESULT result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    // Get a pointer to the data in the vertex buffer.
    const auto verticesPtr = static_cast<VertexType*>(mappedResource.pData);

    // Copy the data into the vertex buffer.
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

    // Unlock the vertex buffer.
    deviceContext->Unmap(sentence->vertexBuffer, 0);

    // Release the vertex array as it is no longer needed.
    delete[] vertices;
    vertices = nullptr;

    return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
    if (*sentence)
    {
        // Release the sentence vertex buffer.
        if ((*sentence)->vertexBuffer)
        {
            (*sentence)->vertexBuffer->Release();
            (*sentence)->vertexBuffer = nullptr;
        }

        // Release the sentence index buffer.
        if ((*sentence)->indexBuffer)
        {
            (*sentence)->indexBuffer->Release();
            (*sentence)->indexBuffer = nullptr;
        }

        // Release the sentence.
        delete *sentence;
        *sentence = nullptr;
    }

    return;
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, const SentenceType* sentence, XMMATRIX worldMatrix,
                               const XMMATRIX orthoMatrix) const
{
    unsigned int stride, offset;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create a pixel color vector with the input sentence color.
    const auto pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

    // Render the text using the font shader.
    bool result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),
        pixelColor);
    if (!result)
    {
        false;
    }

    return true;
}
