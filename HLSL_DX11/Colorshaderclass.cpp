#include "Stdafx.h"
#include "Colorshaderclass.h"
#include <string>

ColorShaderClass::ColorShaderClass()
= default;

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{
    
}

ColorShaderClass::~ColorShaderClass()
= default;

bool ColorShaderClass::Initialize(ID3D11Device* device, const HWND hwnd)
{
    return InitializeShader(device, hwnd, L"../HLSL_DX11/Demo04/Color.vs", L"../HLSL_DX11/Demo04/Color.ps");
}

void ColorShaderClass::Shutdown()
{
    ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, const int indexCount
    , const XMMATRIX worldMatrix, const XMMATRIX viewMatrix, const XMMATRIX projectionMatrix) const
{
    if(!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
    {
        return false;
    }

    RenderShader(deviceContext, indexCount);

    return true;
}

HRESULT ColorShaderClass::CompileShader(const LPCWSTR srcFile, const LPCSTR entryPoint, const LPCSTR profile, ID3DBlob** blob)
{
    if(!srcFile || !entryPoint || !profile || !blob) return E_INVALIDARG;

    *blob = nullptr;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG;
#endif

    const D3D_SHADER_MACRO defines[] =
    {
        nullptr
    };

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    const HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, profile,
                                          flags, 0, &shaderBlob, &errorBlob);
    if(FAILED(hr))
    {
        if(errorBlob)
        {
            OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
        if(shaderBlob) shaderBlob->Release();

        return hr;
    }
    *blob = shaderBlob;

    return hr;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, const HWND hwnd,
    const WCHAR* vsFilename, const WCHAR* psFilename)
{
    ID3D10Blob* vertexShaderBuffer = nullptr;
    HRESULT hr = CompileShader(vsFilename, "ColorVertexShader", "vs_5_0", &vertexShaderBuffer);
    if(FAILED(hr))
    {
        MessageBox(hwnd, L"Failed compiling vertex shader", vsFilename, MB_OK|MB_ICONERROR);
        return false;
    }

    ID3D10Blob* pixelShaderBuffer = nullptr;
    hr = CompileShader(psFilename, "ColorPixelShader", "ps_5_0", &pixelShaderBuffer);
    if(FAILED(hr))
    {
        vertexShaderBuffer->Release();
        MessageBox(hwnd, L"Failed compiling pixel shader", psFilename, MB_OK|MB_ICONERROR);
        return false;
    }

    if(FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
        NULL, &m_vertexShader)))
    {
        return false;
    }

    if(FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
        NULL, &m_pixelShader)))
    {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    constexpr unsigned int numElements = std::size(polygonLayout);

    if(FAILED(device->CreateInputLayout(polygonLayout, numElements,
        vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
    {
        return false;
    }

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

    if(FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
    {
        return false;
    }
    return true;
}

void ColorShaderClass::ShutdownShader()
{
    if(m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = nullptr;
    }

    if(m_layout)
    {
        m_layout->Release();
        m_layout = nullptr;
    }

    if(m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }
    
    if(m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFile)
{
    OutputDebugStringA(static_cast<const char*>(errorMessage->GetBufferPointer()));

    errorMessage->Release();
    errorMessage = nullptr;

    MessageBox(hwnd, L"Error Compiling Shader.", shaderFile, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) const
{
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if(FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    const auto dataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(m_matrixBuffer, 0);

    constexpr unsigned bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const int indexCount) const
{
    deviceContext->IASetInputLayout(m_layout);

    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}
