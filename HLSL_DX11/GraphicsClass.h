#pragma once

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class TextureShaderClass;
class TransparentShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, int, int, float, float) const;
	bool Render();
	// static bool InputKey(char);

private:
	D3DClass* m_direct3D = nullptr;
	CameraClass* m_camera = nullptr;
	ModelClass* m_model1 = nullptr;
	ModelClass* m_model2 = nullptr;
	TextureShaderClass* m_textureShader = nullptr;
	TransparentShaderClass* m_transparentShader = nullptr;
};