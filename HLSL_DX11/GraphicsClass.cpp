#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "Modelclass.h"
#include "LightClass.h"
#include "LightShaderClass.h"
#include "GraphicsClass.h"

#include "TextureShaderClass.h"

GraphicsClass::GraphicsClass()
= default;

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
= default;

bool GraphicsClass::Initialize(const int screenWidth, const int screenHeight, const HWND hwnd)
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
    m_camera->SetPosition(0.0f, 0.0f, -5.0f);

    m_model = new ModelClass;
    char filePath[] = "../HLSL_DX11/Demo07/cube.txt";
    if (!m_model) return false;
    if (!m_model->Initialize(m_direct3D->GetDevice(), filePath, L"../HLSL_DX11/Demo07/sample.dds"))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_lightShader = new LightShaderClass;
    if (!m_lightShader) return false;
    if (!m_lightShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_light = new LightClass;
    if (!m_light) return false;

    m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->SetDirection(0.0f, 0.0f, 1.0f);

    return true;
}

void GraphicsClass::Shutdown()
{
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

    if (m_model)
    {
        m_model->Shutdown();
        delete m_model;
        m_model = nullptr;
    }

    if (m_light)
    {
        delete m_light;
        m_light = nullptr;
    }

    if (m_lightShader)
    {
        m_lightShader->Shutdown();
        delete m_lightShader;
        m_lightShader = nullptr;
    }
}

bool GraphicsClass::Frame() const
{
    static float rotation = 0.0f;

    rotation += static_cast<float>(XM_PI) * 0.01f;
    if (rotation > 360.0f) rotation -= 360.0f;

    return Render(rotation);
}

bool GraphicsClass::Render(const float rotation) const
{
    m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetProjectionmatrix(projectionMatrix);

    worldMatrix = XMMatrixRotationY(rotation);

    m_model->Render(m_direct3D->GetDeviceContext());

    if (!m_lightShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix,
                               projectionMatrix, m_model->GetTexture(), m_light->GetDirection(),
                               m_light->GetDiffuseColor()))
        return false;

    m_direct3D->EndScene();

    return true;
}
