#include "Stdafx.h"
#include "InputClass.h"

InputClass::InputClass()
{
    
}

InputClass::InputClass(const InputClass&)
{
    
}

InputClass::~InputClass()
= default;

void InputClass::Initialize()
{
    for (bool& m_key : m_keys)
    {
        m_key = false;
    }
}

void InputClass::KeyDown(const unsigned int input)
{
    m_keys[input] = true;
}

void InputClass::KeyUp(const unsigned int input)
{
    m_keys[input] = false;
}

bool InputClass::IsKeyDown(const unsigned int key) const
{
    return m_keys[key];
}
