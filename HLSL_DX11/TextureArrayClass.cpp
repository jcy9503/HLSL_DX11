#include "Stdafx.h"
#include "TextureArrayClass.h"

TextureArrayClass::TextureArrayClass()
= default;

TextureArrayClass::TextureArrayClass(const TextureArrayClass&)
{
    
}

TextureArrayClass::~TextureArrayClass()
= default;

bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
    if(FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_textures[0])))
        return false;

    if(FAILED(CreateDDSTextureFromFile(device, filename2, nullptr, &m_textures[1])))
        return false;

    return true;
}

void TextureArrayClass::Shutdown()
{
    for (auto& m_texture : m_textures)
    {
        if(m_texture)
        {
            m_texture->Release();
            m_texture = nullptr;
        }
    }
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
    return m_textures;
}
