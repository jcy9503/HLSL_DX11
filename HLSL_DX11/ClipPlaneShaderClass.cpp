#include "Stdafx.h"
#include "ClipPlaneShaderClass.h"

#include <complex>

ClipPlaneShaderClass::ClipPlaneShaderClass()
= default;

ClipPlaneShaderClass::ClipPlaneShaderClass(const ClipPlaneShaderClass&)
{
}

ClipPlaneShaderClass::~ClipPlaneShaderClass()
= default;

bool ClipPlaneShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    WCHAR vsFile[] = L"../HLSL_DX11/ClipPlaneShader/ClipPlane.vs";
    WCHAR psFile[] = L"../HLSL_DX11/ClipPlaneShader/ClipPlane.ps";
    return InitializeShader(device, hwnd, vsFile, psFile);
}

void ClipPlaneShaderClass::Shutdown()
{
    ShutdownShader();
}

bool ClipPlaneShaderClass::Render(ID3D11DeviceContext* deviceContext, const int indexCount,
                                const XMMATRIX worldMatrix, const XMMATRIX viewMatrix, const XMMATRIX projectionMatrix,
                                ID3D11ShaderResourceView** texture, XMFLOAT4 clipPlane) const
{
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture[0], clipPlane))
    {
        return false;
    }

    RenderShader(deviceContext, indexCount);

    return true;
}

bool ClipPlaneShaderClass::InitializeShader(ID3D11Device* device, const HWND hwnd, const WCHAR* vsFilename,
                                          const WCHAR* psFilename)
{
    ID3D10Blob* errorMessage = nullptr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    HRESULT result = D3DCompileFromFile(vsFilename, nullptr, nullptr, "ClipPlaneVertexShader",
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
    result = D3DCompileFromFile(psFilename, nullptr, nullptr, "ClipPlanePixelShader",
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

    result = device->CreateInputLayout(polygonLayout, numElements,
                                       vertexShaderBuffer->GetBufferPointer(),
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

    D3D11_BUFFER_DESC clipPlaneBufferDesc;
    clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    clipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);
    clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    clipPlaneBufferDesc.MiscFlags = 0;
    clipPlaneBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&clipPlaneBufferDesc, nullptr, &m_clipPlaneBuffer);
    if(FAILED(result)) return false;
    
    return true;
}

void ClipPlaneShaderClass::ShutdownShader()
{
    if(m_clipPlaneBuffer)
    {
        m_clipPlaneBuffer->Release();
        m_clipPlaneBuffer = nullptr;
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

void ClipPlaneShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, const HWND hwnd,
                                                  const WCHAR* shaderFilename)
{
    printf("%s\n", static_cast<const char*>(errorMessage->GetBufferPointer()));

    errorMessage->Release();
    errorMessage = nullptr;

    MessageBox(hwnd, L"Error Compiling Shader.", shaderFilename, MB_OK | MB_ICONERROR);
}

bool ClipPlaneShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
                                             XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
                                             ID3D11ShaderResourceView* texture, XMFLOAT4 clipPlane) const
{
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

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

    result = deviceContext->Map(m_clipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result)) return false;

    auto dataPtr2 = static_cast<ClipPlaneBufferType*>(mappedResource.pData);

    dataPtr2->clipPlane = clipPlane;

    deviceContext->Unmap(m_clipPlaneBuffer, 0);

    bufferNumber = 1;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_clipPlaneBuffer);

    return true;
}

void ClipPlaneShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const int indexCount) const
{
    deviceContext->IASetInputLayout(m_layout);
    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
