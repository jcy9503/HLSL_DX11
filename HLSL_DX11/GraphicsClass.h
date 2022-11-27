#pragma once

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class ColorShaderClass;

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
    bool Render() const;

private:
    D3DClass* m_Direct3D = nullptr;
    CameraClass* m_Camera = nullptr;
    ModelClass* m_Model = nullptr;
    ColorShaderClass* m_ColorShader = nullptr;
};