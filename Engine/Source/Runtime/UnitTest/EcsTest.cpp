#include <gtest/gtest.h>

#include "World/World.hpp"

struct MyTestComponent : public PC_CORE::Component
{
    float x;

    int y;

    std::string name;
    MyTestComponent() : x(0.f), y(0), name("") {}

    auto operator<=>(const MyTestComponent&) const = default;

};
REFLECT(MyTestComponent, PC_CORE::Component)
REFLECT_MEMBER(MyTestComponent, x)
REFLECT_MEMBER(MyTestComponent, y)
REFLECT_MEMBER(MyTestComponent, name)



TEST(ECS, CreateEntity)
{
    PC_CORE::World world;

    for (size_t i = 0; i < 100; i++)
    {
        const auto ent = world.level.CreateEntity("Entity" + std::to_string(i));

        EXPECT_TRUE(static_cast<size_t>(ent) == i);
        EXPECT_TRUE(world.level.GetEntityName(ent) == "Entity" + std::to_string(i));
    }
    
}


TEST(ECS, AddComponent)
{
    PC_CORE::World world;

    const PC_CORE::EntityId ent0 = world.level.CreateEntity("ent0");
    world.level.AddComponent<MyTestComponent>(ent0);
    
    auto m = world.level.GetComponent<MyTestComponent>(ent0);

    const auto defaultOne = MyTestComponent();

    EXPECT_TRUE(m != defaultOne);
    EXPECT_TRUE(m.x == defaultOne.x);
    EXPECT_TRUE(m.y == defaultOne.y);
    EXPECT_TRUE(m.name == defaultOne.name);
}

TEST(ECS, GetComponent)
{
    PC_CORE::World world;

    const PC_CORE::EntityId ent0 = world.level.CreateEntity("ent0");
    world.level.AddComponent<MyTestComponent>(ent0);

    auto& m = world.level.GetComponent<MyTestComponent>(ent0);

    m.x = 0;
    m.y = 1;
    m.name = "Foo";


    EXPECT_TRUE(m == world.level.GetComponent<MyTestComponent>(ent0));
}

TEST(ECS, HasCompnent)
{
    PC_CORE::World world;

    const PC_CORE::EntityId ent0 = world.level.CreateEntity("ent0");

    EXPECT_FALSE(world.level.HasComponent<MyTestComponent>(ent0));
}

TEST(ECS, RemoveComponent)
{
    PC_CORE::World world;

    const PC_CORE::EntityId ent0 = world.level.CreateEntity("ent0");
    world.level.AddComponent<MyTestComponent>(ent0);
    EXPECT_TRUE(world.level.HasComponent<MyTestComponent>(ent0));
    world.level.RemoveComponent<MyTestComponent>(ent0);
    EXPECT_FALSE(world.level.HasComponent<MyTestComponent>(ent0));
}

TEST(ECS, AddComponent2)
{
    PC_CORE::World world;

    const PC_CORE::EntityId ent0 = world.level.CreateEntity("ent0");
    world.level.AddComponent<MyTestComponent>(ent0);

    const PC_CORE::EntityId ent1 = world.level.CreateEntity("ent1");
    world.level.AddComponent<MyTestComponent>(ent1);

    auto& m0 = world.level.GetComponent<MyTestComponent>(ent0);
    auto& m1 = world.level.GetComponent<MyTestComponent>(ent1);

    EXPECT_FALSE(m0 == m1);
}

TEST(ECS, RemoveComponent2)
{
    PC_CORE::World world;

    for (size_t i = 0; i < 10; i++)
    {
        auto ent = world.level.CreateEntity("Entity " + std::to_string(i));
        world.level.AddComponent<MyTestComponent>(ent);

        auto& c = world.level.GetComponent<MyTestComponent>(ent);
        c.name = "Entity " + std::to_string(i);
    }
    auto& r0 = world.level.GetComponent<MyTestComponent>(9);

    world.level.RemoveComponent<MyTestComponent>(5);
    EXPECT_FALSE(world.level.HasComponent<MyTestComponent>(5));
    auto& r1 = world.level.GetComponent<MyTestComponent>(9);
    EXPECT_FALSE(r1.name == "Entity " + std::to_string(5));
    auto& r2 = world.level.GetComponent<MyTestComponent>(9);
    EXPECT_TRUE(r2.name == "Entity " + std::to_string(9));


}