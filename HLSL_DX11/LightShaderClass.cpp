﻿#include "Stdafx.h"
#include "LightShaderClass.h"

LightShaderClass::LightShaderClass()
= default;

LightShaderClass::LightShaderClass(const LightShaderClass&)
{
}

LightShaderClass::~LightShaderClass()
= default;

bool LightShaderClass::Initialize(ID3D11Device* device, const HWND hwnd, const int lightVer)
{
    m_lightVersion = lightVer;

    constexpr WCHAR vsFileAmbient[] = L"../HLSL_DX11/LightShader/PointLight.vs";
    constexpr WCHAR psFileAmbient[] = L"../HLSL_DX11/LightShader/PointLight.ps";
    constexpr WCHAR vsFileDir[] = L"../HLSL_DX11/LightShader/DirLight.vs";
    constexpr WCHAR psFileDir[] = L"../HLSL_DX11/LightShader/DirLight.ps";
    
    switch (m_lightVersion)
    {
    case 0:
        return InitializeShader(device, hwnd, vsFileAmbient, psFileAmbient);
    case 1:
        return InitializeShader(device, hwnd, vsFileDir, psFileDir);
    default:
        break;
    }

    return false;
}

void LightShaderClass::Shutdown()
{
    ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, const int indexCount, const XMMATRIX worldMatrix,
                              const XMMATRIX viewMatrix, const XMMATRIX projectionMatrix,
                              ID3D11ShaderResourceView* texture, const XMFLOAT3 lightDirection,
                              const XMFLOAT4 ambientColor, const XMFLOAT4 diffuseColor, const XMFLOAT3 cameraPosition,
                              const XMFLOAT4 specularColor, const float specularPower) const
{
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection,
        ambientColor, diffuseColor, cameraPosition, specularColor, specularPower))
        return false;

    RenderShader(deviceContext, indexCount);

    return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, const HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    ID3D10Blob* errorMessage = nullptr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    HRESULT result = D3DCompileFromFile(vsFilename, nullptr, nullptr, "LightVertexShader", "vs_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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
    result = D3DCompileFromFile(psFilename, nullptr, nullptr, "LightPixelShader", "ps_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
        nullptr, &m_vertexShader);
    if (FAILED(result)) return false;

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
        nullptr, &m_pixelShader);
    if (FAILED(result)) return false;

    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
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

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    constexpr UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result)) return false;

    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

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

    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
    if (FAILED(result)) return false;

    D3D11_BUFFER_DESC lightBufferDesc;
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    switch(m_lightVersion)
    {
    case 0:
        lightBufferDesc.ByteWidth = sizeof(AmbientLightBufferType);
        break;
    case 1:
        lightBufferDesc.ByteWidth = sizeof(DirLightBufferType);
        break;
    default:
        break;
    }
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
    if (FAILED(result)) return false;

    if (m_lightVersion == 0)
    {
        D3D11_BUFFER_DESC cameraBufferDesc;
        cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
        cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cameraBufferDesc.MiscFlags = 0;
        cameraBufferDesc.StructureByteStride = 0;

        result = device->CreateBuffer(&cameraBufferDesc, nullptr, &m_cameraBuffer);
        if (FAILED(result)) return false;
    }

    return true;
}

void LightShaderClass::ShutdownShader()
{
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = nullptr;
    }

    if (m_lightBuffer)
    {
        m_lightBuffer->Release();
        m_lightBuffer = nullptr;
    }

    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = nullptr;
    }

    if (m_layout)
    {
        m_layout->Release();
        m_layout = nullptr;
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

    if (m_cameraBuffer)
    {
        m_cameraBuffer->Release();
        m_cameraBuffer = nullptr;
    }
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, const HWND hwnd, const WCHAR* shaderFilename)
{
    OutputDebugStringA(static_cast<const char*>(errorMessage->GetBufferPointer()));

    errorMessage->Release();
    errorMessage = nullptr;

    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK | MB_ICONERROR);
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
                                           XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
                                           ID3D11ShaderResourceView* texture, const XMFLOAT3 lightDirection,
                                           const XMFLOAT4 ambientColor, const XMFLOAT4 diffuseColor,
                                           const XMFLOAT3 cameraPosition, const XMFLOAT4 specularColor,
                                           const float specularPower) const
{
    deviceContext->PSSetShaderResources(0, 1, &texture);
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    const auto dataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);
    
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);
    
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(m_matrixBuffer, 0);

    unsigned int bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    if (m_lightVersion == 0)
    {
        result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(result)) return false;

        const auto dataPtr3 = static_cast<CameraBufferType*>(mappedResource.pData);
        dataPtr3->cameraPosition = cameraPosition;
        dataPtr3->padding = 0.0f;

        deviceContext->Unmap(m_cameraBuffer, 0);

        bufferNumber = 1;

        deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
    }

    result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;
    
    const auto ambientPtr = static_cast<AmbientLightBufferType*>(mappedResource.pData);
    const auto dirPtr = static_cast<DirLightBufferType*>(mappedResource.pData);

    if(m_lightVersion == 0)
    {
        const auto dataPtr2 = static_cast<AmbientLightBufferType*>(mappedResource.pData);

        dataPtr2->diffuseColor = diffuseColor;
        dataPtr2->lightDirection = lightDirection;
        dataPtr2->ambientColor = ambientColor;
        dataPtr2->specularColor = specularColor;
        dataPtr2->specularPower = specularPower;

        deviceContext->Unmap(m_lightBuffer, 0);
    
        bufferNumber = 0;
    
        deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
    }
    
    if (m_lightVersion == 1)
    {
        const auto dataPtr2 = static_cast<DirLightBufferType*>(mappedResource.pData);

        dataPtr2->diffuseColor = diffuseColor;
        dataPtr2->lightDirection = lightDirection;
        dataPtr2->padding = 0.0f;

        deviceContext->Unmap(m_lightBuffer, 0);

        bufferNumber = 0;

        deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
    }
    
    return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const int indexCount) const
{
    deviceContext->IASetInputLayout(m_layout);
    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
