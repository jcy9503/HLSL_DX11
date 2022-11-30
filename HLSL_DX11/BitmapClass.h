#pragma once

class TextureClass;

class BitmapClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    BitmapClass();
    BitmapClass(const BitmapClass&);
    ~BitmapClass();

    bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, int);

    int GetIndexCount() const;
    ID3D11ShaderResourceView* GetTexture() const;

private:
    bool Initializebuffers(ID3D11Device*);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext*, int, int);
    void RenderBuffers(ID3D11DeviceContext*) const;

    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

private:
    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_indexBuffer = nullptr;
    int m_vertexCount = 0;
    int m_indexCount = 0;
    TextureClass* m_texture = nullptr;
    int m_screenWidth = 0;
    int m_screenHeight = 0;
    int m_bitmapWidth = 0;
    int m_bitmapHeight = 0;
    int m_previousPosX = 0;
    int m_previousPosY = 0;
    
};