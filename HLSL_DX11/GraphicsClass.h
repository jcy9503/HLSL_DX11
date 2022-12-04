#pragma once

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class TextClass;
class TextureShaderClass;
class MultiTextureShaderClass;
class BitmapClass;
class ModelClass;
class ModelListClass;
class LightClass;
class LightShaderClass;
class LightMapShaderClass;
class FrustumClass;
class AlphaMapShaderClass;

class GraphicsClass
{
public:
    GraphicsClass();
    GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame(int, int, int, int, float, float) const;
    bool Render() const;
    bool InputKey(char) const;

private:
    D3DClass* m_direct3D = nullptr;
    CameraClass* m_camera = nullptr;
    TextClass* m_text = nullptr;
    BitmapClass* m_bitmap = nullptr;
    TextureShaderClass* m_textureShader = nullptr;
    ModelClass* m_model = nullptr;
    ModelListClass* m_modelList = nullptr;
    LightClass* m_light = nullptr;
    LightShaderClass* m_lightShader = nullptr;
    FrustumClass* m_frustum = nullptr;
    MultiTextureShaderClass* m_multiTextureShader = nullptr;
    LightMapShaderClass* m_lightMapShader = nullptr;
    AlphaMapShaderClass* m_alphaMapShader = nullptr;
};
