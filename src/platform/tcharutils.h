// copyright yvm bla bla bla
#pragma once

#include "definitions.h"
#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <cwctype>

namespace vy
{
    inline int IsDigit(char c)
    {
        return isdigit(c);
    };
    inline int IsDigit(wchar_t c)
    {
        return iswdigit(c);
    };

    inline int IsAlpha(char c)
    {
        return isalpha(c);
    };
    inline int IsAlpha(wchar_t c)
    {
        return iswalpha(c);
    };

    template<typename CharType>
    struct TEndOfFile {};
    template<>
    struct TEndOfFile<char> { enum { Value = EOF }; };
    template<>
    struct TEndOfFile<wchar_t> { enum { Value = WEOF }; };

    template<typename CharType>
    inline CharType FGetC(FILE* InFile)
    {
        return 0;
    };
    template<>
    inline wchar_t FGetC<wchar_t>(FILE* InFile)
    {
        return fgetwc(InFile);
    }
    template<>
    inline char FGetC<char>(FILE* InFile)
    {
        return fgetc(InFile);
    }

    template<typename CharType, typename... Args>
    inline int SPrintF(CharType* Buffer, const CharType* Format, Args... args)
    {
        return -1;
    };
    template<typename... Args>
    inline int SPrintF(char* Buffer, const char* Format, Args... args)
    {
        return sprintf(Buffer, Format, args...);
    };
    template<typename... Args>
    inline int SPrintF(wchar_t* Buffer, const wchar_t* Format, Args... args)
    {
        return swprintf(Buffer, Format, args...);
    };

    template<typename ...Args>
    inline int TPrintf(const char* str, Args... args)
    {
        return printf(str, args...);
    };
    template<typename ...Args>
    inline int TPrintf(const wchar_t* str, Args... args)
    {
        return wprintf(str, args...);
    };
    template<typename ...Args>
    inline int TFPrintf(FILE* const stream, const char* str, Args... args)
    {
        return fprintf(stream, str, args...);
    };
    template<typename ...Args>
    inline int TFPrintf(FILE* const stream, const wchar_t* str, Args... args)
    {
        return fwprintf(stream, str, args...);
    };
};
