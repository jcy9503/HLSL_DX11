#pragma once

class InputClass
{
public:
    /**
     * Keyboard input function
     */
    InputClass();
    InputClass(const InputClass&);
    ~InputClass();

    void Initialize();

    void KeyDown(unsigned int);
    void KeyUp(unsigned int);

    bool IsKeyDown(unsigned int) const;

private:
    bool m_keys[256];
};