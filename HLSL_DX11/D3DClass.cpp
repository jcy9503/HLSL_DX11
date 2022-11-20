#include "Stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass()
{
    
}

D3DClass::D3DClass(const D3DClass&)
{
    
}

D3DClass::~D3DClass()
{
    
}

bool D3DClass::initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
    bool fullscreen, float screenDepth, float screenNear)
{
    m_vsync_enabled = vsync;

    IDXGIFactory* factory = nullptr;
    if(FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
    {
        return false;
    }

    IDXGIAdapter* adapter = nullptr;
    if(FAILED(factory->EnumAdapters(0, &adapter)))
    {
        return false;
    }

    IDXGIOutput* adapterOutput = nullptr;
    if(FAILED(adapter->EnumOutputs(0, &adapterOutput)))
    {
        return false;
    }

    unsigned int numModes = 0;
    if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
    {
        return false;
    }

    DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
    if(!displayModeList)
    {
        return false;
    }

    if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
    {
        return false;
    }

    unsigned int numerator = 0;
    unsigned int denominator = 0;
    for(unsigned int i = 0; i < numModes; ++i)
    {
        if(displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if(displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    DXGI_ADAPTER_DESC adapterDesc;
    if(FAILED(adapter->GetDesc(&adapterDesc)))
    {
        return false;
    }

    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    size_t stringLength = 0;
    if(wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
    {
        return false;
    }

    delete[] displayModeList;
    displayModeList = 0;

    adapterOutput->Release();
    adapterOutput = 0;

    adapter->Release();
    adapter = 0;

    factory->Release();
    factory = 0;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.BufferCount = 1;

    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if(m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }
    
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    
    swapChainDesc.OutputWindow = hwnd;
    
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    if(fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    if(FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext)))
    {
        return false;
    }

    ID3D11Texture2D* backBufferPtr = nullptr;
    if(FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
    {
        return false;
    }

    if(FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
    {
        return false;
    }

    backBufferPtr->Release();
    backBufferPtr = 0;

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

    if(FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))
    {
        return false;
    }

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

    if(FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
    {
        return false;
    }

    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    if(FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)))
    {
        return false;
    }

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

    if(FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))
    {
        return false;
    }

    m_deviceContext->RSSetState(m_rasterState);

    D3D11_VIEWPORT viewport;
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    m_deviceContext->RSSetViewports(1, &viewport);

    float fieldOfView = 3.141592654f / 4.0f;
    float screenAspect = (float)screenWidth / (float)screenHeight;

    m_projectionMatrix = XMMatrixPerspectiveLH(fieldOfView, screenAspect, screenNear, screenDepth);
    m_worldMatrix = XMMatrixIdentity();
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}

void D3DClass::Shutdown()
{
    if(m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if(m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = 0;
    }

    if(m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = 0;
    }

    if(m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = 0;
    }

    if(m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = 0;
    }

    if(m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }

    if(m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = 0;
    }

    if(m_device)
    {
        m_device->Release();
        m_device = 0;
    }

    if(m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = 0;
    }
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4] = {red, green, blue, alpha};
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
    if(m_vsync_enabled)
    {
        m_swapChain->Present(1, 0);
    }
    else
    {
        m_swapChain->Present(0, 0);
    }
}

ID3D11Device* D3DClass::GetDevice()
{
    return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return m_deviceContext;
}

void D3DClass::GetProjectionmatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}