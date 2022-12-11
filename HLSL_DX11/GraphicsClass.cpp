#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "ModelClass.h"
#include "TextureShaderClass.h"
#include "RenderTextureClass.h"
#include "ReflectionShaderClass.h"
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

    constexpr char model1[] = "../HLSL_DX11/Geometry/cube.txt";
    WCHAR tex1[] = L"../HLSL_DX11/Texture/seafloor.dds";
    m_model = new ModelClass;
    if (!m_model) return false;
    if (!m_model->Initialize(m_direct3D->GetDevice(), model1, tex1))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_textureShader = new TextureShaderClass;
    if (!m_textureShader) return false;
    if (!m_textureShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_renderTexture = new RenderTextureClass;
    if (!m_renderTexture) return false;
    if (!m_renderTexture->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight))
        return false;

    constexpr char model2[] = "../HLSL_DX11/Geometry/floor.txt";
    WCHAR tex2[] = L"../HLSL_DX11/Texture/blue01.dds";
    m_modelFloor = new ModelClass;
    if (!m_modelFloor) return false;
    if (!m_modelFloor->Initialize(m_direct3D->GetDevice(), model2, tex2))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_reflectionShader = new ReflectionShaderClass;
    if (!m_reflectionShader) return false;
    if (!m_reflectionShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the reflection shader object.", L"Error", MB_OK | MB_ICONERROR);
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

    if (m_modelFloor)
    {
        m_modelFloor->Shutdown();
        delete m_modelFloor;
        m_modelFloor = nullptr;
    }

    if (m_reflectionShader)
    {
        m_reflectionShader->Shutdown();
        delete m_reflectionShader;
        m_reflectionShader = nullptr;
    }

    if (m_textureShader)
    {
        m_textureShader->Shutdown();
        delete m_textureShader;
        m_textureShader = nullptr;
    }

    if (m_renderTexture)
    {
        m_renderTexture->Shutdown();
        delete m_renderTexture;
        m_renderTexture = nullptr;
    }
}


bool GraphicsClass::Frame(const int mouseX, const int mouseY, const int fps, const int cpu, float frameTime, const float rotationX, const float rotationY) const
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

    m_camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_camera->SetRotation(rotationX, rotationY, 0.0f);

    return true;
}


bool GraphicsClass::Render() const
{
    if (!RenderTexture())
        return false;

    if (!RenderScene())
        return false;

    return true;
}

bool GraphicsClass::RenderTexture() const
{
    XMMATRIX worldMatrix{};
    XMMATRIX projectionMatrix{};

    m_renderTexture->SetRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView());

    m_renderTexture->ClearRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->RenderReflection(-1.5f);

    const XMMATRIX reflectionMatrix = m_camera->GetReflectionViewMatrix();

    m_direct3D->GetWorldMatrix(worldMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);

    static float rotation = 0.0f;
    rotation += static_cast<float>(XM_PI) * 0.005f;
    if (rotation > 360.0f) rotation -= 360.0f;
    worldMatrix = XMMatrixRotationY(rotation);

    m_model->Render(m_direct3D->GetDeviceContext());

    m_textureShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, reflectionMatrix, projectionMatrix,
        m_model->GetTextureArray());

    m_direct3D->SetBackBufferRenderTarget();

    return true;
}

bool GraphicsClass::RenderScene() const
{
    m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};
    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);

    static float rotation = 0.0f;
    rotation += static_cast<float>(XM_PI) * 0.005f;
    if (rotation > 360.0f) rotation -= 360.0f;
    worldMatrix = XMMatrixRotationY(rotation);

    m_model->Render(m_direct3D->GetDeviceContext());

    if (!m_textureShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_model->GetTextureArray()))
        return false;

    m_direct3D->GetWorldMatrix(worldMatrix);
    worldMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f);
    // worldMatrix = XMMatrixRotationY(90);

    const XMMATRIX reflectionMatrix = m_camera->GetReflectionViewMatrix();

    m_modelFloor->Render(m_direct3D->GetDeviceContext());

    if (!m_reflectionShader->Render(m_direct3D->GetDeviceContext(), m_modelFloor->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_modelFloor->GetTextureArray(), m_renderTexture->GetShaderResourceView(), reflectionMatrix))
        return false;

    m_direct3D->EndScene();

    return true;
}
