#include "../src/reflection.h"

#undef BASE_REFLECTION_CLASS_DEFINED
class Object
{
    REFLECT_BASE_CLASS(Object);
};
#define BASE_REFLECTION_CLASS_DEFINED

class TestClass : public Object
{
public:
    TestClass(int InIntVariable, float InFloatVariable)
        : IntVariable(InIntVariable),
          FloatVariable(InFloatVariable)
    {};

    int IntVariable;
    float FloatVariable;

    void VoidMethod() 
    {
        VY_LOG("TestClass::VoidMethod called!\n");
    }

    int Sum(int A)
    {
        return IntVariable + A;
    }

    REFLECT_CLASS(
        TestClass,
        REFLECT_INHERIT(Object),
        REFLECT_FIELD(IntVariable),
        REFLECT_FIELD(FloatVariable),
        REFLECT_METHOD(VoidMethod),
        REFLECT_METHOD(Sum)
    )
};


int main()
{
    TestClass Tester { 5, 20.05f };

    Tester.PrintReflectionData();

    return 0;
}
