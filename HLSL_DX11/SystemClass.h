#pragma once
#include "Stdafx.h"

class InputClass;
class GraphicsClass;

class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&);
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Run() const;

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM) const;

private:
    bool Frame() const;
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName{};
    HINSTANCE m_hinstance{};
    HWND m_hwnd{};

    InputClass* m_input = nullptr;
    GraphicsClass* m_Graphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;