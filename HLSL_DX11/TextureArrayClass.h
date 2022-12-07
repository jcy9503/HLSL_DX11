#pragma once

class TextureArrayClass
{
public:
    TextureArrayClass();
    TextureArrayClass(const TextureArrayClass&);
    ~TextureArrayClass();

    bool Initialize(ID3D11Device*, WCHAR*);
    void Shutdown();

    ID3D11ShaderResourceView** GetTextureArray();

private:
    ID3D11ShaderResourceView* m_textures[1] = {nullptr};
};
