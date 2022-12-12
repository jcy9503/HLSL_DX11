#pragma once

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class LightClass;
class RenderTextureClass;
class LightShaderClass;
class RefractionShaderClass;
class WaterShaderClass;

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
	bool RenderRefractionToTexture();
	bool RenderReflactionToTexture();
	bool RenderScene();

private:
	D3DClass* m_direct3D = nullptr;
	CameraClass* m_camera = nullptr;
	ModelClass* m_modelGround = nullptr;
	ModelClass* m_modelWall = nullptr;
	ModelClass* m_modelBath = nullptr;
	ModelClass* m_modelWater = nullptr;
	LightClass* m_light = nullptr;
	RenderTextureClass* m_reflectionTexture = nullptr;
	RenderTextureClass* m_refractionTexture = nullptr;
	LightShaderClass* m_lightShader = nullptr;
	RefractionShaderClass* m_refractionShader = nullptr;
	WaterShaderClass* m_waterShader = nullptr;
	float m_waterHeight = 0;
	float m_waterTranslation = 0;
};