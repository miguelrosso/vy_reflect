// copyright very

#pragma once

#include "../log.h"

namespace vy
{
    template<typename T>
    class TContainerBase
    {
    protected:
        TContainerBase()
        {};

        TContainerBase(size_t InSize)
            : Size(InSize)
        {};

    public:
        inline virtual T* GetData() = 0;
        inline virtual const T* GetData() const = 0;

        inline virtual T* begin() = 0;
        inline virtual const T* begin() const = 0;
        inline virtual T* end() = 0;
        inline virtual const T* end() const = 0;

        inline virtual T& operator[](int Index) = 0;
        inline virtual const T& operator[](int Index) const = 0;

        virtual void Clear(bool bReleaseMemory)
        {
            for (T& i : *this)
            {
                i = T();
            }
        };

        virtual T* Find(const T* Item)
        {
            for (auto& i : *this)
            {
                if (i == *Item)
                {
                    return &i;
                }
            }
            return nullptr;
        };

        virtual const T* Find(const T* Item) const
        {
            for (const auto& i : *this)
            {
                if (i == *Item)
                {
                    return &i;
                }
            }
            return nullptr;
        };

        template <typename PredicateType>
        const T* FindByPredicate(PredicateType Predicate) const
        {
            for (const auto& i : *this)
            {
                if (Predicate(i))
                {
                    return &i;
                }
            }
            return nullptr;
        }

        template <typename PredicateType>
        T* FindByPredicate(PredicateType Predicate)
        {
            for (auto& i : *this)
            {
                if (Predicate(i))
                {
                    return &i;
                }
            }
            return nullptr;
        }

        virtual bool Contains(const T* Item) const
        {
            for (const auto& i : *this)
            {
                if (&i == Item)
                {
                    return true;
                }
            }
            return false;
        };

        virtual bool Contains(const T& Item) const
        {
            for (const auto& i : *this)
            {
                if (i == Item)
                {
                    return true;
                }
            }
            return false;
        };

        bool operator==(const TContainerBase<T>& Other) const
        {
            if (Size != Other.GetSize())
            {
                return false;
            }

            return CompareContents(Other);
        };

        bool operator!=(const TContainerBase<T>& Other) const
        {
           return !(*this == Other);
        };

        inline size_t GetSize() const { return Size; }

    protected:
        virtual bool CompareContents(const TContainerBase<T>& Other) const
        {
            for (size_t i = 0; i < Size; ++i)
            {
                if ((*this)[i] != Other[i])
                {
                    return false;
                }
            }
            return true;
        };

        size_t Size = 0;
    };
};
