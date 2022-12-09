#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "ModelClass.h"
#include "TranslateShaderClass.h"
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

    constexpr char model[] = "../HLSL_DX11/Geometry/triangle.txt";
    WCHAR tex1[] = L"../HLSL_DX11/Texture/seafloor.dds";
    m_model = new ModelClass;
    if (!m_model) return false;
    if (!m_model->Initialize(m_direct3D->GetDevice(), model, tex1))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_translateShader = new TranslateShaderClass;
    if (!m_translateShader) return false;
    if (!m_translateShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the texture translation shader object.", L"Error", MB_OK | MB_ICONERROR);
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

    if (m_translateShader)
    {
        m_translateShader->Shutdown();
        delete m_translateShader;
        m_translateShader = nullptr;
    }
}


bool GraphicsClass::Frame(const int mouseX, const int mouseY, const int fps, const int cpu, float frameTime, const float rotationY) const
{
    // if (!m_text->SetMousePosition(mouseX, mouseY, m_direct3D->GetDeviceContext()))
    //     return false;
    //
    // if(!m_bitmap->SetMousePosition(m_direct3D->GetDeviceContext(), mouseX, mouseY))
    //     return false;

    // if (!m_text->SetFps(m_direct3D->GetDeviceContext(), fps))
    //     return false;
    //
    // if (!m_text->SetCpu(m_direct3D->GetDeviceContext(), cpu))
    //     return false;

    m_camera->SetPosition(0.0f, 0.0f, -5.0f);
    m_camera->SetRotation(0.0f, rotationY, 0.0f);

    return true;
}


bool GraphicsClass::Render()
{
    m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};
    XMMATRIX orthoMatrix{};

    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetWorldMatrix(worldMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);
    m_direct3D->GetOrthoMatrix(orthoMatrix);

    static float rotation = 0.0f;
    rotation += static_cast<float>(XM_PI) * 0.0025f;
    if (rotation > 360.0f) rotation -= 360.0f;
    worldMatrix = XMMatrixRotationY(rotation);

    m_model->Render(m_direct3D->GetDeviceContext());

    static float textureTranslation = 0.0f;
    textureTranslation += 0.005f;
    if (textureTranslation > 1.0f) textureTranslation -= 1.0f;

    if (!m_translateShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_model->GetTextureArray(), textureTranslation))
        return false;

    m_direct3D->EndScene();

    return true;
}
