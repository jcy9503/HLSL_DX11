#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "Modelclass.h"
#include "TextureShaderClass.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
= default;

GraphicsClass::GraphicsClass(const GraphicsClass&)
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
    char filePath[] = "../HLSL_DX11/Demo05/sample.tga";
    if (!m_model) return false;
    if (!m_model->Initialize(m_direct3D->GetDevice(), m_direct3D->GetDeviceContext(), filePath))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    m_textureShader = new TextureShaderClass;
    if (!m_textureShader) return false;
    if (!m_textureShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

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

    if (m_textureShader)
    {
        m_textureShader->Shutdown();
        delete m_textureShader;
        m_textureShader = nullptr;
    }
}

bool GraphicsClass::Frame() const
{
    return Render();
}

bool GraphicsClass::Render() const
{
    m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetProjectionmatrix(projectionMatrix);

    m_model->Render(m_direct3D->GetDeviceContext());

    if (!m_textureShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix,
                                 projectionMatrix, m_model->GetTexture()))
        return false;

    m_direct3D->EndScene();

    return true;
}
