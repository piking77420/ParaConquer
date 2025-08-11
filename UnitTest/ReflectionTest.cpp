#include <Gtest/Gtest.h>


#include "Math/ToolboxTypedef.hpp"
#include "Reflection/CompiletimeKey.hpp"
#include "Rendering/Material.hpp"
#include "Resources/Resource.hpp"

using namespace PC_CORE;

class TestDynamicReflectable : public PC_CORE::DynamicReflectable
{
public:
    TestDynamicReflectable()
    {
        DYNAMIC_REFLECT_INIT
    }

    ~TestDynamicReflectable() override = default;

    IMP_DYNAMIC_REFLECT()
    
private:
    REFLECT(TestDynamicReflectable)
};

class DerivedTestDynamicReflectable : public TestDynamicReflectable
{
public:
    DerivedTestDynamicReflectable()
    {
        DYNAMIC_REFLECT_INIT
    }

    ~DerivedTestDynamicReflectable() override = default;

    IMP_DYNAMIC_REFLECT()
    
private:
    REFLECT(DerivedTestDynamicReflectable)
};

constexpr TypeId NameSpaceS = COMPILE_TIME_TYPE_KEY(std::string);

TEST(TestReflection, DynamicReflectable) 
{
    // Some Static assert 
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

    // Test
    TestDynamicReflectable testSerialaZable;
    DerivedTestDynamicReflectable derivedTestSeriaZable;

    EXPECT_TRUE(testSerialaZable.GetTypeKey() != derivedTestSeriaZable.GetTypeKey());
    EXPECT_TRUE(testSerialaZable.GetTypeKey() == Reflector::GetTypeKey<TestDynamicReflectable>());
    EXPECT_TRUE(derivedTestSeriaZable.GetTypeKey() == Reflector::GetTypeKey<DerivedTestDynamicReflectable>()); 
}