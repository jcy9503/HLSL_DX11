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
class BitmapClass;
class FadeShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, int, int, float, float, float);
	bool Render();
	// static bool InputKey(char);

private:
	bool RenderToTexture(float);
	bool RenderFadingScene();
	bool RenderNormalScene(float);

private:
	D3DClass* m_direct3D = nullptr;
	CameraClass* m_camera = nullptr;
	ModelClass* m_model = nullptr;
	TextureShaderClass* m_textureShader = nullptr;
	RenderTextureClass* m_renderTexture = nullptr;
	BitmapClass* m_bitmap = nullptr;
	FadeShaderClass* m_fadeShader = nullptr;
	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	bool m_fadeDone = false;
};