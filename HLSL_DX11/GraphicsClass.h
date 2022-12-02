#pragma once

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = false;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class TextClass;
class BitmapClass;
class TextureShaderClass;

class GraphicsClass
{
public:
    GraphicsClass();
    GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame(int, int, int, int, float) const;
    bool Render() const;
    bool InputKey(char) const;

private:
    D3DClass* m_direct3D = nullptr;
    CameraClass* m_camera = nullptr;
    TextClass* m_text = nullptr;
    BitmapClass* m_bitmap = nullptr;
    TextureShaderClass* m_textureShader = nullptr;
};
