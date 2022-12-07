#pragma once

class D3DClass : public AlignedAllocationPolicy<16>
{
public:
    D3DClass();
    D3DClass(const D3DClass&);
    ~D3DClass();

    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float) const;
    void EndScene() const;

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

    void GetProjectionmatrix(XMMATRIX&) const;
    void GetWorldMatrix(XMMATRIX&) const;
    void GetOrthoMatrix(XMMATRIX&) const;

    void GetVideoCardInfo(char*, int&) const;

    void TurnZBufferOn() const;
    void TurnZBufferOff() const;

    void TurnOnAlphaBlending() const;
    void TurnOffAlphaBlending() const;

    ID3D11DepthStencilView* GetDepthStencilView() const;
    void SetBackBufferRenderTarget() const;

private:
    bool m_vsync_enabled = false;
    int m_videoCardMemory = 0;
    char m_videoCardDescription[128] = {0, };
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    ID3D11Texture2D* m_depthStencilBuffer = nullptr;
    ID3D11DepthStencilState* m_depthStencilState = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    ID3D11RasterizerState* m_rasterState = nullptr;
    ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
    ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
    ID3D11BlendState* m_alphaDisableBlendingState = nullptr;
    
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    XMMATRIX m_projectionMatrix;
};
