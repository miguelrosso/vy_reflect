// copyright very

#pragma once
#include "containerbase.h"
#include "../memory/allocator.h"
#include "../memory/utils.h"
#include "../memory/move.h"
#include "../log.h"

#include <initializer_list>
#include <memory.h>

#define DEFAULT_DYNAMIC_CAPACITY 4

#define VY_LOG_OPS 0

namespace vy
{
    template<typename T, size_t NUM>
    class TStaticArray : public TContainerBase<T>
    {
        typedef typename vy::mem::RemovePointer<T>::Type ValueType;
        typedef typename vy::mem::RemovePointer<T>::Type* ValuePtrType;

    public:
        // default constructor
        TStaticArray()
            : TContainerBase<T>(NUM)
        {};

        // initializer list constructor
        TStaticArray(const std::initializer_list<T> initializer)
            : TContainerBase<T>(initializer.size())
        {
#ifdef VY_RANGE_CHECK
            assert(Size >= 0 && Size <= NUM);
#endif
            memcpy_s(GetData(), sizeof(T) * this->Size, initializer.begin(), sizeof(T) * initializer.size());
        };

        inline virtual T* GetData() override
        {
            return &Data[0];
        };
        inline virtual const T* GetData() const
        {
            return &Data[0];
        };

        inline T& operator[](int Index)
        {
#ifdef VY_RANGE_CHECK
            assert(Index >= 0 && Index < Size);
#endif
            return Data[Index];
        };
        inline const T& operator[](int Index) const
        {
#ifdef VY_RANGE_CHECK
            assert(Index >= 0 && Index < Size);
#endif
            return Data[Index];
        };

        inline virtual T* begin() override
        {
            return &Data[0];
        };
        inline virtual const T* begin() const override
        {
            return &Data[0];
        };

        inline virtual T* end() override
        {
            return &Data[this->Size];
        };
        inline virtual const T* end() const override
        {
            return &Data[this->Size];
        };

    private:
        T Data[NUM];
    };

    template<typename T, typename Allocator = mem::NativeAllocator>
    class TArray : public TContainerBase<T>
    {
        typedef typename vy::mem::RemovePointer<T>::Type ValueType;
        typedef typename vy::mem::RemovePointer<T>::Type* ValuePtrType;

    public:
        TArray()
            : TContainerBase<T>(0)
        {
            Capacity = DEFAULT_DYNAMIC_CAPACITY;
            Data = Allocator::template New<T>(Capacity);
#if VY_LOG_OPS
            VY_LOGF("Default constructed array with Capacity %i", Capacity);
#endif
        };

        // initializer list constructor
        TArray(const std::initializer_list<T> initializer, size_t Slack = 0)
            : TContainerBase<T>(initializer.size())
        {
            Capacity = this->Size + Slack;
            Data = Allocator::template New<T>(Capacity);

            vy::mem::MoveConstructElements(Data, initializer.begin(), this->Size);

#if VY_LOG_OPS
            VY_LOGF("Initialized Array with Size %i and Slack %i", Size, Slack);
#endif
        };

        // copy constructor
        TArray(const TArray<T, Allocator>& Other)
        {
#if VY_LOG_OPS
            VY_LOG("Copy ctor");
#endif
            this->Size = Other.Size;
            Capacity = Other.Capacity;
            Data = Allocator::template New<T>(Capacity);
            vy::mem::CopyConstructElements(Data, Other.Data, this->Size);
        };

        // move constructor
        TArray(TArray<T, Allocator>&& Other)
        {
#if VY_LOG_OPS
            VY_LOG("Move ctor");
#endif
            this->Size = Other.Size;
            Capacity = Other.Capacity;
            Data = Other.Data;

            Other.Data = nullptr;
            Other.Size = 0;
            Other.Capacity = 0;
        };

        // copy assignment
        TArray<T, Allocator>& operator=(const TArray<T, Allocator>& Other)
        {
#if VY_LOG_OPS
            VY_LOG("Copy assignment");
#endif
            if (Data)
            {
                Allocator::Free(Data, Capacity);
            }

            this->Size = Other.Size;
            Capacity = Other.Capacity;
            Data = Allocator::template New<T>(Capacity);
            vy::mem::CopyConstructElements(Data, Other.Data, this->Size);

            return *this;
        };

        // move assignment
        TArray<T, Allocator>& operator=(TArray<T, Allocator>&& Other)
        {
#if VY_LOG_OPS
            VY_LOG("Move assignment");
#endif
            if (Data)
            {
                Allocator::Free(Data, Capacity);
            }

            this->Size = Other.Size;
            Capacity = Other.Capacity;
            Data = Other.Data;

            Other.Data = nullptr;
            Other.Size = 0;
            Other.Capacity = 0;

            return *this;
        };

        virtual ~TArray()
        {
            if (!Data)
            {
                return;
            }
            vy::mem::DestructElements(Data, this->Size);
            Allocator::Free(Data, Capacity);
        };

        inline virtual T* GetData() override
        {
            return Data;
        };
        inline virtual const T* GetData() const override
        {
            return Data;
        };

        inline virtual T& operator[](int Index) override
        {
#ifdef VY_RANGE_CHECK
            if (!(Index >= 0 && Index < Size))
            {
                VY_LOGERRORF("Attempted to acces index %i in array of size %i", Index, Size);
            }
            assert(Index >= 0 && Index < Size);
#endif
            return Data[Index];
        };
        inline virtual const T& operator[](int Index) const override
        {
#ifdef VY_RANGE_CHECK
            assert(Index >= 0 && Index < Size);
#endif
            return Data[Index];
        };

        inline virtual T* begin() override
        {
            return &Data[0];
        };
        inline virtual const T* begin() const override
        {
            return &Data[0];
        };

        inline virtual T* end() override
        {
            return &Data[this->Size];
        };
        inline virtual const T* end() const override
        {
            return &Data[this->Size];
        };

        virtual void Clear(bool bReleaseMemory) override
        {
            if (bReleaseMemory)
            {
                Allocator::Free(Data, Capacity);
                Capacity = 0;
                this->Size = 0;
                return;
            }
            Capacity = 0;
            this->Size = 0;
            TContainerBase<T>::Clear(false);
        };

        virtual inline T* Add(const T& NewElem)
        {
            if (this->Size == Capacity)
            {
                Resize((Capacity > 0 ? Capacity : 1) * 2);
            }
            return &(Data[this->Size++] = NewElem);
        };

        virtual inline T* Add(T&& NewElem)
        {
            if (this->Size == Capacity)
            {
                Resize((Capacity > 0 ? Capacity : 1) * 2);
            }
            vy::mem::MoveConstructElements(&Data[this->Size++], &NewElem, 1);
            return &(Data[this->Size-1]);
        };

        virtual bool Remove(T& ElemToRemove)
        {
            T* Elem = nullptr;
            size_t ElemIndex = -1;
            for (size_t i = 0; i < this->Size; ++i)
            {
                T& I = Data[i];
                if (I == ElemToRemove)
                {
                    Elem = &I;
                    ElemIndex = i;
                    break;
                }
            }
            if (!Elem)
            {
                return false;
            }

            // Call the destructor for the element being removed, if we're holding a non-trivially-destructible type
            vy::mem::DestructElements(Elem, 1);

            // Move rest of array back by one if Elem is not the last one in the array
            if (ElemIndex < this->Size - 1)
            {
                Elem = Allocator::Move(Elem, Elem+1, this->Size - ElemIndex - 1);
            }

            this->Size--;
            return true;
        };

        virtual void RemoveIndex(int Index)
        {
            Remove((*this)[Index]);
        };

        inline void Resize(size_t NewCapacity)
        {
            if (NewCapacity == Capacity)
            {
                return;
            }
            if (Data)
            {
                Data = Allocator::Realloc(Data, NewCapacity, Capacity);
            }
            else
            {
                Data = Allocator::template New<T>(Capacity);
            }
            Capacity = NewCapacity;
        };

    private:
        T* Data = nullptr;

        // Total capacity in memory.
        size_t Capacity;
    };
};
