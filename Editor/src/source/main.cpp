#include "editor.hpp"

#include "reflection/reflector.hpp"
#include "rendering/material.hpp"
#include "resources/resource.hpp"
#include "serialize/iseriazable.h"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;
using namespace Tbx;

#ifdef _DEBUG



constexpr TypeId NameSpaceS = COMPILE_TIME_TYPE_KEY(std::string);

class TestSerialaZable : public DynamicReflectable
{
public:

    TestSerialaZable()
    {
        DYNAMIC_REFLECT_INIT
    }

    ~TestSerialaZable() override = default;

    IMP_DYNAMIC_REFLECT()
};
REFLECT(TestSerialaZable)

class DerivedTestSeriaZable : public TestSerialaZable
{
public:
    DerivedTestSeriaZable()
    {
        DYNAMIC_REFLECT_INIT
    }

    ~DerivedTestSeriaZable() override = default;

    IMP_DYNAMIC_REFLECT()
};


REFLECT(DerivedTestSeriaZable)

void RelfectionHashingTest()
{
    using namespace std;
    static_assert(COMPILE_TIME_TYPE_KEY(string) == NameSpaceS);
    static_assert(COMPILE_TIME_TYPE_KEY(Tbx::Vector3f) != COMPILE_TIME_TYPE_KEY(Tbx::Vector3d));
    static_assert(COMPILE_TIME_TYPE_KEY(Tbx::Vector3d) == COMPILE_TIME_TYPE_KEY(Tbx::Vector3d));
    static_assert(COMPILE_TIME_TYPE_KEY(PC_CORE::Resource) != COMPILE_TIME_TYPE_KEY(PC_CORE::Material));
    static_assert(COMPILE_TIME_TYPE_KEY(Tbx::Vector3f) != COMPILE_TIME_TYPE_KEY(Tbx::Vector3d));

    static_assert(COMPILE_TIME_TYPE_KEY(float_t) == COMPILE_TIME_TYPE_KEY(float));
    static_assert(COMPILE_TIME_TYPE_KEY(double_t) == COMPILE_TIME_TYPE_KEY(double));

    using d = uint32_t;
    static_assert(COMPILE_TIME_TYPE_KEY(d) == COMPILE_TIME_TYPE_KEY(unsigned int));

    TestSerialaZable testSerialaZable;
    DerivedTestSeriaZable derivedTestSeriaZable;

    assert(testSerialaZable.GetTypeKey() != derivedTestSeriaZable.GetTypeKey());
    assert(testSerialaZable.GetTypeKey() == Reflector::GetTypeKey<TestSerialaZable>());
    assert(derivedTestSeriaZable.GetTypeKey() == Reflector::GetTypeKey<DerivedTestSeriaZable>());

}

#endif



int main(int argc, char* argv[])
{
#ifdef _DEBUG
    RelfectionHashingTest();
#endif

    bool appShounldClose = false;


    while (!appShounldClose)
    {
        appShounldClose = true;
        Editor editor;

        editor.Init();
        editor.Run(&appShounldClose);
        editor.Destroy();
    }

  


    return 0;
}