// copyright yvm

#pragma once

#include "../log.h"
#include <stdlib.h>
#include <new>
#include <type_traits>

#ifndef VERY_SHIPPING
#include <assert.h>
#endif

#if VY_PLATFORM_UNIX
#include <string.h>
#define memmove_s(dest, count, src, _) memmove(dest, src, count)
#endif

#define VY_LOG_ALLOCATIONS 1

namespace vy::mem
{
    template<typename T, typename SizeType>
    inline void MoveConstructElements(T* Destination, const T* Source, SizeType Count)
    {
        if constexpr (std::is_trivially_move_constructible<T>::value)
        {
            memmove_s(Destination, sizeof(T) * Count, Source, sizeof(T) * Count);
        }
        else
        {
            for (SizeType i = 0; i < Count; ++i)
            {
                new (&Destination[i]) T((T&&)Source[i]);
            }
        }
    }

    template<typename T, typename SizeType>
    inline void CopyConstructElements(T* Destination, const T* Source, SizeType Count)
    {
        if constexpr (std::is_trivially_copyable<T>::value)
        {
            memcpy_s(Destination, sizeof(T) * Count, Source, sizeof(T) * Count);
        }
        else
        {
            for (size_t i = 0; i < Count; ++i)
            {
                new (&Destination[i]) T(Source[i]);
            }
        }
    }

    template<typename T, typename SizeType>
    inline void DestructElements(T* Data, SizeType Count)
    {
        if constexpr (!std::is_trivially_destructible<T>::value)
        {
            for (SizeType i = 0; i < Count; ++i)
            {
                Data[i].T::~T();
            }
        }
    }

    template<typename T, typename SizeType>
    inline void CopyAssignElements(T* Destination, const T* Source, SizeType Count)
    {
        if constexpr (std::is_trivially_copy_assignable<T>::value)
        {
            memcpy_s(Destination, sizeof(T) * Count, Source, sizeof(T) * Count);
        }
        else
        {
            for (size_t i = 0; i < Count; ++i)
            {
                Destination[i] = Source[i];
            }
        }
    }

    template<typename T, typename SizeType>
    inline void MoveAssignElements(T* Destination, const T* Source, SizeType Count)
    {
        if constexpr (std::is_trivially_move_assignable<T>::value)
        {
            memmove_s(Destination, sizeof(T) * Count, Source, sizeof(T) * Count);
        }
        else
        {
            for (size_t i = 0; i < Count; ++i)
            {
                Destination[i] = T((T&&)Source[i]);
            }
        }
    }

    template<typename T>
    class RemovePointer
    {
    public:
        typedef T Type;
    };
    template<typename T>
    class RemovePointer<T*>
    {
    public:
        typedef T Type;
    };
    template<typename T>
    class RemovePointer<T&>
    {
    public:
        typedef T Type;
    };

    template<typename T>
    class IsPointer
    {
    public:
        enum { Value = false };
    };
    template<typename T>
    class IsPointer<T*>
    {
    public:
        enum { Value = true };
    };

    template<typename T>
    class IsReference
    {
    public:
        enum { Value = false };
    };
    template<typename T>
    class IsReference<T&>
    {
    public:
        enum { Value = true };
    };

    template<typename T>
    class AddReference
    {
    public:
        typedef T& Type;
    };
    template<typename T>
    class AddReference<T&>
    {
    public:
        typedef T& Type;
    };

    template<typename T>
    inline typename RemovePointer<T>::Type DereferenceIfIsPtr(T& Elem)
    {
        if constexpr (IsPointer<T>::Value)
        {
            return *Elem;
        }
        else
        {
            return Elem;
        }
    }

    template<typename T>
    inline typename RemovePointer<T>::Type* ReferenceIfIsValue(T& Elem)
    {
        if constexpr (IsPointer<T>::Value)
        {
            return Elem;
        }
        else
        {
            return &Elem;
        }
    }

};

#undef VY_LOG_ALLOCATIONS
