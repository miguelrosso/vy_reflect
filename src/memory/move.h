// copyright yvm bla bla bla

#pragma once

namespace vy::mem
{
    template <typename T> 
    struct TRemoveReference      { typedef T Type; };
    template <typename T> 
    struct TRemoveReference<T& > { typedef T Type; };
    template <typename T> 
    struct TRemoveReference<T&&> { typedef T Type; };

    template<typename T>
    constexpr typename TRemoveReference<T>::Type&& Move(T&& Temp)
    {
        typedef typename TRemoveReference<T>::Type OutType;
        return (OutType&&)Temp;
    };

    template<typename T>
    constexpr T&& Forward(typename TRemoveReference<T>::Type& Arg)
    {
        return (T&&)Arg;
    }
    template<typename T>
    constexpr T&& Forward(typename TRemoveReference<T>::Type&& Arg)
    {
        return (T&&)Arg;
    }
};
