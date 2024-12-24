#include "reflection.h"

namespace vy::reflection
{
        // @TODO: Implement this once we have an actual StringBuilder implementation...
        // const vy::ANSIString ObjectMember::ToString() const
        // {
        //     return vy::ANSIString::Printf(
        //             "%s %s%s %s = %s\n",
        //             (ReflectionFlags & RF_Variable) ? "Field" : "Method",
        //             GetTypeStr(),
        //             (ReflectionFlags & RF_Pointer) ? "*" : (ReflectionFlags & RF_Reference) ? "&" : "",
        //             *MemberName,
        //             GetValueStr()
        //             );
        // }

    bool IsClassASameTypeOrChildOfClassB(const ReflectionData* A, const ReflectionData* B)
    {
        assert(A && B);
        return A->IsSameTypeOrChildOf(*B);
    };

    void* TransformChildPtrToParentPtr(void* Object, const ReflectionData* ChildClass, const ReflectionData* ParentClass)
    {
        // @NOTE: Object ptr could be a ReflectionInstanceContainer. Would also be
        // safer as that would remove the possibility of passing in the wrong ChildClass
        return ChildClass->TransformPtrToParentPtr(Object, *ParentClass);
    };

    namespace utils
    {
#ifndef VY_SHIPPING
        const char* GetTypeStr(const vy::reflection::ObjectMember& Member)
        {
            switch (Member.MemberType)
            {
                case TI_Int:
                    return "int";
                case TI_Bool:
                    return "bool";
                case TI_Float:
                    return "float";
                case TI_Double:
                    return "double";
                case TI_UInt:
                    return "double";
                case TI_Char:
                    return "char";
                case TI_Struct:
                    return "struct";
                case TI_Enum:
                    return "enum";
                case TI_Void:
                    return "void";
                default:
                    return "Unknown";
            }
        };

        void PrintMember(const vy::reflection::ObjectMember& Member, const ReflectionInstanceContainer& Instance, vy::File* Stream)
        {
            vy::TFPrintf(Stream, "%s", (Member.ReflectionFlags & RF_Variable) ? "Field" : "Method");

            vy::TFPrintf(Stream, " %s%s %s = ", GetTypeStr(Member), (Member.ReflectionFlags & RF_Pointer) ? "*" : (Member.ReflectionFlags & RF_Reference) ? "&" : "", *Member.MemberName);
            if (Member.ReflectionFlags & RF_Function)
            {
                vy::TFPrintf(Stream, "Function(...)");
            }
            else if (Member.ReflectionFlags & (RF_Pointer | RF_Reference))
            {
                char** ReflValue = Member.GetValue<char*>(Instance);
                vy::TFPrintf(Stream, "%p", *ReflValue);
            }
            else
            {
                switch (Member.MemberType)
                {
                    case TI_Int:
                        {
                            int* ReflValue = Member.GetValue<int>(Instance);
                            vy::TFPrintf(Stream, "%i", *ReflValue);
                            break;
                        }
                    case TI_Bool:
                        {
                            bool* ReflValue = Member.GetValue<bool>(Instance);
                            vy::TFPrintf(Stream, "%i", *ReflValue);
                            break;
                        }
                    case TI_Float:
                        {
                            float* ReflValue = Member.GetValue<float>(Instance);
                            vy::TFPrintf(Stream, "%f", *ReflValue);
                            break;
                        }
                    case TI_Double:
                        {
                            double* ReflValue = Member.GetValue<double>(Instance);
                            vy::TFPrintf(Stream, "%f", *ReflValue);
                            break;
                        }
                    case TI_UInt:
                        {
                            unsigned int* ReflValue = Member.GetValue<unsigned int>(Instance);
                            vy::TFPrintf(Stream, "%ui", *ReflValue);
                            break;
                        }
                        break;
                    case TI_Char:
                        {
                            char* ReflValue = Member.GetValue<char>(Instance);
                            vy::TFPrintf(Stream, "%s", *ReflValue);
                            break;
                        }
                    case TI_Struct:
                        {
                            vy::TFPrintf(Stream, "{...}");
                            break;
                        }
                        break;
                    case TI_Enum:
                        {
                            vy::TFPrintf(Stream, "{...}");
                            break;
                        }
                        break;
                    default:
                        {
                            vy::TFPrintf(Stream, "Unknown");
                            break;
                        }
                        break;
                }
            }

            vy::TFPrintf(Stream, "\n");
        };
#endif

        void PrintReflectionData(const vy::reflection::ReflectionData& RD, const ReflectionInstanceContainer& Instance, vy::File* Stream)
        {
#ifndef VY_SHIPPING
            vy::TFPrintf(Stream, "%s\n{\n", *RD.ClassName);
            for (const vy::reflection::ObjectInheritanceData& Parent : RD.InheritanceLayout)
            {
                PrintReflectionData(*Parent.ClassReflectionData, Instance, Stream);
            }
            for (const vy::reflection::ObjectMember& Member : RD.Members)
            {
                PrintMember(Member, Instance, Stream);
            }
            vy::TFPrintf(Stream, "}\n");
#endif
        };
    };
};
