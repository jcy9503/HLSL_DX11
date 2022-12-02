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

    // 윈도우 생성 초기화
    InitializeWindows(screenWidth, screenHeight);

    // m_input : 마우스와 키보드 입력 처리 클래스
    m_input = new InputClass;
    if (!m_input) return false;
    if (!m_input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight))
    {
        MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_graphics : 그래픽 렌더링 처리 클래스
    m_graphics = new GraphicsClass;
    if (!m_graphics) return false;
    if (!m_graphics->Initialize(screenWidth, screenHeight, m_hwnd))
    {
        MessageBox(m_hwnd, L"Could not initialize Graphics object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_sound : 다이렉트 사운드 클래스
    m_sound = new SoundClass;
    if (!m_sound) return false;
    if (!m_sound->Initialize(m_hwnd))
    {
        MessageBox(m_hwnd, L"Could not initialize Direct sound object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // m_fps : 프레임 클래스
    m_fps = new FpsClass;
    if (!m_fps) return false;
    m_fps->Initialize();

    // m_cpu : CPU 사용률 관련 클래스
    m_cpu = new CpuClass;
    if (!m_cpu) return false;
    m_cpu->Initialize();

    // m_timer : 타이머 클래스
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
    // 메시지 구조체 생성/초기화
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // 사용자로부터 종료 메시지를 받을 때 종료
    while (true)
    {
        // 윈도우 메시지 처리
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 프레임 처리
            if (!Frame())
            {
                MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
        }
        // ESC를 눌렀을 경우
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
    // 외부 포인터를 해당 객체로 지정
    ApplicationHandle = this;

    // 이 프로그램의 인스턴스 획득
    m_hinstance = GetModuleHandle(nullptr);

    // 프로그램 이름 지정
    m_applicationName = L"HLSL_Project";

    // windows 클래스 wc 설정
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

    // windows 클래스 등록
    RegisterClassEx(&wc);

    // 모니터 해상도 획득
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int posX;
    int posY;

    // 전체화면 처리
    if (FULL_SCREEN)
    {
        // 전체화면 모드에서 해상도 지정 및 색상 32bit로 지정
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

        // 윈도우 창을 가로, 세로의 정 가운데에 위치 시킴.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // 윈도우 생성, 핸들 획득
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight,
        nullptr, nullptr, m_hinstance, nullptr);

    // 윈도우를 화면에 표시하고 포커스를 지정
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

    // 프로그램 윈도우 제거
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;

    // 프로그램 인스턴스 제거
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = nullptr;

    // 외부포인터 참조 초기화
    ApplicationHandle = nullptr;
}

LRESULT WndProc(const HWND hwnd, const UINT umsg, const WPARAM wparam, const LPARAM lparam)
{
    switch (umsg)
    {
    // 윈도우 종료 확인
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    // 윈도우 창 닫음 확인
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    // 그 외 모든 메시지는 시스템 클래스 메시지 처리로 넘김.
    default:
    {
        return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
    }
    }
}
