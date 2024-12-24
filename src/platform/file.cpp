#include "file.h"
#include "../log.h"
#include "definitions.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace vy
{
    namespace Private
    {
        inline File* FOpen(const wchar_t* Source, const wchar_t* Mode)
        {
            return _wfopen(Source, Mode);
        }
        inline File* FOpen(const char* Source, const char* Mode)
        {
            return fopen(Source, Mode);
        }
        inline size_t GetFileSize(File* InFile)
        {
            if (!InFile)
            {
                return -1;
            }
#if VY_PLATFORM_WINDOWS
            struct _stat FileStat;
            if (_fstat(_fileno(InFile), &FileStat) != 0)
            {
                return -1;
            }
            return FileStat.st_size;
#endif
        }

    }

    bool FileModeTypeToString(FileModeType Mode, vy::String& OutString)
    {
        if (Mode & FILE_CREATE || (Mode & (FILE_READ & FILE_WRITE)))
        {
            if (Mode & FILE_BINARY)
            {
                OutString = TEXT("wb+");
            }
            else
            {
                OutString = TEXT("w+");
            }
            return true;
        }
        else if (Mode & FILE_READ)
        {
            OutString = TEXT("r");
        }
        else if (Mode & FILE_WRITE)
        {
            OutString = TEXT("w");
        }

        if (Mode & FILE_BINARY)
        {
            OutString += TEXT('b');
        }

        return OutString.GetLength() > 0;
    }

    File* OpenFile(const vy::String& Source, FileModeType Mode)
    {
        vy::String ModeStr;
        if (!FileModeTypeToString(Mode, ModeStr))
        {
            VY_LOGERRORF("Unknown FileModeType passed to OpenFile. (%i, %s)", ModeStr.GetSize(), ModeStr.GetSize() > 0 ? *ModeStr : TEXT(""));
            return nullptr;
        }
        VY_LOGF("Opening File \"%s\" with mode \"%s\"...", *Source, *ModeStr);

        File* OpenedFile = Private::FOpen(*Source, *ModeStr);
        if (!OpenedFile)
        {
            VY_LOGERRORF("Could not open File \"%s\"", *Source);
        }
        return OpenedFile;
    }

    int CloseFile(File* FileToClose)
    {
        return fclose(FileToClose);
    }

    ScopedFileHandle::ScopedFileHandle(const vy::String& Source, FileModeType Mode)
    {
        LoadedFile = OpenFile(Source, Mode);
        Size = Private::GetFileSize(LoadedFile); // @TODO - Size is -1 always?
    }

    ScopedFileHandle::~ScopedFileHandle()
    {
        if (LoadedFile)
        {
            CloseFile(LoadedFile);
        }
    }
}
