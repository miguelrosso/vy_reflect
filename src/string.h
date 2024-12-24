// copyright yvm bla bla bla

#pragma once

#include "platform/definitions.h"
#include "memory/allocator.h"
#include "platform/tcharutils.h"
#include <assert.h>

#define INITIAL_BUFFER_SIZE 512

namespace vy
{
    template<typename T, typename Allocator = vy::mem::NativeAllocator>
    class TString
    {
    public:
        typedef T CharType;

        TString()
            : Data(nullptr), Size(0)
        {
        };

        TString(T* Data)
        {
            SetData(Data, CalcSize(Data));
        };

        TString(const TString<T>& Other)
        {
            SetData(Other.Data, Other.Size);
        };

        TString(TString<T>&& Other)
        {
            Data = Other.Data;
            Size = Other.Size;
            Other.Data = nullptr;
            Other.Size = 0;
        };

        TString<T>& operator=(const TString<T>& Other)
        {
            FreeData();
            SetData(Other.Data, Other.Size);
            return *this;
        };

        TString<T>& operator=(TString<T>&& Other)
        {
            FreeData();
            Data = Other.Data;
            Size = Other.Size;
            Other.Data = nullptr;
            Other.Size = 0;
            return *this;
        };

        const T* operator*() const
        {
            return Data;
        };

        const T& operator[](size_t i)
        {
#ifdef VY_RANGE_CHECK
            assert(i >= 0 && i < Size-1);
#endif
            return Data[i];
        };

        bool operator==(const TString<T>& Other) const
        {
            if (Size != Other.Size)
            {
                return false;
            }
            for (size_t i = 0; i < Size; ++i)
            {
                if (Data[i] != Other.Data[i])
                {
                    return false;
                }
            }
            return true;
        };
        bool operator==(const TChar* Other) const
        {
            size_t OtherSize = CalcSize(Other);
            if (Size != OtherSize)
            {
                return false;
            }
            for (size_t i = 0; i < Size; ++i)
            {
                if (Data[i] != Other[i])
                {
                    return false;
                }
            }
            return true;
        }

        TString<T>& operator+=(const T* Other)
        {
            size_t OtherSize = CalcSize(Other);
            VY_LOGF("OtherSize: %i (%s)", OtherSize, Other);
            if (OtherSize > 0)
            {
                // Subtract one from OtherSize as otherwise we'd be accounting for 2 terminators
                size_t NewSize = (Size > 1 ? Size : 1) + OtherSize-1;
                Data = Allocator::template ReallocZeroedExtra<T>(Data, NewSize, Size);
                Allocator::template Copy<T>(Data + (Size > 0 ? Size-1 : 0), Other, OtherSize-1);
                Size = NewSize;
            }
            return *this;
        }

        TString<T>& operator+=(T Other)
        {
            size_t NewSize = Size > 1 ? Size + 1 : 2;
            Data = Allocator::template ReallocZeroedExtra<T>(Data, NewSize, Size);
            Allocator::template Copy<T>(Data + (Size > 0 ? Size-1 : 0), &Other, 1);
            Size = NewSize;
            return *this;
        }

        ~TString()
        {
            if (Data)
            {
                assert(Size > 0);
                Allocator::template Free<T>(Data, Size);
            }
        }

        // Get the length of the string, excluding the 0 terminator
        inline size_t GetLength() const
        {
            return Size-1;
        }

        // Get the length of the string, including the 0 terminator
        inline size_t GetSize() const
        {
            return Size;
        }

        inline bool IsEmpty() const
        {
            return Size <= 1;
        }

        inline void Clear()
        {
            FreeData();
        }

        void ClearAndReserveSize(size_t NewSize)
        {
            if (Size != NewSize)
            {
                FreeData();
                Size = NewSize;
                Data = Allocator::template NewZeroed<T>(Size);
            }
        }

        template<typename... Args>
        static inline TString<CharType> Printf(const CharType* Format, Args... args)
        {
            CharType InitialBuffer[INITIAL_BUFFER_SIZE];

            CharType* Buffer = InitialBuffer;
            size_t    BufferSize = INITIAL_BUFFER_SIZE;

            int Result = vy::SPrintF(InitialBuffer, Format, args...);
            if (Result == -1)
            {
                Buffer = nullptr;
                BufferSize *= 2;
                Buffer = Allocator::template NewZeroed<CharType>(BufferSize);
                do
                {
                    size_t OldBufferSize = BufferSize;
                    BufferSize *= 2;
                    Buffer = Allocator::template ReallocZeroedExtra<CharType>(Buffer, BufferSize, OldBufferSize);
                    Result = vy::SPrintF(Buffer, Format, args...);
                } while (Result == -1);
            }
            
            TString<CharType> ResultString = Buffer;

            if (BufferSize != INITIAL_BUFFER_SIZE)
            {
                Allocator::Free(Buffer, BufferSize);
            }

            return ResultString;
        };

    private:
        void SetData(T* InData, size_t NewSize)
        {
            if (Size != NewSize)
            {
                FreeData();
                Size = NewSize;
                Data = Allocator::template NewZeroed<T>(Size);
            }
            Allocator::template Copy<T>(Data, InData, Size);
        }

        void FreeData()
        {
            if (!Data)
            {
                assert(Size == 0);
                return;
            }
            Allocator::Free(Data, Size);
            Size = 0;
        }

        static inline size_t CalcSize(const T* InData)
        {
            if (!InData)
            {
                return 0;
            }

            size_t Result = 0;
            const T* Ptr = InData;

            do
            {
                ++Result;
            }
            while(*(Ptr++) != 0);

            return Result;
        }

        T* Data = nullptr;
        size_t Size = 0;

    };
    typedef TString<TChar> String;
    typedef TString<char>  ANSIString;
    typedef TString<wchar_t>  UTF16String;
    typedef String PathString; //@TODO: Actual Path type
    typedef String Name;       //@TODO: Actual Name type
};
