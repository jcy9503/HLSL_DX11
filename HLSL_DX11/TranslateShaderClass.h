#pragma once
#include "AlignedAllocationPolicy.h"

class TranslateShaderClass : public AlignedAllocationPolicy<16>
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct TranslateBufferType
    {
        float translation;
        XMFLOAT3 padding;
    };

public:
    TranslateShaderClass();
    TranslateShaderClass(const TranslateShaderClass&);
    ~TranslateShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**, float) const;

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
    void ShutdownShader();
    static void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float) const;
    void RenderShader(ID3D11DeviceContext*, int) const;
    
private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11SamplerState* m_sampleState = nullptr;
    ID3D11Buffer* m_translateBuffer = nullptr;
};
