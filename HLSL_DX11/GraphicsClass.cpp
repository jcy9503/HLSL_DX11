#include "Stdafx.h"
#include "D3DClass.h"
#include "Cameraclass.h"
#include "TextClass.h"
#include "BitmapClass.h"
#include "TextureShaderClass.h"
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
    // Direct3D 객체 생성/초기화
    m_direct3D = new D3DClass;
    if (!m_direct3D) return false;
    if (!m_direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    // 카메라 객체 생성/위치 설정
    m_camera = new CameraClass;
    if (!m_camera) return false;
    m_camera->SetPosition(0.0f, 0.0f, -10.0f);

    XMMATRIX baseViewMatrix;
    m_camera->Render();
    m_camera->GetViewMatrix(baseViewMatrix);

    // 텍스트 객체 생성
    m_text = new TextClass;
    if (!m_text) return false;
    if (!m_text->Initialize(m_direct3D->GetDevice(), m_direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix))
    {
        MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_textureShader = new TextureShaderClass;
    if (!m_textureShader) return false;
    if (!m_textureShader->Initialize(m_direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    WCHAR cursor[] = L"../HLSL_DX11/Demo10/sample.dds";
    m_bitmap = new BitmapClass;
    if (!m_bitmap) return false;
    if (!m_bitmap->Initialize(m_direct3D->GetDevice(), screenWidth, screenHeight, cursor, 50, 50))
    {
        MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

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
    // m_light = new LightClass;
    // if (!m_light) return false;
    //
    // m_light->SetAmbientColor(0.15f, 0.15f, 0.0f, 1.0f);
    // m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    // m_light->SetDirection(1.0f, 0.0f, 1.0f);
    // m_light->SetSpecularcolor(1.0f, 1.0f, 1.0f, 1.0f);
    // m_light->SetSpecularPower(32.0f);

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

    if (m_text)
    {
        m_text->Shutdown();
        delete m_text;
        m_text = nullptr;
    }

    if (m_textureShader)
    {
        m_textureShader->Shutdown();
        delete m_textureShader;
        m_textureShader = nullptr;
    }

    if (m_bitmap)
    {
        m_bitmap->Shutdown();
        delete m_bitmap;
        m_bitmap = nullptr;
    }

    // if (m_model)
    // {
    //     m_model->Shutdown();
    //     delete m_model;
    //     m_model = nullptr;
    // }
    //
    // if (m_light)
    // {
    //     delete m_light;
    //     m_light = nullptr;
    // }
    //
    // if (m_lightShader)
    // {
    //     m_lightShader->Shutdown();
    //     delete m_lightShader;
    //     m_lightShader = nullptr;
    // }
}

bool GraphicsClass::Frame(const int mouseX, const int mouseY) const
{
    // static float rotation = 0.0f;
    //
    // rotation += static_cast<float>(XM_PI) * 0.005f;
    // if (rotation > 360.0f) rotation -= 360.0f;
    //
    // return Render(rotation);

    if (!m_text->SetMousePosition(mouseX, mouseY, m_direct3D->GetDeviceContext()))
        return false;

    if(!m_bitmap->SetMousePosition(m_direct3D->GetDeviceContext(), mouseX, mouseY))
        return false;

    return true;
}

bool GraphicsClass::Render() const
{
    // Scene을 그리기 위해 Buffer 지우기
    m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // 카메라의 위치에 따라 뷰 행렬 생성
    m_camera->Render();

    // 카메라 및 D3D 객체에서 월드, 뷰, 투영 행렬 획득
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

    m_camera->GetViewMatrix(viewMatrix);
    m_direct3D->GetWorldMatrix(worldMatrix);
    m_direct3D->GetProjectionmatrix(projectionMatrix);
    m_direct3D->GetOrthoMatrix(orthoMatrix);

    // 2D 렌더링을 위해 Z Buffer 끄기
    m_direct3D->TurnZBufferOff();

    // 텍스트 렌더링을 위해 알파 블렌딩 켜기
    m_direct3D->TurnOnAlphaBlending();

    m_bitmap->Render(m_direct3D->GetDeviceContext());
    if (!m_textureShader->Render(m_direct3D->GetDeviceContext(), m_bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
        m_bitmap->GetTexture()))
            return false;

    // 텍스트 문자열 렌더링
    if (!m_text->Render(m_direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
        return false;

    m_direct3D->TurnOffAlphaBlending();
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
    if(!m_text->KeyInput(m_direct3D->GetDeviceContext(), input))
        return false;

    return true;
}
