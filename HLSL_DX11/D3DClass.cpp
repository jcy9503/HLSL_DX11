#include "Stdafx.h"
#include "D3DClass.h"
#include <fstream>
#include <iostream>

D3DClass::D3DClass()
{
}

D3DClass::D3DClass(const D3DClass&)
{
}

D3DClass::~D3DClass()
= default;

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
                          bool fullscreen, float screenDepth, float screenNear)
{
    m_vsync_enabled = vsync;

    IDXGIFactory* factory = nullptr;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
    if (FAILED(result)) return false;

    IDXGIAdapter* adapter = nullptr;
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) return false;

    IDXGIOutput* adapterOutput = nullptr;
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) return false;

    unsigned int numModes = 0;
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
    if (FAILED(result)) return false;

    auto displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) return false;

    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) return false;

    unsigned int numerator = 0;
    unsigned int denominator = 0;
    for (unsigned int i = 0; i < numModes; ++i)
    {
        if (displayModeList[i].Width == static_cast<unsigned int>(screenWidth))
        {
            if (displayModeList[i].Height == static_cast<unsigned int>(screenHeight))
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    DXGI_ADAPTER_DESC adapterDesc;
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) return false;

    m_videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
    size_t stringLength = 0;
    if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
        return false;

    delete[] displayModeList;
    displayModeList = nullptr;

    adapterOutput->Release();
    adapterOutput = nullptr;

    adapter->Release();
    adapter = nullptr;

    factory->Release();
    factory = nullptr;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    m_vsync_enabled
        ? swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator, swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator
        : swapChainDesc.BufferDesc.RefreshRate.Numerator = 0, swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    fullscreen ? swapChainDesc.Windowed = false : swapChainDesc.Windowed = true;

    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION,
        &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);
    if (FAILED(result)) return false;

    ID3D11Texture2D* backBufferPtr = nullptr;
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBufferPtr));
    if (FAILED(result)) return false;

    result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
    if (FAILED(result)) return false;

    backBufferPtr->Release();
    backBufferPtr = nullptr;

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
    if (FAILED(result)) return false;

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result)) return false;

    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result)) return false;

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if (FAILED(result)) return false;

    m_deviceContext->RSSetState(m_rasterState);

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<float>(screenWidth);
    viewport.Height = static_cast<float>(screenHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    m_deviceContext->RSSetViewports(1, &viewport);

    float fieldOfView = XM_PI / 4.0f;
    float screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
    m_worldMatrix = XMMatrixIdentity();
    m_orthoMatrix = XMMatrixOrthographicLH(static_cast<float>(screenWidth), static_cast<float>(screenHeight), screenNear, screenDepth);

    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
    if (FAILED(result)) return false;

    D3D11_BLEND_DESC blendStateDesc;
    ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

    blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    result = m_device->CreateBlendState(&blendStateDesc, &m_alphaEnableBlendingState);
    if (FAILED(result)) return false;

    blendStateDesc.RenderTarget[0].BlendEnable = FALSE;

    result = m_device->CreateBlendState(&blendStateDesc, &m_alphaDisableBlendingState);
    if (FAILED(result)) return false;

    std::ofstream fout;
    fout.open("video_card_info.txt");
    fout << "Video Card Name:\t\t" << m_videoCardDescription << std::endl;
    fout << "Video Card Memory:\t" << m_videoCardMemory << "MB" << std::endl;
    fout << "Numerator:\t\t\t" << numerator << std::endl;
    fout << "Denominator:\t\t" << denominator << std::endl;
    fout.close();

    return true;
}

void D3DClass::Shutdown()
{
    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }

    if (m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, nullptr);
    }

    if (m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }

    if (m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = nullptr;
    }

    if (m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = nullptr;
    }

    if (m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = nullptr;
    }

    if (m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = nullptr;
    }

    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    if (m_depthDisabledStencilState)
    {
        m_depthDisabledStencilState->Release();
        m_depthDisabledStencilState = nullptr;
    }

    if (m_alphaEnableBlendingState)
    {
        m_alphaEnableBlendingState->Release();
        m_alphaEnableBlendingState = nullptr;
    }

    if (m_alphaDisableBlendingState)
    {
        m_alphaDisableBlendingState->Release();
        m_alphaDisableBlendingState = nullptr;
    }
}

void D3DClass::BeginScene(const float red, const float green, const float blue, const float alpha) const
{
    // Buffer를 지울 색 결정
    const float color[4] = {red, green, blue, alpha};

    // Back Buffer 지우기
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // Depth Buffer 지우기
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene() const
{
    m_vsync_enabled ? m_swapChain->Present(1, 0) : m_swapChain->Present(0, 0);
}

ID3D11Device* D3DClass::GetDevice() const
{
    return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext() const
{
    return m_deviceContext;
}

void D3DClass::GetProjectionmatrix(XMMATRIX& projectionMatrix) const
{
    projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix) const
{
    worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix) const
{
    orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory) const
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}

void D3DClass::TurnZBufferOn() const
{
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void D3DClass::TurnZBufferOff() const
{
    m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void D3DClass::TurnOnAlphaBlending() const
{
    constexpr float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOffAlphaBlending() const
{
    constexpr float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

ID3D11DepthStencilView* D3DClass::GetDepthStencilView() const
{
    return m_depthStencilView;
}

void D3DClass::SetBackBufferRenderTarget() const
{
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}
