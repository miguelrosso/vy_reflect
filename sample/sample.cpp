#include "../src/reflection.h"

#undef BASE_REFLECTION_CLASS_DEFINED
class Object
{
    REFLECT_BASE_CLASS(Object);
};
#define BASE_REFLECTION_CLASS_DEFINED

class TestBaseClass : public Object
{
public:

    virtual void VirtualMethod()
    {
        VY_LOG("TestBaseClass::VirtualMethod called");
    };

    REFLECT_CLASS(
        TestBaseClass,
        REFLECT_INHERIT(Object),
        REFLECT_METHOD(VirtualMethod)
    )
}

class TestDerivedClass : public TestBaseClass
{
public:
    TestDerivedClass(int InIntVariable, float InFloatVariable)
        : IntVariable(InIntVariable),
          FloatVariable(InFloatVariable)
    {};

    int IntVariable;
    float FloatVariable;

    void VoidMethod() 
    {
        VY_LOG("TestClass::VoidMethod called!\n");
    };

    void Sum(int A, int& OutValue)
    {
        OutValue =  IntVariable + A;
    };

    void VirtualMethod() override
    {
        VY_LOG("TestDerivedClass::VirtualMethod called");
    };

    REFLECT_CLASS(
        TestDerivedClass,
        REFLECT_INHERIT(TestBaseClass),
        REFLECT_FIELD(IntVariable),
        REFLECT_FIELD(FloatVariable),
        REFLECT_METHOD(VoidMethod),
        REFLECT_METHOD(Sum),
    )
};


int main()
{
    TestDerivedClass Tester { 5, 20.05f };

    Tester.PrintReflectionData();

    Tester.InvokeMethod("VoidMethod");

    int result;
    Tester.InvokeMethod("Sum", 2, result);
    VY_LOGF("Sum method invokation returned: %i", result);

    Tester.InvokeMethod("VirtualMethod");

    return 0;
}
