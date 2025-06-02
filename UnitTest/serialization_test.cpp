#include <map>
#include <gtest/gtest.h>

#include "reflection/reflector.hpp"
#include "resources/resource.hpp"
#include "resources/resource_manager.hpp"
#include "serialize/serializer.h"


using namespace PC_CORE;


struct SerializaStruct
{
    float x;

    char y;

    uint64_t z;

    std::array<float, 4> w;

    float array[4];

    Tbx::Quaternionf q;
    
    REFLECT(SerializaStruct)
    REFLECT_MEMBER(SerializaStruct, x);
    REFLECT_MEMBER(SerializaStruct, y);
    REFLECT_MEMBER(SerializaStruct, z);

    REFLECT_MEMBER(SerializaStruct, w);
    REFLECT_MEMBER(SerializaStruct, array);
    REFLECT_MEMBER(SerializaStruct, q);

};
const SerializaStruct s =
    {
    .x = 1.214f,
    .y = 'd',
    .z = std::numeric_limits<uint64_t>::max(),
    .w = {123.f, 0.24531684f, 1200.0f, 0356.f},
    .array = {1656948.f, 0.21366354f, static_cast<float>(M_PI), static_cast<float>(-M_PI)},
    .q = Tbx::Quaternionf::Identity() 
    };


TEST(Serialization, BasicSerialization)
{
    Serializer::Serialize(s, "BasicSerialization.test");
    SerializaStruct s2;
    Serializer::DeSerialize(&s2, "BasicSerialization.test");

    EXPECT_FLOAT_EQ(s.x , s2.x);
    EXPECT_EQ(s.y , s2.y);
    EXPECT_EQ(s.z , s2.z);

    for (size_t i = 0 ; i < 4 ; i++)
    {
        EXPECT_FLOAT_EQ(s.w[i] , s2.w[i]);
    }
    
    for (size_t i = 0 ; i < 4 ; i++)
    {
        EXPECT_FLOAT_EQ(s.array[i] , s2.array[i]);
    }

    EXPECT_EQ(s.q , s2.q);
}


struct DataTest
{
    int x;
    float y;
    uint64_t z;
    REFLECT(DataTest)
    REFLECT_MEMBER(DataTest, x);
    REFLECT_MEMBER(DataTest, y);
    REFLECT_MEMBER(DataTest, z);
};

class SerializedResource : public Resource
{
public:


    DEFAULT_COPY_MOVE_OPERATIONS(SerializedResource)
    
    SerializedResource()
    {
        DYNAMIC_REFLECT_INIT
    }

    SerializedResource(const std::string& name, DataTest dataTest) : Resource(name), m_DataTest(dataTest)
    {
        DYNAMIC_REFLECT_INIT;
    }

    IMP_DYNAMIC_REFLECT();
    
    
    DataTest m_DataTest;

private:
    REFLECT(SerializedResource, Resource)
    REFLECT_MEMBER(SerializedResource, m_DataTest);
};




TEST(Serialization, SerializationRes)
{
    const DataTest dataTest =
        {
        .x = 1,
        .y = 214.f,
        .z = 15616516
        };
    
    ResourceManager::Create<SerializedResource>("SerializedResource", dataTest);

    ResourceRef<SerializedResource> rRef = ResourceManager::Get<SerializedResource>("SerializedResource");
    Serializer::Serialize(*rRef.lock().get(), "SerializedResource.test");

    
    SerializedResource deserializedResource;
    Serializer::DeSerialize(&deserializedResource, "SerializedResource.test");


    std::shared_ptr<SerializedResource> d = rRef.lock();
    EXPECT_TRUE(d->name == deserializedResource.name);
    EXPECT_TRUE(d->guid == deserializedResource.guid);
    EXPECT_TRUE(d->guid == deserializedResource.guid);
    
    EXPECT_EQ(d->m_DataTest.x, deserializedResource.m_DataTest.x);
    EXPECT_FLOAT_EQ(d->m_DataTest.y , deserializedResource.m_DataTest.y);
    EXPECT_EQ(d->m_DataTest.z , deserializedResource.m_DataTest.z);
}

REFLECT(std::vector<Tbx::Vector3f>)


TEST(Serialization, VectorTrivial)
{
    std::vector<Tbx::Vector3f> m_vertexPositions;

    m_vertexPositions.emplace_back(1.f, 2.f, 3.f);
    m_vertexPositions.emplace_back(4.f, 5.f, 6.f);
    m_vertexPositions.emplace_back(7.f, 8.f, 9.f);
    
    Serializer::Serialize(m_vertexPositions, "SerializedResourceVector.test");

    std::vector<Tbx::Vector3f> vec;
    Serializer::DeSerialize(&vec, "SerializedResourceVector.test");
    EXPECT_EQ(vec.size(), m_vertexPositions.size());

    for (size_t i = 0 ; i < vec.size() ; i++)
    {
        EXPECT_FLOAT_EQ(vec[i].x, m_vertexPositions[i].x);
        EXPECT_FLOAT_EQ(vec[i].y, m_vertexPositions[i].y);
        EXPECT_FLOAT_EQ(vec[i].z, m_vertexPositions[i].z);
    }
}


REFLECT(std::vector<SerializedResource>)

TEST(Serialization, VectorNotTrivial)
{
    std::vector<SerializedResource> resourceVector;

    static_assert(!std::is_trivially_constructible_v<SerializedResource>, "SerializedResource shouldn't be trivially_constructible");

    DataTest d = 
    {
        .x = 1,
        .y = 214.f,
        .z = 15616516
    };

    DataTest d2 = 
    {
        .x = 255,
        .y = 156.654f,
        .z = 56451646
    };
    DataTest d3 = 
  {
        .x = 156,
        .y = 156363,
        .z = 4666
    };
    
    resourceVector.push_back(SerializedResource("1",d));
    resourceVector.push_back(SerializedResource("2",d2));
    resourceVector.push_back(SerializedResource("3",d3));

    Serializer::Serialize(resourceVector, "SerializedResourceVector.test");

    std::vector<SerializedResource> deserializedResource;
    Serializer::DeSerialize(&deserializedResource, "SerializedResourceVector.test");

    EXPECT_EQ(resourceVector.size(), deserializedResource.size());

    for (size_t i = 0 ; i < deserializedResource.size() ; i++)
    {
        EXPECT_TRUE(resourceVector[i].name == deserializedResource[i].name);
        EXPECT_TRUE(resourceVector[i].guid == deserializedResource[i].guid);
    
        EXPECT_EQ(resourceVector[i].m_DataTest.x, deserializedResource[i].m_DataTest.x);
        EXPECT_FLOAT_EQ(resourceVector[i].m_DataTest.y , deserializedResource[i].m_DataTest.y);
        EXPECT_EQ(resourceVector[i].m_DataTest.z , deserializedResource[i].m_DataTest.z);

        // this data is not reflect and so not serialize
        // this should be equal because of the call of the contructor
        EXPECT_EQ(resourceVector[i].GetTypeKey(), deserializedResource[i].GetTypeKey());
    }

}

REFLECT(std::unordered_map<uint32_t, DataTest >)

TEST(Serialization, MapTrivial)
{
    std::unordered_map<uint32_t, DataTest > map;

    std::pair<uint32_t, DataTest > d;



    map[0] = 
    {
        .x = 1,
        .y = 214.f,
        .z = 15616516
    };
    map[1] = {
        .x = 2,
        .y = 25334.6458f,
        .z = 15616516
    };
    map[2] = {
        .x = 3,
        .y = 25334.6458f,
        .z = 2222
    };
    map[3] = {
        .x = 4,
        .y = 45.f,
        .z = 156151+6
    };

    Serializer::Serialize(map, "SerializedResourceUnordoredMap.test");

    std::unordered_map<uint32_t, DataTest > map2;
    Serializer::DeSerialize(&map2, "SerializedResourceUnordoredMap.test");

    EXPECT_EQ(map.size(), map2.size());

    for (size_t i = 0; i < map2.size(); i++)
    {
        DataTest d = map.at(i);
        DataTest d2 = map2.at(i);

        EXPECT_EQ(d.x, d2.x);
        EXPECT_FLOAT_EQ(d.y, d2.y);
        EXPECT_EQ(d.z, d2.z);
    }

}

REFLECT(std::bitset<100>);

TEST(Serialization, BiteSet)
{
    std::bitset<100> bitset;

    for (size_t i = 0; i < bitset.size(); i++)
    {
        bitset.set(i, i % 2);
    }

    Serializer::Serialize(bitset, "SerializedResourceBitSet.test");

    std::bitset<100> bitset2;
    Serializer::DeSerialize(&bitset2, "SerializedResourceBitSet.test");

    EXPECT_EQ(bitset,  bitset2);
}

class TestISerizableClass : public ISeriazable
{
public:
  
    
    ~TestISerizableClass() override = default;

    TestISerizableClass()
    {
        DYNAMIC_REFLECT_INIT
    }

    IMP_DYNAMIC_REFLECT()

private:
    REFLECT(TestISerizableClass)
    
};


TEST(Serialization, ISeriazable)
{
    
}

