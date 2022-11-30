#include "Stdafx.h"
#include "TextureClass.h"
#include "FontClass.h"

#include <fstream>

FontClass::FontClass()
= default;

FontClass::FontClass(const FontClass&)
{
}

FontClass::~FontClass()
= default;

bool FontClass::Initialize(ID3D11Device* device, const char* fontFilename, WCHAR* textureFilename)
{
    if (!LoadFontData(fontFilename)) return false;

    return LoadTexture(device, textureFilename);
}


void FontClass::Shutdown()
{
    ReleaseTexture();
    ReleaseFontData();
}

bool FontClass::LoadFontData(const char* filename)
{
    std::ifstream fin;
    char temp;

    m_font = new FontType[95]; // Font Spacing Buffer
    if (!m_font) return false; 

    fin.open(filename);
    if (fin.fail()) return false;

    for (int i = 0; i < 95; ++i) // 95 ASCII characters
    {
        fin.get(temp);
        while (temp != ' ') fin.get(temp);
        fin.get(temp);
        while (temp != ' ') fin.get(temp);

        fin >> m_font[i].left;
        fin >> m_font[i].right;
        fin >> m_font[i].size;
    }

    fin.close();

    return true;
}

void FontClass::ReleaseFontData()
{
    if(m_font)
    {
        delete[] m_font;
        m_font = nullptr;
    }
}

bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
    m_texture = new TextureClass;
    if (!m_texture) return false;

    return m_texture->Initialize(device, filename);
}

void FontClass::ReleaseTexture()
{
    if (m_texture)
    {
        m_texture->Shutdown();
        delete m_texture;
        m_texture = nullptr;
    }
}

ID3D11ShaderResourceView* FontClass::GetTexture() const
{
    return m_texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, const float drawY) const
{
    const auto vertexPtr = static_cast<VertexType*>(vertices);
    const int numLetters = static_cast<int>(strlen(sentence));
    int letter;
    
    int index = 0;

    for (int i = 0; i < numLetters; ++i)
    {
        letter = (static_cast<int>(sentence[i])) - 32;
        if (letter == 0)
            drawX = drawX + 3.0f; // ' '(space) -> move over 3 pixels.
        else
        {
            const float right = drawX + static_cast<float>(m_font[letter].size);
            const float left = drawX;
            const float top = drawY;
            const float bottom = drawY - 16.0f;
            // First Triangle in quad
            vertexPtr[index].position = XMFLOAT3(left, top, 0.0f); // Top left
            vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
            ++index;

            vertexPtr[index].position = XMFLOAT3(right, bottom, 0.0f); // Bottom Right
            vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
            ++index;

            vertexPtr[index].position = XMFLOAT3(left, bottom, 0.0f); // Bottom left
            vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 1.0f);
            ++index;

            // Second Triangle in quad
            vertexPtr[index].position = XMFLOAT3(left, top, 0.0f); // Top left
            vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
            ++index;

            vertexPtr[index].position = XMFLOAT3(right, top, 0.0f); // Top right
            vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 0.0f);
            ++index;

            vertexPtr[index].position = XMFLOAT3(right, bottom, 0.0f); // Bottom Right
            vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
            ++index;

            drawX += static_cast<float>(m_font[letter].size) + 1.0f; // 1.0f = spacing between letters.
        }
    }
}
