#pragma once

class LightShaderClass : public AlignedAllocationPolicy<16>
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    class CameraBufferType
    {
    public:
        XMFLOAT3 cameraPosition;
        float padding;
    };

    struct LightBufferType
    {
        XMFLOAT4 ambientColor;
        XMFLOAT4 diffuseColor;
        XMFLOAT3 lightDirection;
        float specularPower;
        XMFLOAT4 specularColor;
    };

public:
    LightShaderClass();
    LightShaderClass(const LightShaderClass&);
    ~LightShaderClass();

    bool Initialize(ID3D11Device*, HWND, int);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3,
                XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float) const;

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
    void ShutdownShader();
    static void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3,
                             XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float) const;
    void RenderShader(ID3D11DeviceContext*, int) const;

private:
    int m_lightVersion = -1; // 0: Point Light, 1: Directional Light, 2: Spot Light(not implemented yet)
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11SamplerState* m_sampleState = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_cameraBuffer = nullptr;
    ID3D11Buffer* m_lightBuffer = nullptr;
};
