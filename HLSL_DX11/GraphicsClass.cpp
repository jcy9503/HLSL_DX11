#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "ModelClass.h"
#include "LightClass.h"
#include "RenderTextureClass.h"
#include "LightShaderClass.h"
#include "RefractionShaderClass.h"
#include "WaterShaderClass.h"
#include "GraphicsClass.h"


GraphicsClass::GraphicsClass()
= default;

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
= default;


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    m_direct3D = new D3DClass;
    if (!m_direct3D) return false;
    if (!m_direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_camera = new CameraClass;
    if (!m_camera) return false;

    constexpr char modelGround[] = "../HLSL_DX11/Geometry/ground.txt";
    WCHAR texGround[] = L"../HLSL_DX11/Texture/ground01.dds";
    m_modelGround = new ModelClass;
    if (!m_modelGround) return false;
    if (!m_modelGround->Initialize(m_direct3D->GetDevice(), modelGround, texGround))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    constexpr char modelWall[] = "../HLSL_DX11/Geometry/wall.txt";
    WCHAR texWall[] = L"../HLSL_DX11/Texture/wall01.dds";
    m_modelWall = new ModelClass;
    if (!m_modelWall) return false;
    if (!m_modelWall->Initialize(m_direct3D->GetDevice(), modelWall, texWall))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    constexpr char modelBath[] = "../HLSL_DX11/Geometry/bath.txt";
    WCHAR texBath[] = L"../HLSL_DX11/Texture/marble01.dds";
    m_modelBath = new ModelClass;
    if (!m_modelBath) return false;
    if (!m_modelBath->Initialize(m_direct3D->GetDevice(), modelBath, texBath))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    constexpr char modelWater[] = "../HLSL_DX11/Geometry/water.txt";
    WCHAR texWater[] = L"../HLSL_DX11/Texture/water01.dds";
    m_modelWater = new ModelClass;
    if (!m_modelWater) return false;
    if (!m_modelWater->Initialize(m_direct3D->GetDevice(), modelWater, texWater))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_light = new LightClass;
    if (!m_light) return false;
    m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->SetDirection(0.0f, -1.0f, 0.5f);

    m_reflectionTexture = new RenderTextureClass;
    if (!m_reflectionTexture) return false;
    if (!m_reflectionTexture->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight))
    {
        MessageBox(hwnd, L"Could not initialize the reflection render to texture object", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_refractionTexture = new RenderTextureClass;
    if (!m_refractionTexture) return false;
    if (!m_refractionTexture->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight))
    {
        MessageBox(hwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_lightShader = new LightShaderClass;
    if (!m_lightShader) return false;
    if (!m_lightShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_refractionShader = new RefractionShaderClass;
    if (!m_refractionShader) return false;
    if (!m_refractionShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_waterShader = new WaterShaderClass;
    if (!m_waterShader) return false;
    if (!m_waterShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the water shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_waterHeight = 2.75f;
    m_waterTranslation = 0.0f;

    return true;
}


void GraphicsClass::Shutdown()
{
    if (m_waterShader)
    {
        m_waterShader->Shutdown();
        delete m_waterShader;
        m_waterShader = nullptr;
    }

    if (m_refractionShader)
    {
        m_refractionShader->Shutdown();
        delete m_refractionShader;
        m_refractionShader = nullptr;
    }

    if (m_lightShader)
    {
        m_lightShader->Shutdown();
        delete m_lightShader;
        m_lightShader = nullptr;
    }

    if (m_reflectionTexture)
    {
        m_reflectionTexture->Shutdown();
        delete m_reflectionTexture;
        m_reflectionTexture = nullptr;
    }

    if (m_refractionTexture)
    {
        m_refractionTexture->Shutdown();
        delete m_refractionTexture;
        m_refractionTexture = nullptr;
    }

    if (m_light)
    {
        delete m_light;
        m_light = nullptr;
    }

    if (m_modelWater)
    {
        m_modelWater->Shutdown();
        delete m_modelWater;
        m_modelWater = nullptr;
    }

    if (m_modelBath)
    {
        m_modelBath->Shutdown();
        delete m_modelBath;
        m_modelBath = nullptr;
    }

    if (m_modelWall)
    {
        m_modelWall->Shutdown();
        delete m_modelWall;
        m_modelWall = nullptr;
    }

    if (m_modelGround)
    {
        m_modelGround->Shutdown();
        delete m_modelGround;
        m_modelGround = nullptr;
    }

    if (m_direct3D)
    {
        m_direct3D->Shutdown();
        m_direct3D = nullptr;
    }

    if (m_camera)
    {
        delete m_camera;
        m_camera = nullptr;
    }
}


bool GraphicsClass::Frame(const int mouseX, const int mouseY, const int fps, const int cpu, float frameTime, const float rotationX,
                          const float rotationY)
{
    m_waterTranslation += 0.001f;
    if (m_waterTranslation > 1.0f) m_waterTranslation -= 1.0f;

    m_camera->SetPosition(-11.0f, 8.0f, -10.0f);
    m_camera->SetRotation(rotationX, rotationY, 0.0f);

    return true;
}


bool GraphicsClass::Render()
{
    static float rotation = 0.0f;

    bool result = RenderRefractionToTexture();
    if (!result) return false;

    result = RenderReflactionToTexture();
    if (!result) return false;

    result = RenderScene();
    if (!result) return false;

    return true;
}

bool GraphicsClass::RenderRefractionToTexture()
{
    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};
    XMFLOAT4 clipPlane(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);

    m_refractionTexture->SetRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView());
    m_refractionTexture->ClearRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

    m_modelBath->Render(m_direct3D->GetDeviceContext());

    if (!m_refractionShader->Render(m_direct3D->GetDeviceContext(), m_modelBath->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_modelBath->GetTextureArray(), m_light->GetDirection(), m_light->GetAmbientColor(), m_light->GetDiffuseColor(), clipPlane))
        return false;

    m_direct3D->SetBackBufferRenderTarget();

    return true;
}

bool GraphicsClass::RenderReflactionToTexture()
{
    XMMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;

    m_reflectionTexture->SetRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView());
    m_reflectionTexture->ClearRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->RenderReflection(m_waterHeight);

    reflectionViewMatrix = m_camera->GetReflectionViewMatrix();

    m_direct3D->GetWorldMatrix(worldMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

    m_modelWall->Render(m_direct3D->GetDeviceContext());

    if (!m_lightShader->Render(m_direct3D->GetDeviceContext(), m_modelWall->GetIndexCount(), worldMatrix, reflectionViewMatrix,
        projectionMatrix, m_modelWall->GetTextureArray(), m_light->GetDirection(), m_light->GetAmbientColor(), m_light->GetDiffuseColor()))
        return false;

    m_direct3D->SetBackBufferRenderTarget();

    return true;
}

bool GraphicsClass::RenderScene()
{
    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};
    XMMATRIX reflectionMatrix{};

    m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

    m_modelGround->Render(m_direct3D->GetDeviceContext());

    if (!m_lightShader->Render(m_direct3D->GetDeviceContext(), m_modelGround->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_modelGround->GetTextureArray(), m_light->GetDirection(), m_light->GetAmbientColor(), m_light->GetDiffuseColor()))
        return false;

    m_direct3D->GetWorldMatrix(worldMatrix);

    worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

    m_modelWall->Render(m_direct3D->GetDeviceContext());

    if (!m_lightShader->Render(m_direct3D->GetDeviceContext(), m_modelWall->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_modelWall->GetTextureArray(), m_light->GetDirection(), m_light->GetAmbientColor(), m_light->GetDiffuseColor()))
        return false;

    m_direct3D->GetWorldMatrix(worldMatrix);

    worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

    m_modelBath->Render(m_direct3D->GetDeviceContext());

    if (!m_lightShader->Render(m_direct3D->GetDeviceContext(), m_modelBath->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_modelBath->GetTextureArray(), m_light->GetDirection(), m_light->GetAmbientColor(), m_light->GetDiffuseColor()))
        return false;

    m_direct3D->GetWorldMatrix(worldMatrix);

    reflectionMatrix = m_camera->GetReflectionViewMatrix();

    worldMatrix = XMMatrixTranslation(0.0f, m_waterHeight, 0.0f);

    m_modelWater->Render(m_direct3D->GetDeviceContext());

    if (!m_waterShader->Render(m_direct3D->GetDeviceContext(), m_modelWater->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        reflectionMatrix, m_reflectionTexture->GetShaderResourceView(), m_refractionTexture->GetShaderResourceView(),
        m_modelWater->GetTextureArray(), m_waterTranslation, 0.01f))
        return false;

    m_direct3D->EndScene();

    return true;
}
