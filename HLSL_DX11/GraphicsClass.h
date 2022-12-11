#pragma once

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class TextureShaderClass;
class RenderTextureClass;
class ReflectionShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, int, int, float, float, float) const;
	bool Render() const;
	// static bool InputKey(char);

private:
	bool RenderTexture() const;
	bool RenderScene() const;

private:
	D3DClass* m_direct3D = nullptr;
	CameraClass* m_camera = nullptr;
	ModelClass* m_model = nullptr;
	ModelClass* m_modelFloor = nullptr;
	TextureShaderClass* m_textureShader = nullptr;
	RenderTextureClass* m_renderTexture = nullptr;
	ReflectionShaderClass* m_reflectionShader = nullptr;
};