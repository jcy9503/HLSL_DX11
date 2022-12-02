#pragma once
#include "Stdafx.h"

class InputClass;
class GraphicsClass;
class SoundClass;
class FpsClass;
class CpuClass;
class TimerClass;
class PositionClass;

class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&);
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Run() const;

    static LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame() const;
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName{};
    HINSTANCE m_hinstance{};
    HWND m_hwnd{};

    InputClass* m_input = nullptr;
    GraphicsClass* m_graphics = nullptr;
    SoundClass* m_sound = nullptr;
    FpsClass* m_fps = nullptr;
    CpuClass* m_cpu = nullptr;
    TimerClass* m_timer = nullptr;
    PositionClass* m_position = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;
