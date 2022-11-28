#pragma once

#include "targetver.h"

#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
constexpr float RADIAN = 0.0174532925f;

#include <Windows.h>

#include <stdio.h>
#include <cstdlib>
#include <memory.h>
#include <tchar.h>

#include "DxDefine.h"

char* ConvertWCtoC(const wchar_t*);
wchar_t* ConvertCtoWC(const char*);