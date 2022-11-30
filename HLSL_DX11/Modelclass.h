#pragma once

class TextureClass;

class ModelClass : public AlignedAllocationPolicy<16>
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };

    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device*, const char*, WCHAR*);
    void Shutdown();
    void Render(ID3D11DeviceContext*) const;
    
    int GetIndexCount() const;
    ID3D11ShaderResourceView* GetTexture() const;

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*) const;

    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

    bool LoadModel(const char*);
    void ReleaseModel();

private:
    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_indexBuffer = nullptr;
    int m_vertexCount = 0;
    int m_indexCount = 0;
    TextureClass* m_texture = nullptr;
    ModelType* m_model = nullptr;
};
