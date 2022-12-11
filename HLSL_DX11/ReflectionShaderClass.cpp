#include "Stdafx.h"
#include "ReflectionShaderClass.h"

#include <complex>

ReflectionShaderClass::ReflectionShaderClass()
= default;

ReflectionShaderClass::ReflectionShaderClass(const ReflectionShaderClass&)
{
}

ReflectionShaderClass::~ReflectionShaderClass()
= default;

bool ReflectionShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    constexpr WCHAR vsFile[] = L"../HLSL_DX11/ReflectionShader/Reflection.vs";
    constexpr WCHAR psFile[] = L"../HLSL_DX11/ReflectionShader/Reflection.ps";
    return InitializeShader(device, hwnd, vsFile, psFile);
}

void ReflectionShaderClass::Shutdown()
{
    ShutdownShader();
}

bool ReflectionShaderClass::Render(ID3D11DeviceContext* deviceContext, const int indexCount,
                                   const XMMATRIX worldMatrix, const XMMATRIX viewMatrix, const XMMATRIX projectionMatrix,
                                   ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* texture,
                                   XMMATRIX reflectionMatrix) const
{
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray[0], texture, reflectionMatrix))
    {
        return false;
    }

    RenderShader(deviceContext, indexCount);

    return true;
}

bool ReflectionShaderClass::InitializeShader(ID3D11Device* device, const HWND hwnd, const WCHAR* vsFilename,
                                             const WCHAR* psFilename)
{
    ID3D10Blob* errorMessage = nullptr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    HRESULT result = D3DCompileFromFile(vsFilename, nullptr, nullptr, "ReflectionVertexShader",
        "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK | MB_ICONERROR);
        }

        return false;
    }

    ID3D10Blob* pixelShaderBuffer = nullptr;
    result = D3DCompileFromFile(psFilename, nullptr, nullptr, "ReflectionPixelShader",
        "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK | MB_ICONERROR);
        }

        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
    if (FAILED(result)) return false;

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
    if (FAILED(result)) return false;

    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    constexpr UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result)) return false;

    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
    if (FAILED(result)) return false;

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result)) return false;

    D3D11_BUFFER_DESC reflectionBufferDesc;
    reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
    reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    reflectionBufferDesc.MiscFlags = 0;
    reflectionBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&reflectionBufferDesc, nullptr, &m_reflectionBuffer);
    if (FAILED(result)) return false;

    return true;
}

void ReflectionShaderClass::ShutdownShader()
{
    if (m_reflectionBuffer)
    {
        m_reflectionBuffer->Release();
        m_reflectionBuffer = nullptr;
    }

    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = nullptr;
    }
    if (m_layout)
    {
        m_layout->Release();
        m_matrixBuffer = nullptr;
    }
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = nullptr;
    }
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }
}

void ReflectionShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, const HWND hwnd,
                                                     const WCHAR* shaderFilename)
{
    printf("%s\n", static_cast<const char*>(errorMessage->GetBufferPointer()));

    errorMessage->Release();
    errorMessage = nullptr;

    MessageBox(hwnd, L"Error Compiling Shader.", shaderFilename, MB_OK | MB_ICONERROR);
}

bool ReflectionShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
                                                XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
                                                ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture,
                                                XMMATRIX reflectionMatrix) const
{
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);
    reflectionMatrix = XMMatrixTranspose(reflectionMatrix);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD,
        0, &mappedResource);
    if (FAILED(result)) return false;

    const auto dataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(m_matrixBuffer, 0);

    unsigned int bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    deviceContext->PSSetShaderResources(0, 1, &texture);

    result = deviceContext->Map(m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result)) return false;

    const auto dataPtr2 = static_cast<ReflectionBufferType*>(mappedResource.pData);

    dataPtr2->reflectionMatrix = reflectionMatrix;

    deviceContext->Unmap(m_reflectionBuffer, 0);

    bufferNumber = 1;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_reflectionBuffer);
    deviceContext->PSSetShaderResources(0, 1, &texture);
    deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);

    return true;
}

void ReflectionShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const int indexCount) const
{
    deviceContext->IASetInputLayout(m_layout);
    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
