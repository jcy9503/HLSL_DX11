#pragma once

class TextureClass;

class FontClass
{
private:
    struct FontType
    {
        float left, right;
        int size;
    };

    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    FontClass();
    FontClass(const FontClass&);
    ~FontClass();

    bool Initialize(ID3D11Device*, const char*, WCHAR*);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture() const;

    void BuildVertexArray(void*, const char*, float, float) const;

private:
    bool LoadFontData(const char*);
    void ReleaseFontData();
    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

private:
    FontType* m_font = nullptr;
    TextureClass* m_texture = nullptr;
};