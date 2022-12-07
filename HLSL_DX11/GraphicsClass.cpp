#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "TextClass.h"
#include "BitmapClass.h"
#include "TextureShaderClass.h"
#include "ModelClass.h"
#include "ModelListClass.h"
#include "LightClass.h"
#include "LightShaderClass.h"
#include "FrustumClass.h"
#include "LightMapShaderClass.h"
#include "MultiTextureShaderClass.h"
#include "BumpmapShaderClass.h"
#include "AlphaMapShaderClass.h"
#include "SpecMapShaderClass.h"
#include "RenderTextureClass.h"
#include "DebugWindowClass.h"
#include "GraphicsClass.h"

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
    m_camera->SetPosition(0.0f, 0.0f, -1.0f);

    XMMATRIX baseViewMatrix{};
    m_camera->Render();
    m_camera->GetViewMatrix(baseViewMatrix);

    // m_text = new TextClass;
    // if (!m_text) return false;
    // if (!m_text->Initialize(m_direct3D->GetDevice(), m_direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix))
    // {
    //     MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    m_textureShader = new TextureShaderClass;
    if (!m_textureShader) return false;
    if (!m_textureShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // WCHAR cursor[] = L"../HLSL_DX11/Demo10/sample.dds";
    // m_bitmap = new BitmapClass;
    // if (!m_bitmap) return false;
    // if (!m_bitmap->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight, cursor, 50, 50))
    // {
    //     MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    constexpr char model[] = "../HLSL_DX11/Demo22/cube.txt";
    WCHAR tex1[] = L"../HLSL_DX11/Demo22/seafloor.dds";
    // WCHAR tex2[] = L"../HLSL_DX11/SpecMapShader/bump02.dds";
    // WCHAR tex3[] = L"../HLSL_DX11/SpecMapShader/spec02.dds";
    m_model = new ModelClass;
    if (!m_model) return false;
    if (!m_model->Initialize(m_direct3D->GetDevice(), model, tex1))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_bumpmapShader = new BumpmapShaderClass;
    // if (!m_bumpmapShader) return false;
    // if (!m_bumpmapShader->Initialize(m_direct3D->GetDevice(), hwnd))
    // {
    //     MessageBox(hwnd, L"Could not initialize the bumpmap shader object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    // m_alphaMapShader = new AlphaMapShaderClass;
    // if (!m_alphaMapShader) return false;
    // if (!m_alphaMapShader->Initialize(m_direct3D->GetDevice(), hwnd))
    // {
    //     MessageBox(hwnd, L"Could not initialize the alpha map shader object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    // m_lightMapShader = new LightMapShaderClass;
    // if (!m_lightMapShader) return false;
    // if (!m_lightMapShader->Initialize(m_direct3D->GetDevice(), hwnd))
    // {
    //     MessageBox(hwnd, L"Coult not initialize the light map shader object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    // m_multiTextureShader = new MultiTextureShaderClass;
    // if (!m_multiTextureShader) return false;
    // if (!m_multiTextureShader->Initialize(m_direct3D->GetDevice(), hwnd))
    // {
    //     MessageBox(hwnd, L"Could not initialize the multi-texture shader object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    m_lightShader = new LightShaderClass;
    if (!m_lightShader) return false;
    if (!m_lightShader->Initialize(m_direct3D->GetDevice(), hwnd, 0))
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_light = new LightClass;
    // if (!m_light) return false;
    // m_light->SetDirection(0.0f, 0.0f, 1.0f);

    // m_modelList = new ModelListClass;
    // if (!m_modelList) return false;
    // if (!m_modelList->Initialize(25))
    // {
    //     MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    // m_frustum = new FrustumClass;
    // if (!m_frustum) return false;

    // m_model = new ModelClass;
    // constexpr char filePath[] = "../HLSL_DX11/Demo07/cube.txt";
    // if (!m_model) return false;
    // if (!m_model->Initialize(m_direct3D->GetDevice(), filePath, L"../HLSL_DX11/Demo07/sample.dds"))
    // {
    //     MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }
    //
    // m_lightShader = new LightShaderClass;
    // if (!m_lightShader) return false;
    // if (!m_lightShader->Initialize(m_direct3D->GetDevice(), hwnd))
    // {
    //     MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }
    //

    // m_specMapShader = new SpecMapShaderClass;
    // if (!m_specMapShader) return false;
    // if (!m_specMapShader->Initialize(m_direct3D->GetDevice(), hwnd))
    // {
    //     MessageBox(hwnd, L"Could not initialize the specular map shader object.", L"Error", MB_OK | MB_ICONERROR);
    //     return false;
    // }

    m_light = new LightClass;
    if (!m_light) return false;

    m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->SetDirection(0.0f, 0.0f, 1.0f);
    m_light->SetSpecularcolor(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->SetSpecularPower(16.0f);

    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture) return false;
    if (!m_RenderTexture->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight))
        return false;

    m_debugWindow = new DebugWindowClass;
    if (!m_debugWindow) return false;
    if (!m_debugWindow->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight, 133, 100))
    {
        MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK | MB_ICONERROR);
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

    // if (m_text)
    // {
    //     m_text->Shutdown();
    //     delete m_text;
    //     m_text = nullptr;
    // }
    //
    if (m_textureShader)
    {
        m_textureShader->Shutdown();
        delete m_textureShader;
        m_textureShader = nullptr;
    }
    //
    // if (m_bitmap)
    // {
    //     m_bitmap->Shutdown();
    //     delete m_bitmap;
    //     m_bitmap = nullptr;
    // }

    if (m_model)
    {
        m_model->Shutdown();
        delete m_model;
        m_model = nullptr;
    }

    // if (m_modelList)
    // {
    //     m_modelList->Shutdown();
    //     delete m_modelList;
    //     m_modelList = nullptr;
    // }

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
    //
    // if (m_frustum)
    // {
    //     delete m_frustum;
    //     m_frustum = nullptr;
    // }
    //
    // if (m_multiTextureShader)
    // {
    //     m_multiTextureShader->Shutdown();
    //     delete m_multiTextureShader;
    //     m_multiTextureShader = nullptr;
    // }
    //
    // if (m_lightMapShader)
    // {
    //     m_lightMapShader->Shutdown();
    //     delete m_lightMapShader;
    //     m_lightMapShader = nullptr;
    // }
    //
    // if (m_alphaMapShader)
    // {
    //     m_alphaMapShader->Shutdown();
    //     delete m_alphaMapShader;
    //     m_alphaMapShader = nullptr;
    // }
    //
    // if (m_bumpmapShader)
    // {
    //     m_bumpmapShader->Shutdown();
    //     delete m_bumpmapShader;
    //     m_bumpmapShader = nullptr;
    // }

    // if (m_specMapShader)
    // {
    //     m_specMapShader->Shutdown();
    //     delete m_specMapShader;
    //     m_specMapShader = nullptr;
    // }

    if (m_RenderTexture)
    {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = nullptr;
    }

    if (m_debugWindow)
    {
        m_debugWindow->Shutdown();
        delete m_debugWindow;
        m_debugWindow = nullptr;
    }
}

bool GraphicsClass::Frame(const int mouseX, const int mouseY, const int fps, const int cpu, float frameTime, const float rotationY) const
{
    // static float rotation = 0.0f;
    //
    // rotation += static_cast<float>(XM_PI) * 0.005f;
    // if (rotation > 360.0f) rotation -= 360.0f;
    //
    // return Render(rotation);

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
    if (!RenderToTexture()) return false;
    // Scene을 그리기 위해 Buffer 지우기
    m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
    if (!RenderScene()) return false;

    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};
    XMMATRIX orthoMatrix{};

    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetWorldMatrix(worldMatrix);
    m_direct3D->GetProjectionmatrix(projectionMatrix);
    m_direct3D->GetOrthoMatrix(orthoMatrix);

    // float positionX = 0.0f;
    // float positionY = 0.0f;
    // float positionZ = 0.0f;
    // XMFLOAT4 color{};
    //
    // m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    // m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    // m_light->SetDirection(1.0f, -0.5f, 0.5f);
    // m_light->SetSpecularcolor(1.0f, 1.0f, 1.0f, 1.0f);
    // m_light->SetSpecularPower(16.0f);
    //
    // m_frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
    //
    // const int modelCount = m_modelList->GetModelCount();
    //
    // int renderCount = 0;
    //
    // for (int index = 0; index < modelCount; ++index)
    // {
    //     constexpr float radius = 1.0f;
    //     m_modelList->GetData(index, positionX, positionY, positionZ, color);
    //
    //     if (m_frustum->CheckCube(positionX, positionY, positionZ, radius))
    //     {
    //         worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
    //
    //         m_model->Render(m_direct3D->GetDeviceContext());
    //
    //         m_lightShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
    //             m_model->GetTexture(), m_light->GetDirection(), m_light->GetAmbientColor(), color,
    //             m_camera->GetPosition(), m_light->GetSpecularColor(), m_light->GetSpecularPower());
    //
    //         m_direct3D->GetWorldMatrix(worldMatrix);
    //
    //         ++renderCount;
    //     }
    // }
    //
    // if (!m_text->SetRenderCount(m_direct3D->GetDeviceContext(), renderCount))
    //     return false;

    m_model->Render(m_direct3D->GetDeviceContext());

    // m_specMapShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
    //     m_model->GetTextureArray(), m_light->GetDirection(), m_light->GetDiffuseColor(), m_camera->GetPosition(),
    //     m_light->GetSpecularColor(), m_light->GetSpecularPower());

    // m_bumpmapShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
    //     m_model->GetTextureArray(), m_light->GetDirection(), m_light->GetDiffuseColor());

    // m_multiTextureShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
    //     m_model->GetTextureArray());

    // m_lightMapShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
    //     m_model->GetTextureArray());

    // m_alphaMapShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
    //     m_model->GetTextureArray());

    // 2D 렌더링을 위해 Z Buffer 끄기
    m_direct3D->TurnZBufferOff();

    if (!m_debugWindow->Render(m_direct3D->GetDeviceContext(), 0, 0))
        return false;

    if (!m_textureShader->Render(m_direct3D->GetDeviceContext(), m_debugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
        m_RenderTexture->GetShaderResourceView()))
        return false;

    // 텍스트 렌더링을 위해 알파 블렌딩 켜기
    // m_direct3D->TurnOnAlphaBlending();

    // m_bitmap->Render(m_direct3D->GetDeviceContext());
    // if (!m_textureShader->Render(m_direct3D->GetDeviceContext(), m_bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
    //     m_bitmap->GetTexture()))
    //     return false;

    // 텍스트 문자열 렌더링
    // if (!m_text->Render(m_direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
    //     return false;
    //
    // m_direct3D->TurnOffAlphaBlending();
    m_direct3D->TurnZBufferOn();

    // m_direct3D->TurnZBufferOff();
    //
    // if (!m_bitmap->Render(m_direct3D->GetDeviceContext(), 50, 50)) return false;
    // if (!m_textureShader->Render(m_direct3D->GetDeviceContext(), m_bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
    //     m_bitmap->GetTexture()))
    //     return false;
    //
    // m_direct3D->TurnZBufferOn();

    // worldMatrix = XMMatrixRotationY(rotation);

    // m_model->Render(m_direct3D->GetDeviceContext());
    //
    // if (!m_lightShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix,
    //                            projectionMatrix, m_model->GetTexture(), m_light->GetDirection(),
    //                            m_light->GetAmbientColor(), m_light->GetDiffuseColor(), m_camera->GetPosition(),
    //                            m_light->GetSpecularColor(), m_light->GetSpecularPower()))
    //     return false;

    // 버퍼의 내용 화면에 출력
    m_direct3D->EndScene();

    return true;
}

bool GraphicsClass::InputKey(const char input) const
{
    // if (!m_text->KeyInput(m_direct3D->GetDeviceContext(), input))
    //     return false;

    return true;
}

bool GraphicsClass::RenderToTexture()
{
    m_RenderTexture->SetRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView());
    m_RenderTexture->ClearRenderTarget(m_direct3D->GetDeviceContext(), m_direct3D->GetDepthStencilView(), 0.25f, 0.25f, 0.25f, 1.0f);

    if (!RenderScene())
        return false;

    m_direct3D->SetBackBufferRenderTarget();

    return true;
}

bool GraphicsClass::RenderScene()
{
    m_camera->Render();

    XMMATRIX worldMatrix{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};
    XMMATRIX orthoMatrix{};

    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetWorldMatrix(worldMatrix);
    m_direct3D->GetProjectionmatrix(projectionMatrix);
    m_direct3D->GetOrthoMatrix(orthoMatrix);

    static float rotation = 0.0f;
    rotation += static_cast<float>(XM_PI) * 0.0025f;
    if (rotation > 360.0f) rotation -= 360.0f;

    worldMatrix = XMMatrixRotationY(rotation);
    m_model->Render(m_direct3D->GetDeviceContext());

    return m_lightShader->Render(m_direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_model->GetTextureArray(), m_light->GetDirection(), m_light->GetAmbientColor(), m_light->GetDiffuseColor(), m_camera->GetPosition(),
        m_light->GetSpecularColor(), m_light->GetSpecularPower());
}
