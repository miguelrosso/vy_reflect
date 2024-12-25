// copyright yvm bla bla bla
#pragma once

#include "memory/move.h"
#include "containers/array.h"
#include "string.h"
#include "platform/tcharutils.h"
#include "platform/file.h"

// @HERE: Implement vy::tuple!
#include <functional>
#include <tuple>

#include <cstddef>

namespace vy::reflection
{
    struct ReflectionData;

     bool IsClassASameTypeOrChildOfClassB(const ReflectionData* A, const ReflectionData* B);

    /**
     * Transforms a pointer to a subclass into a pointer to a given parent class.
     */
    void* TransformChildPtrToParentPtr(void* Object, const ReflectionData* ChildClass, const ReflectionData* ParentClass);

    enum ReflectionFlags : unsigned char
    {
        RF_Variable   = 1 << 1,
        RF_Function   = 1 << 2,

        RF_Pointer    = 1 << 3,
        RF_Reference  = 1 << 4,
        RF_Array      = 1 << 5,

        RF_InheritedClass = (unsigned char)~0 // Special case for inherited classes. Should NOT be used outside of ReflectionData construction.
    };

    // @TODO: TypeInfo should be it's own struct.
    enum TypeInfo
    {
        TI_Int,
        TI_Float,
        TI_Double,
        TI_UInt,
        TI_Bool,
        TI_Char,
        TI_Struct,
        TI_Enum,

        TI_Void,
        
        // pointers & arrays? we probably will end up turning this into a struct

        TI_Unknown,
    };

    template<typename T>
    struct  ObjectMemberInitializer
    {
    };

    template<>
    struct  ObjectMemberInitializer<void>
    {
        static constexpr TypeInfo MemberType = TypeInfo::TI_Void;
        static constexpr unsigned char Flags = RF_Variable;
    };

    template<>
    struct  ObjectMemberInitializer<int>
    {
        static constexpr TypeInfo MemberType = TypeInfo::TI_Int;
        static constexpr unsigned char Flags = RF_Variable;
    };

    template<>
    struct  ObjectMemberInitializer<float>
    {
        static constexpr TypeInfo MemberType = TypeInfo::TI_Float;
        static constexpr unsigned char Flags = RF_Variable;
    };

    template<>
    struct  ObjectMemberInitializer<bool>
    {
        static constexpr TypeInfo MemberType = TypeInfo::TI_Bool;
        static constexpr unsigned char Flags = RF_Variable;
    };

    template<>
    struct  ObjectMemberInitializer<char>
    {
        static constexpr TypeInfo MemberType = TypeInfo::TI_Char;
        static constexpr unsigned char Flags = RF_Variable;
    };

    template<typename T>
    struct  ObjectMemberInitializer<T*>
    {
        static constexpr TypeInfo MemberType = ObjectMemberInitializer<T>::MemberType;
        static constexpr unsigned char Flags = RF_Pointer | ObjectMemberInitializer<T>::Flags;
    };

    template<typename T>
    struct  ObjectMemberInitializer<T&>
    {
        static constexpr TypeInfo MemberType = ObjectMemberInitializer<T>::MemberType;
        static constexpr unsigned char Flags = RF_Reference | ObjectMemberInitializer<T>::Flags;
    };

    template<typename T>
    struct  ObjectMemberInitializer<T[]>
    {
        static constexpr TypeInfo MemberType = ObjectMemberInitializer<T>::MemberType;
        static constexpr unsigned char Flags = RF_Array | ObjectMemberInitializer<T>::Flags;
    };

    template<typename T, typename R,  typename... Args>
    struct  ObjectMemberInitializer<R(T::*)(Args...)>
    {
        static constexpr TypeInfo MemberType = ObjectMemberInitializer<R>::MemberType;
        static constexpr unsigned char Flags = RF_Function;
        using MehtodPtrType = R(T::*)(Args...);
    };

    struct  AbstractObjectMemberMethodCaller
    {
        virtual ~AbstractObjectMemberMethodCaller() = default;
        virtual bool Call() = 0;
    };

    template<typename... MethodArgs>
    struct  AbstractObjectMemberMethodCallerWithArgs : public AbstractObjectMemberMethodCaller
    {
    protected:
        std::tuple<MethodArgs...>* Args = nullptr;

    public:
        void UnbindArgs()
        {
            Args = nullptr;
        };
        void BindArgs(std::tuple<MethodArgs...>* InArgs)
        {
            Args = InArgs;
        };

        virtual bool Call() = 0;
    };

    // template<typename MethodPtrType, typename... MethodArgs>
    template<typename MethodReturnType, typename MethodClassType, typename... MethodArgs>
    struct  ConcreteObjectMemberMethodCaller : public AbstractObjectMemberMethodCallerWithArgs<MethodClassType*, MethodArgs...>
    {
    using MethodPtrType = MethodReturnType(MethodClassType::*)(MethodArgs...);
    private:
        MethodPtrType MethodPtr;

    public:
        ConcreteObjectMemberMethodCaller(MethodPtrType InMethodPtr)
            : MethodPtr(InMethodPtr)
        {};

        bool Call() override
        {
            if (!MethodPtr || !this->Args)
            {
                return false;
            }

            std::apply(MethodPtr, *(this->Args));

            return true;
        };
    };

    template<typename... MethodArgs>
    struct  ObjectMemberMethodWrapper {
        using CallerWithArgsType = AbstractObjectMemberMethodCallerWithArgs<MethodArgs...>;

    private:
        AbstractObjectMemberMethodCaller* Caller = nullptr;
        std::tuple<MethodArgs...> Args;

    public:
        ObjectMemberMethodWrapper() = default;

        ObjectMemberMethodWrapper(
                AbstractObjectMemberMethodCaller* InCaller,
                MethodArgs&&... InArgs)
            : Caller(InCaller),
              Args(vy::mem::Forward<MethodArgs>(InArgs)...)
        {
        };
        
        ObjectMemberMethodWrapper(ObjectMemberMethodWrapper&& Other)
        {
            Caller = Other.Caller;
            Other.Caller = nullptr;
        };

        ObjectMemberMethodWrapper& operator=(ObjectMemberMethodWrapper&& Other)
        {
            Caller = Other.Caller;
            Other.Caller = nullptr;
            return *this;
        };

        bool operator()() 
        {
            if (Caller)
            {
                CallerWithArgsType* CallerWithArgs = (CallerWithArgsType*)Caller;
                CallerWithArgs->BindArgs(&Args);
                Caller->Call();
                CallerWithArgs->UnbindArgs();
                return true;
            }
            return false;
        };
    };

    // This struct is used on ReflectionData construction to initialize either an ObjectMember or a ObjectInheritanceData.
    struct  ReflectionDataInitializationParam
    {
        // ObjectMember Field constructor
        ReflectionDataInitializationParam(
                unsigned char InReflectionFlags,
                TypeInfo InMemberType,
                size_t InOffset,
                const vy::ANSIString& InMemberName
                )
            : ReflectionFlags(InReflectionFlags),
              MemberType(InMemberType),
              MemberName(InMemberName),
              MemberOffset(InOffset)
        {}

        // ObjectMember Method constructor
        ReflectionDataInitializationParam(
                unsigned char InReflectionFlags,
                TypeInfo InMemberType,
                AbstractObjectMemberMethodCaller* InMemberMethodCaller,
                const vy::ANSIString& InMemberName
                )
            : ReflectionFlags(InReflectionFlags),
              MemberType(InMemberType),
              MemberName(InMemberName),
              MemberMethodCaller(InMemberMethodCaller)
        {}

        // ObjectInheritanceData constructor
        ReflectionDataInitializationParam(
                unsigned char InReflectionFlags,
                ReflectionData* InClassReflectionData,
                size_t InOffset
                )
            : ReflectionFlags(InReflectionFlags),
              ParentReflectionDataPtr(InClassReflectionData),
              ParentOffset(InOffset)
        {}

        unsigned char ReflectionFlags; // Used for ObjectMember
        TypeInfo MemberType; // Used for ObjectMember
        const vy::ANSIString MemberName; // Used for ObjectMember

        union {
            size_t MemberOffset; // Used for ObjectMember
            AbstractObjectMemberMethodCaller* MemberMethodCaller; // Used for ObjectMember
            ReflectionData* ParentReflectionDataPtr; // Used for ObjectInheritanceData
        };
        size_t ParentOffset; // Used for ObjectInheritanceData
    };

    /**
     * Helper struct to bundle a pointer to some object and a pointer to its ReflectionData.
     * Used internally, and intended to be 'opaque' to the user (via implicit conversion 
     * operators added to reflection-enabled types)
     */
    struct  ReflectionInstanceContainer
    {
        void* Object;
        const ReflectionData* ObjectReflectionData;
    };

    // reflection representation of a class member. May be a method or a field
    struct  ObjectMember
    {
        ObjectMember() {};
        ObjectMember(
                unsigned char InReflectionFlags,
                TypeInfo InMemberType,
                size_t InOffset,
                const vy::ANSIString& InMemberName,
                const ReflectionData* InOwnerReflectionData
                )
            : ReflectionFlags(InReflectionFlags),
            MemberType(InMemberType),
            MemberName(InMemberName),
            Offset(InOffset),
            OwnerReflectionData(InOwnerReflectionData)
        {};
        ObjectMember(
                unsigned char InReflectionFlags,
                TypeInfo InMemberType,
                AbstractObjectMemberMethodCaller* InMethodCaller,
                const vy::ANSIString& InMemberName,
                const ReflectionData* InOwnerReflectionData
                )
            : ReflectionFlags(InReflectionFlags),
            MemberType(InMemberType),
            MemberName(InMemberName),
            MethodCaller(InMethodCaller),
            OwnerReflectionData(InOwnerReflectionData)
        {
            assert(MethodCaller != nullptr);
        };

        ObjectMember(ObjectMember&& Other) 
        {
            ReflectionFlags = Other.ReflectionFlags;
            MemberType = Other.MemberType;
            MemberName = vy::mem::Move(Other.MemberName);
            if (ReflectionFlags & RF_Variable)
            {
                Offset = Other.Offset;
            }
            else
            {
                MethodCaller = Other.MethodCaller;
                Other.MethodCaller = nullptr;
            }
            OwnerReflectionData = Other.OwnerReflectionData;
        };

        ObjectMember(const ObjectMember& Other)
        {
            assert(false && "ObjectMember copy constructor called! This should never happen.");
            ReflectionFlags = Other.ReflectionFlags;
            MemberType = Other.MemberType;
            MemberName = Other.MemberName;
            if (ReflectionFlags & RF_Variable)
            {
                Offset = Other.Offset;
            }
            else
            {
                MethodCaller = Other.MethodCaller;
            }
            OwnerReflectionData = Other.OwnerReflectionData;
        };

        ObjectMember& operator=(ObjectMember&& Other) 
        {
            new (this) ObjectMember(vy::mem::Forward<ObjectMember>(Other));
            return *this;
        };

        ObjectMember& operator=(const ObjectMember& Other) 
        {
            assert(false && "ObjectMember copy assignment operator called! This should never happen.");
            return *this;
        };

        unsigned char  ReflectionFlags;
        TypeInfo       MemberType;
        vy::ANSIString MemberName;

        union {
            size_t Offset;
            AbstractObjectMemberMethodCaller* MethodCaller;
        };

        const ReflectionData* OwnerReflectionData;

        template<typename T>
        T* GetValue(const ReflectionInstanceContainer& ObjectContainer) const
        {
            void* FinalObjectPtr = TransformChildPtrToParentPtr(ObjectContainer.Object, ObjectContainer.ObjectReflectionData, OwnerReflectionData);
            // printf("Transformed object ptr: %p\n", FinalObjectPtr);
            return (T*)((char*)FinalObjectPtr + Offset);
        }

        template<typename T>
        const T& SetValue(const ReflectionInstanceContainer& ObjectContainer, const T& NewValue) const
        {
            void* FinalObjectPtr = TransformChildPtrToParentPtr(ObjectContainer.Object, ObjectContainer.ObjectReflectionData, OwnerReflectionData);
            return (*(T*)((char*)FinalObjectPtr + Offset) = NewValue);
        }

        // Leaving these old implementations here in case they're useful to have later on
        // template<typename T>
        // T* GetValue(const void* ObjectPtr) const
        // {
        //     return (T*)((char*)ObjectPtr + Offset);
        // }
        // template<typename T>
        // const T& SetValue(void* ObjectPtr, const T& NewValue) const
        // {
        //     return (*(T*)((char*)ObjectPtr + Offset) = NewValue);
        // }

        template<typename D, typename... Args>
        bool InvokeMethod(D* ObjectPtr, Args&&... args) const
        {
            if (!(ReflectionFlags & RF_Function) ||
                !IsClassASameTypeOrChildOfClassB(&ObjectPtr->GetReflectionData(), OwnerReflectionData))
            {
                // We can't safely call this method with an ObjectPtr of a different type than our OwnerReflectionData.
                printf("Called Method %s with instance wrong of class.\n", *MemberName);
                return false;
            }

            void* FinalObjectPtr = (void*)ObjectPtr;
            FinalObjectPtr = ObjectPtr->GetReflectionData().TransformPtrToParentPtr((void*)ObjectPtr, *OwnerReflectionData);
            assert(FinalObjectPtr);

            ObjectMemberMethodWrapper<void*, Args...> Wrapper(MethodCaller, vy::mem::Forward<void*>(FinalObjectPtr), vy::mem::Forward<Args>(args)...);

            // ObjectMemberMethodWrapper<D*, Args...> Wrapper(MethodCaller, vy::mem::Forward<D*>(ObjectPtr), vy::mem::Forward<Args>(args)...);
            return Wrapper();

            return true;
        };

        bool operator==(const ObjectMember& Other) const
        {
            return ReflectionFlags == Other.ReflectionFlags &&
                   MemberType == Other.MemberType &&
                   MemberName == Other.MemberName &&
                   Offset == Other.Offset;
        }
        bool operator!=(const ObjectMember& Other) const
        {
            return !(*this == Other);
        }

        ~ObjectMember()
        {
            if (ReflectionFlags & RF_Function)
            {
                delete MethodCaller;
            }
        };
    };

    struct  ObjectInheritanceData
    {
        ReflectionData* ClassReflectionData;
        size_t Offset;

        ObjectInheritanceData() = default;
        ObjectInheritanceData(ReflectionData* InClassReflectionData, size_t InOffset)
            : ClassReflectionData(InClassReflectionData), Offset(InOffset)
        {};

        bool operator==(const ObjectInheritanceData& Other) const
        {
            return Other.ClassReflectionData == ClassReflectionData &&
                   Other.Offset == Offset;
        }
        bool operator!=(const ObjectInheritanceData& Other) const
        {
            return !(*this == Other);
        }
    };

    /**
     * Reflection representation of a given type.
     * Essentially contains a name, a list of members and a list of inheritance structs.
     */
    struct  ReflectionData
    {
        const vy::ANSIString ClassName;
        const vy::TArray<ObjectMember> Members;
        const vy::TArray<ObjectInheritanceData> InheritanceLayout;

        const ObjectMember* GetMember(const vy::ANSIString& InMemberName) const
        {
            const ObjectMember*   FoundMember = nullptr;
            const ReflectionData* DataPtr = this;
            const ObjectInheritanceData* InheritanceIterator = InheritanceLayout.begin();

            do
            {
                FoundMember = DataPtr->Members.FindByPredicate([&InMemberName](const ObjectMember& InMember){ return InMember.MemberName == InMemberName; });
            }
            while (
                    !FoundMember &&
                    InheritanceIterator != nullptr &&
                    InheritanceIterator != InheritanceLayout.end() &&
                    (DataPtr = (InheritanceIterator++)->ClassReflectionData)
                  );

            return FoundMember;
        }

        template<typename D, typename... Args>
        bool InvokeMethod(const vy::ANSIString& MemberMethodName, D* ObjectPtr, Args&&... args) const
        {
            const ObjectMember* MemberMethod = GetMember(MemberMethodName);
            if (!MemberMethod)
            {
                return false;
            }

            return MemberMethod->InvokeMethod(ObjectPtr, vy::mem::Forward<Args>(args)...);
        };

        bool HasMember(const vy::ANSIString& InMemberName) const
        {
            return GetMember(InMemberName) != nullptr;
        }

        const ObjectMember* begin() const
        {
            return Members.begin();
        }
        const ObjectMember* end() const
        {
            return Members.end();
        }

        // constructor
        ReflectionData() = default;
        ReflectionData(const vy::ANSIString& InClassName, std::initializer_list<ReflectionDataInitializationParam> InParameterList)
            : ClassName(InClassName)
        {
            ReflectionDataList.Add(this);

            if (InParameterList.size() == 0)
            {
                return;
            }

            // Reserve enough space, to make sure we don't realloc while constructing elements.
            {
                size_t ObjectMemberCount = 0;
                for (const ReflectionDataInitializationParam& Param : InParameterList)
                {
                    ObjectMemberCount += Param.ReflectionFlags != RF_InheritedClass;
                }

                if (InParameterList.size() > ObjectMemberCount)
                {
                    const_cast<vy::TArray<ObjectInheritanceData>&>(InheritanceLayout).Resize(InParameterList.size() - ObjectMemberCount);
                }
                if (ObjectMemberCount > 0)
                {
                    const_cast<vy::TArray<ObjectMember>&>(Members).Resize(ObjectMemberCount);
                }
            }

            for (const ReflectionDataInitializationParam& Param : InParameterList)
            {
                if (Param.ReflectionFlags == RF_InheritedClass)
                {
                    // iterate through Member.ParentReflectionDataPtr->InheritanceLayout recursively.
                    const_cast<vy::TArray<ObjectInheritanceData>&>(InheritanceLayout).Add(
                        {
                            Param.ParentReflectionDataPtr,
                            Param.ParentOffset
                        });
                }
                else if (Param.ReflectionFlags & RF_Variable)
                {
                    const_cast<vy::TArray<ObjectMember>&>(Members).Add(
                        {
                            Param.ReflectionFlags,
                            Param.MemberType,
                            Param.MemberOffset,
                            Param.MemberName,
                            this
                        });
                }
                else if (Param.ReflectionFlags & RF_Function)
                {
                    const_cast<vy::TArray<ObjectMember>&>(Members).Add(
                        {
                            Param.ReflectionFlags,
                            Param.MemberType,
                            Param.MemberMethodCaller,
                            Param.MemberName,
                            this
                        });
                }
                else
                {
                    assert(false && "Unknown ReflectionDataInitializationParam!");
                }
            }
        };

        bool operator==(const ReflectionData& Other) const
        {
            // always return false unless Other is this
            return &Other == this;
        };
        bool operator!=(const ReflectionData& Other) const
        {
            return !(*this == Other);
        };

        inline bool IsSameTypeOrChildOf(const ReflectionData& Other) const
        {
            return (Other == *this) || IsChildOf(Other);
        };

        inline bool IsChildOf(const ReflectionData& Other) const
        {
            // printf("Class %s IsChildOf class %s\n", *ClassName, *Other.ClassName);
            return InheritanceLayout.FindByPredicate([&Other](const ObjectInheritanceData& OID)
                        { return OID.ClassReflectionData == &Other; } );
        };

        /**
         * Transforms a pointer to a subclass into a pointer to a given parent class.
         */
        void* TransformPtrToParentPtr(void* Object, const ReflectionData& InParentClass) const
        {
            if (&InParentClass == this)
            {
                return Object;
            }

            if (!IsChildOf(InParentClass))
            {
                return nullptr;
            }

            for (const ObjectInheritanceData& OID : InheritanceLayout)
            {
                if (OID.ClassReflectionData->IsSameTypeOrChildOf(InParentClass))
                {
                    char* ObjectPtr = (char*)Object;
                    ObjectPtr += OID.Offset;
                    return OID.ClassReflectionData->TransformPtrToParentPtr(ObjectPtr, InParentClass);
                }
            }

            return nullptr;
        }

        // Global reflection data list
        inline static TArray<ReflectionData*> ReflectionDataList {};
    };

    template<>
    struct  ObjectMemberInitializer<ReflectionData>
    {
        static constexpr TypeInfo MemberType = TI_Unknown;
        static constexpr unsigned char Flags = RF_InheritedClass;
    };

    namespace utils
    {
#ifndef VY_SHIPPING
         inline const char* GetTypeStr(const vy::reflection::ObjectMember& Member);
         inline void PrintMember(const vy::reflection::ObjectMember& Member, const void* Instance, vy::File* Stream = stdout);
#endif
         void PrintReflectionData(const vy::reflection::ReflectionData& RD, const ReflectionInstanceContainer& Instance, vy::File* Stream = stdout);
    };

}; // namespace vy::reflection

#define REFLECT_INHERIT(InClassName) \
    { \
        vy::reflection::ObjectMemberInitializer<vy::reflection::ReflectionData>::Flags, \
        &vy_##InClassName##_reflection_ns::vy_##InClassName##_reflection_data, \
        (size_t)(((char*)static_cast<InClassName*>((SelfReflectType*)1)) - ((char*)1)) \
    }

#define REFLECT_FIELD(MemberName) \
    { \
        vy::reflection::ObjectMemberInitializer<decltype(SelfReflectType::MemberName)>::Flags, \
        vy::reflection::ObjectMemberInitializer<decltype(SelfReflectType::MemberName)>::MemberType, \
        offsetof(SelfReflectType, MemberName), \
        #MemberName \
    } 

#define REFLECT_METHOD(MemberName) \
    { \
        vy::reflection::ObjectMemberInitializer<decltype(&SelfReflectType::MemberName)>::Flags, \
        vy::reflection::ObjectMemberInitializer<decltype(&SelfReflectType::MemberName)>::MemberType, \
        new vy::reflection::ConcreteObjectMemberMethodCaller(&SelfReflectType::MemberName), \
        #MemberName \
    } 

#define REFLECT_CLASS(InClassName, ...) \
        public:\
        friend class vy::reflection::ReflectionData; \
        inline static const vy::reflection::ReflectionData& GetStaticReflectionData() \
        { \
            static const vy::reflection::ReflectionData* ClassRD = \
                *vy::reflection::ReflectionData::ReflectionDataList.FindByPredicate([](const vy::reflection::ReflectionData* InRD){ return InRD->ClassName == #InClassName; }); \
            assert(ClassRD); \
            return *ClassRD; \
        } \
        virtual inline const vy::reflection::ReflectionData& GetReflectionData() const OVERRIDE_IF_NOT_BASE \
        { \
            if (!ClassReflectionData) \
            { \
                const vy::reflection::ReflectionData** ClassReflectionDataNonConstPtr = const_cast<const vy::reflection::ReflectionData**>(&ClassReflectionData); \
                *ClassReflectionDataNonConstPtr = &InClassName::GetStaticReflectionData(); \
            } \
            return *ClassReflectionData; \
        }; \
    }; \
    namespace vy_##InClassName##_reflection_ns { \
        using SelfReflectType = InClassName; \
        inline static vy::reflection::ReflectionData vy_##InClassName##_reflection_data { \
            #InClassName, \
            { __VA_ARGS__ } \
        }; \
        inline const vy::reflection::ReflectionData* GetReflectionData() \
        { \
            return &vy_##InClassName##_reflection_data; \
        } \
        inline const vy::reflection::ObjectMember* GetMember(const vy::ANSIString& MemberName) \
        { \
            return vy_##InClassName##_reflection_data.GetMember(MemberName); \
        } \
        inline bool HasMember(const vy::ANSIString& MemberName) \
        { \
            return vy_##InClassName##_reflection_data.HasMember(MemberName); \
        }

#define REFLECT_BASE_CLASS(InClassName) \
        protected: \
        const vy::reflection::ReflectionData* const ClassReflectionData = nullptr; \
        public: \
        operator vy::reflection::ReflectionInstanceContainer() const \
        { \
            return { (void*)this, &GetReflectionData() }; \
        }; \
        inline const vy::reflection::ObjectMember* GetMember(const vy::ANSIString& MemberName) const \
        { \
            return GetReflectionData().GetMember(MemberName); \
        }; \
        template<typename... Args> \
        bool InvokeMethod(const vy::ANSIString& MemberName, Args&&... args) const \
        { \
            return GetReflectionData().InvokeMethod(MemberName, this, vy::mem::Forward<Args>(args)...); \
        }; \
        inline void PrintReflectionData(vy::File* Stream = stdout) const \
        { \
            vy::reflection::utils::PrintReflectionData(GetReflectionData(), *this, Stream); \
        }; \
        REFLECT_CLASS(InClassName)

// a reflected class' non-static GetReflectionData is virtual, and we use the same
// REFLECT_CLASS macro in REFLECT_BASE_CLASS, so we need a way to conditionally add
// the override specifier or not to the method. So, before calling 
// REFLECT_BASE_CLASS the user must #undef BASE_REFLECTION_CLASS_DEFINED, and #define
// it after calling REFLECT_BASE_CLASS. See object.h for an example of this.
#ifndef BASE_REFLECTION_CLASS_DEFINED
#define OVERRIDE_IF_NOT_BASE
#else
#define OVERRIDE_IF_NOT_BASE override
#endif
