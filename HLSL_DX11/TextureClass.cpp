#include "Stdafx.h"
#include "TextureClass.h"

TextureClass::TextureClass()
= default;

TextureClass::TextureClass(const TextureClass&)
{
}

TextureClass::~TextureClass()
= default;

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
    // int width = 0;
    // int height = 0;

    // D3D11_TEXTURE2D_DESC textureDesc;
    // textureDesc.Height = height;
    // textureDesc.Width = width;
    // textureDesc.MipLevels = 0;
    // textureDesc.ArraySize = 1;
    // textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // textureDesc.SampleDesc.Count = 1;
    // textureDesc.SampleDesc.Quality = 0;
    // textureDesc.Usage = D3D11_USAGE_DEFAULT;
    // textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    // textureDesc.CPUAccessFlags = 0;
    // textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    HRESULT result = CreateDDSTextureFromFile(device, filename, nullptr, &m_textureView);
    if(FAILED(result)) return false;

    // D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    // srvDesc.Format = textureDesc.Format;
    // srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    // srvDesc.Texture2D.MostDetailedMip = 0;
    // srvDesc.Texture2D.MipLevels = -1;

    // result = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
    // if (FAILED(result)) return false;

    // deviceContext->GenerateMips(m_textureView);

    return true;
}

void TextureClass::Shutdown()
{
    if (m_texture)
    {
        m_texture->Release();
        m_texture = nullptr;
    }

    if (m_textureView)
    {
        m_textureView->Release();
        m_textureView = nullptr;
    }

    if (m_targaData)
    {
        delete[] m_targaData;
        m_targaData = nullptr;
    }
}

ID3D11ShaderResourceView* TextureClass::GetTexture() const
{
    return m_textureView;
}

bool TextureClass::LoadTarga(const char* filename, int& height, int& width)
{
    FILE* filePtr;
    if (fopen_s(&filePtr, filename, "rb") != 0) return false;

    TargaHeader targaFileHeader{};
    auto count = static_cast<unsigned int>(fread(&targaFileHeader,
                                                 sizeof(TargaHeader), 1, filePtr));
    if (count != 1) return false;

    height = static_cast<int>(targaFileHeader.height);
    width = static_cast<int>(targaFileHeader.width);
    const int bpp = static_cast<int>(targaFileHeader.bpp);

    if (bpp != 32) return false;

    const int imageSize = width * height * 4;

    auto* targaImage = new unsigned char[imageSize];
    if (!targaImage) return false;

    count = static_cast<unsigned int>(fread(targaImage, 1, imageSize, filePtr));
    if (count != imageSize) return false;

    if (fclose(filePtr) != 0) return false;

    m_targaData = new unsigned char[imageSize];
    if (!m_targaData) return false;

    int index = 0;
    int k = (width * height * 4) - (width * 4);
    for (int j = 0; j < height; ++j, k -= (width * 8))
    {
        for (int i = 0; i < width; ++i, k += 4, index += 4)
        {
            m_targaData[index + 0] = targaImage[k + 2]; // Red
            m_targaData[index + 1] = targaImage[k + 1]; // Green
            m_targaData[index + 2] = targaImage[k + 0]; // Blue
            m_targaData[index + 3] = targaImage[k + 3]; // Alpha
        }
    }

    delete[] targaImage;
    targaImage = nullptr;

    return true;
}
