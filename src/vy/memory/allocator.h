// copyright yvm

#pragma once

#include "../log.h"
#include <stdlib.h>
#include <string.h>

#ifndef VERY_SHIPPING
#include <assert.h>
#endif

#define VY_LOG_ALLOCATIONS 0

namespace vy::mem
{
    class NativeAllocator
    {
    public:
        template<typename T>
        static inline T* New(int n = 1)
        {
#ifndef VERY_SHIPPING
            assert(n > 0);
            MemAllocated += sizeof(T)*n;
#endif
#if VY_LOG_ALLOCATIONS
            VY_LOGF("NativeAllocator - Alloc %i bytes (%i bits)", sizeof(T) * n, sizeof(T)*n*8);
            GetMemAllocated();
#endif
            return (T*)malloc(sizeof(T)*n);
        };

        template<typename T>
        static inline T* NewZeroed(int n = 1)
        {
#ifndef VERY_SHIPPING
            assert(n > 0);
            MemAllocated += sizeof(T)*n;
#endif
#if VY_LOG_ALLOCATIONS
            VY_LOGF("NativeAllocator - AllocZeroed %i bytes (%i bits)", sizeof(T) * n, sizeof(T)*n*8);
            GetMemAllocated();
#endif
            return (T*)calloc(n, sizeof(T)*n);
        };

        template<typename T>
        static inline T* Realloc(T*& Data, int n, int old_n = 1)
        {
#ifndef VERY_SHIPPING
            assert(n > 0);
            MemAllocated += sizeof(T)*(n-old_n);
#endif
#if VY_LOG_ALLOCATIONS
            VY_LOGF("NativeAllocator - Realloc %i bytes (%i bits) - was %i bytes (%i bits)", sizeof(T) * n, sizeof(T)*n*8, sizeof(T) * old_n, sizeof(T)*old_n*8);
            GetMemAllocated();
#endif
            return (T*)realloc(Data, sizeof(T)*n);
        };

        template<typename T>
        static inline T* ReallocZeroedExtra(T*& Data, int n, int old_n)
        {
            const int OldSize = sizeof(T)*old_n;
            const int NewSize = sizeof(T)*n;
            const int SizeDifference = NewSize - OldSize;
#ifndef VERY_SHIPPING
            assert(n > 0);
            MemAllocated += SizeDifference;
#endif
#if VY_LOG_ALLOCATIONS && 0
            VY_LOGF("NativeAllocator - Realloc %i bytes (%i bits) - was %i bytes (%i bits)", sizeof(T) * n, sizeof(T)*n*8, sizeof(T) * old_n, sizeof(T)*old_n*8);
            GetMemAllocated();
#endif
            void* NewPtr = (void*)realloc(Data, NewSize);
            if (NewPtr && SizeDifference > 0)
            {
                memset((char*)NewPtr + OldSize, 0, SizeDifference);
            }
            return (T*)NewPtr;
        };

        template<typename T>
        static inline void Free(T*& Data, int n = 1)
        {
#ifndef VERY_SHIPPING
            assert(Data);
            assert(n > 0);
            MemAllocated -= sizeof(T)*n;
            if (MemAllocated == 0)
            {
                VY_LOG("NativeAllocator - All bytes freed");
            }
#endif
#if VY_LOG_ALLOCATIONS
            VY_LOGF("NativeAllocator - Free %i bytes (%i bits)", sizeof(T) * n, sizeof(T)*n*8);
            GetMemAllocated();
#endif
            free(Data);
            Data = nullptr;
        };

        template<typename T>
        static inline T* Move(T* Destination, const T* Source, int n = 1)
        {
#if VY_LOG_ALLOCATIONS
            // VY_LOGF("NativeAllocator - Move: %llu bytes (%llu bits)", (unsigned long long)MemAllocated, (unsigned long long)(MemAllocated*8));
#endif
            return (T*)memmove(Destination, Source, sizeof(T) * n);
        };

        template<typename T>
        static inline T* Copy(T* Destination, const T* Source, int n = 1)
        {
            return (T*)memcpy(Destination, Source, sizeof(T) * n);
        };

        inline static size_t GetMemAllocated() {
#if VY_LOG_ALLOCATIONS
            VY_LOGF("NativeAllocator - GetMemAllocated: %llu bytes (%llu bits)", (unsigned long long)MemAllocated, (unsigned long long)(MemAllocated*8));
#endif
            return MemAllocated;
        }

    private:
        inline static size_t MemAllocated;
    };
};

#undef VY_LOG_ALLOCATIONS
