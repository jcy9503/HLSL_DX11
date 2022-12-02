#pragma once

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

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
    char ReturnKey() const;
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
    char m_keys[256] = {
        '!', '!', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '!', '!', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
        '[', ']', '!', '!', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '!', '!', '!', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',',
        '.', '/',
    };
    DIMOUSESTATE m_mouseState{};

    int m_screenWidth = 0;
    int m_screenHeight = 0;
    int m_mouseX = 0;
    int m_mouseY = 0;
};
