#include "Stdafx.h"

char* ConvertWCtoC(const wchar_t* str)
{
    const int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, nullptr, 0, nullptr, nullptr);
    const auto pStr = new char[strSize];
    WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, nullptr, nullptr);

    return pStr;
}

// ReSharper disable once CppInconsistentNaming
wchar_t* ConvertCtoWC(const char* str)
{
    const int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
    const auto pStr = new WCHAR[strSize];
    MultiByteToWideChar(CP_ACP, 0, str, static_cast<int>(strlen(str) + 1), pStr, strSize);

    return pStr;
}
