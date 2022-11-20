#include "Stdafx.h"
#include "D3DClass.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
    
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
    
}

GraphicsClass::~GraphicsClass()
{
    
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
    
    if(!m_Direct3D)
    {
        return false;
    }

    if(!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }
    return true;
}

void GraphicsClass::Shutdown()
{
    if(m_Direct3D)
    {
        m_Direct3D->Shutdown();
        m_Direct3D = 0;
    }
}

bool GraphicsClass::Frame()
{
    return Render();
}

bool GraphicsClass::Render()
{
    m_Direct3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);
    m_Direct3D->EndScene();
    
    return true;
}

