#pragma once

class WaterShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct ReflectionBufferType
    {
        XMMATRIX reflection;
    };

    struct WaterBufferType
    {
        float waterTranslation;
        float reflectRefractScale;
        XMFLOAT2 padding;
    };

public:
    WaterShaderClass();
    WaterShaderClass(const WaterShaderClass&);
    ~WaterShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
                ID3D11ShaderResourceView**, float, float);

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
                             ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11SamplerState* m_sampleState = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_reflectionBuffer = nullptr;
    ID3D11Buffer* m_waterBuffer = nullptr;
};
