#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "ModelClass.h"
#include "TextureShaderClass.h"
#include "RenderTextureClass.h"
#include "BitmapClass.h"
#include "FadeShaderClass.h"
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

    m_bitmap = new BitmapClass;
    if (!m_bitmap) return false;
    if (!m_bitmap->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight))
    {
        MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_fadeInTime = 1000.0f;
    m_accumulatedTime = 0;
    m_fadePercentage = 0;
    m_fadeDone = false;
    m_fadeShader = new FadeShaderClass;
    if (!m_fadeShader) return false;
    if (!m_fadeShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK | MB_ICONERROR);
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

    if (m_renderTexture)
    {
        m_renderTexture->Shutdown();
        delete m_renderTexture;
        m_renderTexture = nullptr;
    }

    if (m_fadeShader)
    {
        m_fadeShader->Shutdown();
        delete m_fadeShader;
        m_fadeShader = nullptr;
    }

    if (m_bitmap)
    {
        m_bitmap->Shutdown();
        delete m_bitmap;
        m_bitmap = nullptr;
    }
}


bool GraphicsClass::Frame(const int mouseX, const int mouseY, const int fps, const int cpu, float frameTime, const float rotationX,
                          const float rotationY)
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

    if (!m_fadeDone)
    {
        m_accumulatedTime += frameTime;
        if (m_accumulatedTime < m_fadeInTime)
            m_fadePercentage = m_accumulatedTime / m_fadeInTime;
        else
        {
            m_fadeDone = true;
            m_fadePercentage = 1.0f;
        }
    }
    else
    {
        m_accumulatedTime -= frameTime;
        if(m_accumulatedTime > 0)
            m_fadePercentage = m_accumulatedTime / m_fadeInTime;
        else
        {
            m_fadeDone = false;
            m_fadePercentage = 0.0f;
        }
    }

    m_camera->SetPosition(0.0f, 0.0f, -6.0f);
    m_camera->SetRotation(rotationX, rotationY, 0.0f);

    return true;
}


bool GraphicsClass::Render()
{
    static float rotation = 0.0f;
    bool result;

    rotation += static_cast<float>(XM_PI) * 0.005f;
    if (rotation > 360.0f) rotation -= 360.0f;

    if (m_fadeDone)
    {
        result = RenderToTexture(rotation);
        if(!result) return false;
        
        result = RenderFadingScene();
        if(!result) return false;
    }
    else
    {
        result = RenderToTexture(rotation);
        if (!result) return false;

        result = RenderFadingScene();
        if (!result) return false;
    }

    return true;
}

bool GraphicsClass::RenderToTexture(float rotation)
{
    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};

    m_renderTexture->SetRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView());

    m_renderTexture->ClearRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixRotationY(rotation);

    m_model->Render(m_direct3D->GetDeviceContext());

    m_textureShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_model->GetTextureArray());

    m_direct3D->SetBackBufferRenderTarget();

    return true;
}

bool GraphicsClass::RenderFadingScene()
{
    m_direct3D->BeginScene(0.0f, 1.0f, 0.0f, 1.0f);

    m_camera->Render();

    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX orthoMatrix{};

    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetOrthoMatrix(orthoMatrix);

    m_direct3D->TurnZBufferOff();

    bool result = m_bitmap->Render(m_direct3D->GetDeviceContext(), 0, 0);
    if (!result) return false;

    result = m_fadeShader->Render(m_direct3D->GetDeviceContext(), m_bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
        m_renderTexture->GetShaderResourceView(), m_fadePercentage);
    if (!result) return false;

    m_direct3D->TurnZBufferOn();

    m_direct3D->EndScene();

    return true;
}

bool GraphicsClass::RenderNormalScene(float rotation)
{
    m_direct3D->BeginScene(0.0f, 0.0f, 1.0f, 1.0f);

    m_camera->Render();

    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};

    m_direct3D->GetWorldMatrix(worldMatrix);
    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixRotationY(rotation);

    m_model->Render(m_direct3D->GetDeviceContext());

    if (!m_textureShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_model->GetTextureArray()))
        return false;

    m_direct3D->EndScene();

    return true;
}
