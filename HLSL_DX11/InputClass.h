#pragma once

#define DIRECTINPUT_VERSION 0x0800

class InputClass
{
public:
    /**
     * Keyboard/Mouse input function
     */
    InputClass();
    InputClass(const InputClass&);
    ~InputClass();

    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown();
    bool Frame();

    bool IsEscapePressed() const;
    void GetMouseLocation(int&, int&) const;

private:
    bool ReadKeyboard();
    bool ReadMouse();
    void ProcessInput();

private:
    IDirectInput8* m_directInput = nullptr;
    IDirectInputDevice8* m_keyboard = nullptr;
    IDirectInputDevice8* m_mouse = nullptr;

    unsigned char m_keyboardState[256] = {0,};
    DIMOUSESTATE m_mouseState{};

    int m_screenWidth = 0;
    int m_screenHeight = 0;
    int m_mouseX = 0;
    int m_mouseY = 0;
};
