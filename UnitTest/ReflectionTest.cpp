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

enum EnumTest0 : uint8_t
{
    E0 = 0,
    E2 = 2,
    E3 = 3,
};

REFLECT(EnumTest0);


TEST(TestReflection, EnumReflectionU8)
{
    const auto& t = PC_CORE::Reflector::GetEnum<EnumTest0>();
    
    EXPECT_EQ("EnumTest0", t.name);

    for (size_t i = 0; i < t.members.size(); i++)
    {
        const auto& m = t.members[i];

        switch (i)
        {
        case 0:
            EXPECT_EQ(m.value, 0);
            EXPECT_EQ(m.name, "E0");

            break;
        case 1:
            EXPECT_EQ(m.value, 2);
            EXPECT_EQ(m.name, "E2");
            break;
        case 2:
            EXPECT_EQ(m.value, 3);
            EXPECT_EQ(m.name, "E3");
            break;
        default:
            break;
        }

    }

}

class TestPtr : public std::shared_ptr<Resource>
{
public:
};

TEST(TestReflection, TestPtrAlias)
{
    static_assert(!std::is_same_v<TestPtr, std::shared_ptr<Resource>>);
    static_assert(COMPILE_TIME_TYPE_KEY(TestPtr) != COMPILE_TIME_TYPE_KEY(std::shared_ptr<Resource>));

}