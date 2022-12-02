#include "Stdafx.h"
#include "InputClass.h"
#include "graphicsclass.h"
#include "SoundClass.h"
#include "FPSClass.h"
#include "CPUClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "SystemClass.h"

SystemClass::SystemClass()
= default;

SystemClass::SystemClass(const SystemClass& other): m_applicationName(nullptr), m_hinstance(nullptr), m_hwnd(nullptr)
{
}

SystemClass::~SystemClass()
= default;

bool SystemClass::Initialize()
{
    int screenWidth = 0;
    int screenHeight = 0;

    // ������ ���� �ʱ�ȭ
    InitializeWindows(screenWidth, screenHeight);

    // m_input : ���콺�� Ű���� �Է� ó�� Ŭ����
    m_input = new InputClass;
    if (!m_input) return false;
    if (!m_input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight))
    {
        MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_graphics : �׷��� ������ ó�� Ŭ����
    m_graphics = new GraphicsClass;
    if (!m_graphics) return false;
    if (!m_graphics->Initialize(screenWidth, screenHeight, m_hwnd))
    {
        MessageBox(m_hwnd, L"Could not initialize Graphics object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_sound : ���̷�Ʈ ���� Ŭ����
    m_sound = new SoundClass;
    if (!m_sound) return false;
    if (!m_sound->Initialize(m_hwnd))
    {
        MessageBox(m_hwnd, L"Could not initialize Direct sound object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_fps : ������ Ŭ����
    m_fps = new FpsClass;
    if (!m_fps) return false;
    m_fps->Initialize();

    // m_cpu : CPU ���� ���� Ŭ����
    m_cpu = new CpuClass;
    if (!m_cpu) return false;
    m_cpu->Initialize();

    // m_timer : Ÿ�̸� Ŭ����
    m_timer = new TimerClass;
    if (!m_timer) return false;
    if (!m_timer->Initialize())
    {
        MessageBox(m_hwnd, L"Could not initialize the timer object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_position = new PositionClass;
    if(!m_position) return false;

    return true;
}

void SystemClass::Shutdown()
{
    if (m_graphics)
    {
        m_graphics->Shutdown();
        delete m_graphics;
        m_graphics = nullptr;
    }

    if (m_input)
    {
        m_input->Shutdown();
        delete m_input;
        m_input = nullptr;
    }

    if (m_sound)
    {
        m_sound->Shutdown();
        delete m_sound;
        m_sound = nullptr;
    }

    if (m_fps)
    {
        delete m_fps;
        m_fps = nullptr;
    }

    if (m_cpu)
    {
        m_cpu->Shutdown();
        delete m_cpu;
        m_cpu = nullptr;
    }

    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }

    if(m_position)
    {
        delete m_position;
        m_position = nullptr;
    }

    ShutdownWindows();
}

void SystemClass::Run() const
{
    // �޽��� ����ü ����/�ʱ�ȭ
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // ����ڷκ��� ���� �޽����� ���� �� ����
    while (true)
    {
        // ������ �޽��� ó��
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // ������ ó��
            if (!Frame())
            {
                MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
        }
        // ESC�� ������ ���
        if (m_input->IsEscapePressed())
            break;
    }
}

bool SystemClass::Frame() const
{
    int mouseX = 0, mouseY = 0;

    if (!m_input->Frame())
        return false;

    m_input->GetMouseLocation(mouseX, mouseY);

    // const char inputKey = m_input->ReturnKey();
    // if (inputKey != -1)
    // {
    //     // printf("%c", inputKey);
    //     // m_graphics->InputKey(inputKey);
    // }

    m_timer->Frame();
    m_fps->Frame();
    m_cpu->Frame();

    m_position->SetFrameTime(m_timer->GetTime());
    
    bool keyDown = m_input->IsLeftArrowPressed();
    m_position->TurnLeft(keyDown);

    keyDown = m_input->IsRightArrowPressed();
    m_position->TurnRight(keyDown);
    
    float rotationY = 0.0f;
    m_position->GetRotation(rotationY);

    if (!m_graphics->Frame(mouseX, mouseY, m_fps->GetFps(), m_cpu->GetCpuPercentage(), m_timer->GetTime(), rotationY))
        return false;

    return m_graphics->Render();
}

LRESULT SystemClass::MessageHandler(const HWND hwnd, const UINT umsg, const WPARAM wparam, const LPARAM lparam)
{
    return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
    // �ܺ� �����͸� �ش� ��ü�� ����
    ApplicationHandle = this;

    // �� ���α׷��� �ν��Ͻ� ȹ��
    m_hinstance = GetModuleHandle(nullptr);

    // ���α׷� �̸� ����
    m_applicationName = L"HLSL_Project";

    // windows Ŭ���� wc ����
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = m_applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // windows Ŭ���� ���
    RegisterClassEx(&wc);

    // ����� �ػ� ȹ��
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int posX;
    int posY;

    // ��üȭ�� ó��
    if (FULL_SCREEN)
    {
        // ��üȭ�� ��忡�� �ػ� ���� �� ���� 32bit�� ����
        DEVMODE dmScreenSettings = {};
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
        dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = 0;
        posY = 0;
    }
    else
    {
        screenWidth = 800;
        screenHeight = 600;

        // ������ â�� ����, ������ �� ����� ��ġ ��Ŵ.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // ������ ����, �ڵ� ȹ��
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight,
        nullptr, nullptr, m_hinstance, nullptr);

    // �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� ����
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

void SystemClass::ShutdownWindows()
{
    if (FULL_SCREEN)
    {
        ChangeDisplaySettings(nullptr, 0);
    }

    // ���α׷� ������ ����
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;

    // ���α׷� �ν��Ͻ� ����
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = nullptr;

    // �ܺ������� ���� �ʱ�ȭ
    ApplicationHandle = nullptr;
}

LRESULT WndProc(const HWND hwnd, const UINT umsg, const WPARAM wparam, const LPARAM lparam)
{
    switch (umsg)
    {
    // ������ ���� Ȯ��
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    // ������ â ���� Ȯ��
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    // �� �� ��� �޽����� �ý��� Ŭ���� �޽��� ó���� �ѱ�.
    default:
    {
        return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
    }
    }
}
