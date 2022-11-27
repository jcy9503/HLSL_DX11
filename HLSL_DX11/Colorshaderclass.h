#pragma once

class ColorShaderClass : public AlignedAllocationPolicy<16>
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

public:
    ColorShaderClass();
    ColorShaderClass(const ColorShaderClass&);
    ~ColorShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX) const;

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
    static HRESULT CompileShader(_In_ LPCWSTR, _In_ LPCSTR, _In_ LPCSTR, _Outptr_ ID3DBlob**);
    void ShutdownShader();
    static void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX) const;
    void RenderShader(ID3D11DeviceContext*, int) const;

private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
};
