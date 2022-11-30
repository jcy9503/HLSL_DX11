#pragma once

#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
constexpr float RADIAN = 0.0174532925f;

#include "targetver.h"
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#include <tchar.h>

#include "DxDefine.h"

char* ConvertWCtoC(const wchar_t*);
// ReSharper disable once CppInconsistentNaming
wchar_t* ConvertCtoWC(const char*);
