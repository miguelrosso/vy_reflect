// copyright yvm bla bla bla

#pragma once

#include "platform/definitions.h"
#include "platform/tcharutils.h"
#include <stdio.h>
#include <wchar.h>

#define VY_LOG(str) \
    vy::Log(VY_WIDE_FILE, __LINE__, TEXT("[%s:%u]\nLog: ") TEXT(str) TEXT("\n\n"));
#define VY_LOGF(str, ...) \
    vy::Log(VY_WIDE_FILE, __LINE__, TEXT("[%s:%u]:\nLog: ") TEXT(str) TEXT("\n\n"), __VA_ARGS__);

#define VY_LOGWARNING(str) \
    vy::Log(VY_WIDE_FILE, __LINE__, TEXT("[%s:%u]:\nWarning: ") TEXT(str) TEXT("\n\n"));
#define VY_LOGWARNINGF(str, ...) \
    vy::Log(VY_WIDE_FILE, __LINE__, TEXT("[%s:%u]:\nWarning: ") TEXT(str) TEXT("\n\n"), __VA_ARGS__);

#define VY_LOGERROR(str) \
    vy::LogError(VY_WIDE_FILE, __LINE__, TEXT("[%s:%u]:\nError: ") TEXT(str) TEXT("\n\n"));
#define VY_LOGERRORF(str, ...) \
    vy::LogError(VY_WIDE_FILE, __LINE__, TEXT("[%s:%u]:\nError: ") TEXT(str) TEXT("\n\n"), __VA_ARGS__);

#define VY_PRINT_BOOL(expression) \
    expression ? TEXT("true") : TEXT("false")

namespace vy
{
    enum LogCategory
    {
        LOG_DISPLAY = 0,
        LOG_WARNING = 1,
        LOG_ERROR   = 2
    };

    template<typename... Args>
    inline void Log(const TChar* file, unsigned int line, const TChar* str, Args... args)
    {
        vy::TPrintf(str, file, line, args...);
    };

    template<typename... Args>
    inline void LogError(const TChar* file, unsigned int line, const TChar* str, Args... args)
    {
        vy::TFPrintf(stderr, str, file, line, args...);
    };
}


