#include "Stdafx.h"
#include "InputClass.h"
#include "graphicsclass.h"
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

    InitializeWindows(screenWidth, screenHeight);

    m_input = new InputClass;
    if(!m_input)
    {
        return false;
    }

    m_input->Initialize();

    m_Graphics = new GraphicsClass;
    if(!m_Graphics)
    {
        return false;
    }

    return m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
}

void SystemClass::Shutdown()
{
    if(m_Graphics)
    {
        m_Graphics->Shutdown();
        delete m_Graphics;
        m_Graphics = nullptr;
    }

    if(m_input)
    {
        delete m_input;
        m_input = nullptr;
    }

    ShutdownWindows();
}

void SystemClass::Run() const
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while(true)
    {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if(!Frame())
                break;
        }
    }
}

bool SystemClass::Frame() const
{
    if(m_input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    return m_Graphics->Frame();
}

LRESULT SystemClass::MessageHandler(const HWND hwnd, const UINT umsg, const WPARAM wparam, const LPARAM lparam) const
{
    switch(umsg)
    {
    case WM_KEYDOWN:
        {
            m_input->KeyDown(static_cast<unsigned int>(wparam));
            return 0;
        }
    case WM_KEYUP:
        {
            m_input->KeyUp(static_cast<unsigned int>(wparam));
            return 0;
        }
        default:
        {
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
    ApplicationHandle = this;

    m_hinstance = GetModuleHandle(nullptr);

    m_applicationName = L"Dx11Demo_02";

    WNDCLASSEX wc;
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = m_hinstance;
    wc.hIcon            = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm          = wc.hIcon;
    wc.hCursor          = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground    = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName     = nullptr;
    wc.lpszClassName    = m_applicationName;
    wc.cbSize           = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int posX;
    int posY;

    if(FULL_SCREEN)
    {
        DEVMODE dmScreenSettings = {};
        dmScreenSettings.dmSize         = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth    = static_cast<unsigned long>(screenWidth);
        dmScreenSettings.dmPelsHeight   = static_cast<unsigned long>(screenHeight);
        dmScreenSettings.dmBitsPerPel   = 32;
        dmScreenSettings.dmFields       = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = 0;
        posY = 0;
    }
    else
    {
        screenWidth = 800;
        screenHeight = 600;

        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight,
        nullptr, nullptr, m_hinstance, nullptr);

    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

void SystemClass::ShutdownWindows()
{
    if(FULL_SCREEN)
    {
        ChangeDisplaySettings(nullptr, 0);
    }

    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;

    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = nullptr;

    ApplicationHandle = nullptr;
}

LRESULT WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch(umsg)
    {
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
        {
            return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
        }
    }
}


