#pragma once
#include "BitmapClass.h"
#include "TextureShaderClass.h"

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class LightClass;
class LightShaderClass;

class GraphicsClass
{
public:
    GraphicsClass();
    GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame() const;

private:
    bool Render(float) const;

private:
    D3DClass* m_direct3D = nullptr;
    CameraClass* m_camera = nullptr;
    TextureShaderClass* m_textureShader = nullptr;
    BitmapClass* m_bitmap = nullptr;
};