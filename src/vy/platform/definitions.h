// copyright very

#pragma once

#ifdef _WIN32
#define VY_PLATFORM_WINDOWS 1
#else
#define VY_PLATFORM_WINDOWS 0
#endif

#if defined(__unix__)
#define VY_PLATFORM_UNIX 1
#else
#define VY_PLATFORM_UNIX 0
#endif

#if VY_PLATFORM_WINDOWS

#define UNICODE
#define _UNICODE

typedef wchar_t TChar;

#define TEXT_PASTE_ANSI_INNER(str) #str
#define TEXT_PASTE_ANSI(str) TEXT_PASTE_ANSI_INNER(str)

#define TEXT_PASTE_INNER(str) L#str
#define TEXT_PASTE(str) TEXT_PASTE_INNER(str)

#define VY_FILE TEXT_PASTE_ANSI(__FILE__)
#define VY_WIDE_FILE TEXT_PASTE(__FILE__)

#ifndef TEXT
#define TEXT(str) L ## str
#endif

#elif VY_PLATFORM_UNIX


typedef char TChar;

#define TEXT_PASTE_ANSI_INNER(str) #str
#define TEXT_PASTE_ANSI(str) TEXT_PASTE_ANSI_INNER(str)

#define TEXT_PASTE_INNER(str) #str
#define TEXT_PASTE(str) TEXT_PASTE_INNER(str)

#define VY_FILE TEXT_PASTE_ANSI(__FILE__)
#define VY_WIDE_FILE TEXT_PASTE(__FILE__)

#ifndef TEXT
#define TEXT(str) str
#endif

#endif // VY_PLATFORM_WINDOWS
