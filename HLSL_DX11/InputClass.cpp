#include "Stdafx.h"
#include "InputClass.h"

InputClass::InputClass()
= default;

InputClass::InputClass(const InputClass&): m_mouseState()
{
}

InputClass::~InputClass()
= default;

bool InputClass::Initialize(const HINSTANCE hinstance, const HWND hwnd, const int screenWidth, const int screenHeight)
{
    // 마우스 커서의 위치 지정에 사용될 화면 크기 설정
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Direct Input 인터페이스 초기화
    HRESULT result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_directInput), nullptr);
    if(FAILED(result)) return false;

    // 키보드의 Direct Input 인터페이스 생성
    result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr);
    if(FAILED(result)) return false;

    // 키보드 데이터 형식 설정
    result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
    if(FAILED(result)) return false;

    // 다른 프로그램과 공유하지 않도록 키보드의 Cooperative level 설정
    result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if(FAILED(result)) return false;

    // 키보드 할당
    result = m_keyboard->Acquire();
    if(FAILED(result)) return false;

    // 마우스의 Direct Input 인터페이스 설정
    result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, nullptr);
    if(FAILED(result)) return false;

    // 마우스의 데이터 형식 설정
    result = m_mouse->SetDataFormat(&c_dfDIMouse);
    if(FAILED(result)) return false;

    // 다른 프로그램과 공유할 수 있도록 마우스의 Cooperative level 설정
    result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if(FAILED(result)) return false;

    // 마우스 할당
    result = m_mouse->Acquire();
    if(FAILED(result)) return false;

    return true;
}

void InputClass::Shutdown()
{
    if(m_mouse)
    {
        m_mouse->Unacquire();
        m_mouse->Release();
        m_mouse = nullptr;
    }

    if(m_keyboard)
    {
        m_keyboard->Unacquire();
        m_keyboard->Release();
        m_keyboard = nullptr;
    }

    if(m_directInput)
    {
        m_directInput->Release();
        m_directInput = nullptr;
    }
}

bool InputClass::Frame()
{
    if(!ReadKeyboard()) return false;
    if(!ReadMouse()) return false;

    ProcessInput();

    return true;
}

bool InputClass::ReadKeyboard()
{
    const HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), static_cast<LPVOID>(&m_keyboardState)); // 키보드 디바이스 획득
    if(FAILED(result))
    {
        if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            m_keyboard->Acquire();
        else
        {
            return false;
        }
    }

    return true;
}

bool InputClass::ReadMouse()
{
    const HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&m_mouseState)); // 마우스 디바이스 획득
    if(FAILED(result))
    {
        if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            m_mouse->Acquire();
        else
        {
            return false;
        }
    }

    return true;
}

void InputClass::ProcessInput()
{
    // 프레임 동안 마우스 위치 변경을 기반으로 마우스 커서의 위치를 업데이트한다.
    m_mouseX += m_mouseState.lX;
    m_mouseY += m_mouseState.lY;

    if(m_mouseX < 0) m_mouseX = 0;
    if(m_mouseY < 0) m_mouseY = 0;

    if(m_mouseX > m_screenWidth) m_mouseX = m_screenWidth;
    if(m_mouseY > m_screenHeight) m_mouseY = m_screenHeight;
}

bool InputClass::IsEscapePressed() const
{
    // ESC 키가 눌려지고 있는지 Bit 계산으로 확인
    if(m_keyboardState[DIK_ESCAPE] & 0x80)
        return true;

    return false;
}

bool InputClass::IsLeftArrowPressed() const
{
    if(m_keyboardState[DIK_LEFT] & 0x80 || m_keyboardState[DIK_A] & 0x80)
        return true;
    
    return false;
}

bool InputClass::IsRightArrowPressed() const
{
    if(m_keyboardState[DIK_RIGHT] * 0x80 || m_keyboardState[DIK_D] & 0x80)
        return true;

    return false;
}

char InputClass::ReturnKey() const
{
    for(int i = 0; i < 256; ++i)
    {
        if(m_keyboardState[i] & 0x80)
            return m_keys[i];
    }

    return -1;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY) const
{
    mouseX = m_mouseX;
    mouseY = m_mouseY;
}
