// copyright very
#pragma once

#include "../string.h"
#include "tcharutils.h"
#include <stdio.h>

namespace vy
{
    // Generic File IO abstractions
    typedef FILE File;

    typedef char FileModeType;
    enum FileMode : FileModeType
    {
        FILE_READ   = 1 << 0,
        FILE_WRITE  = 1 << 1,
        FILE_CREATE = 1 << 2,
        FILE_BINARY = 1 << 3
    };
    bool FileModeTypeToString(FileModeType Mode, vy::String& OutString);

    File* OpenFile(const vy::String& Source, FileModeType Mode);
    int CloseFile(File* FileToClose);

    class ScopedFileHandle
    {
    public:
        ScopedFileHandle(const vy::String& Source, FileModeType Mode);
        ~ScopedFileHandle();

        operator bool()
        {
            return LoadedFile != nullptr ;// && Size > -1;
        }

        File* operator*() const
        {
            return LoadedFile;
        }

    private:
        File* LoadedFile = nullptr;
        size_t Size = -1;
    };

    template<typename CharType = TChar>
    class FileCharIterator
    {
    public:
        FileCharIterator(File* InFile)
            : LoadedFile(InFile)
        {};

        FileCharIterator& begin()
        {
            if (!LoadedFile)
            {
                return end();
            }
            fseek(LoadedFile, 0, SEEK_SET);
            CurrentChar = FGetC<CharType>(LoadedFile);
            return *this;
        }

        FileCharIterator& end()
        {
            static FileCharIterator EndInstance(nullptr);
            EndInstance.CurrentChar = TEndOfFile<TChar>::Value;
            return EndInstance;
        }

        FileCharIterator& operator++()
        {
            CurrentChar = FGetC<CharType>(LoadedFile);
            return *this;
        }

        bool operator==(const FileCharIterator& Other)
        {
            return CurrentChar == Other.CurrentChar;
        }
        bool operator!=(const FileCharIterator& Other)
        {
            return !(*this == Other);
        }

        CharType operator*()
        {
            return CurrentChar;
        }

    private:
        File* LoadedFile = nullptr;
        CharType CurrentChar = 0;

        static FileCharIterator EndFileCharIteratorInstance;
    };

    template<typename StringImpl>
    bool ReadEntireFile(const vy::PathString& Source, StringImpl& OutFileContents)
    {
        typedef typename StringImpl::CharType CharType;
        ScopedFileHandle ScopedHandle(Source, FILE_READ);
        if (!*ScopedHandle)
        {
            return false;
        }
        fseek(*ScopedHandle, 0, SEEK_END);
        long FileSize = ftell(*ScopedHandle);
        fseek(*ScopedHandle, 0, SEEK_SET);

        OutFileContents.ClearAndReserveSize(FileSize);

        CharType* MutableDataPtr = (CharType*)*OutFileContents; // remove const
        int i = 0;
        for (CharType c : FileCharIterator<CharType>(*ScopedHandle))
        {
            assert(i < FileSize);
            MutableDataPtr[i++] = c;
        }
        MutableDataPtr[FileSize] = 0; // null terminator

        return true;
    }

};
