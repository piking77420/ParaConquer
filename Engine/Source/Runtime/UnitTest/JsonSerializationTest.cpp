#include <map>
#include <gtest/gtest.h>

#include "Reflection/Reflector.hpp"
#include "Resources/Resource.hpp"
#include "Resources/ResourceManager.hpp"
#include "Serialize/JsonSerializer.hpp"
#include "DataStructure/SpareSet.hpp"
#include <Math/ToolBoxTypeDef.hpp>

using namespace PC_CORE;


ResourceManager ResourceManagerInstance;


enum TestEnum : uint8_t
{
    Red,
    Green,
    Blue,
};
REFLECT(TestEnum)

struct SerializaStruct
{
    float x;

    char y;

    uint64_t z;

    std::array<float, 4> w;

    float array[4];

    Tbx::Quaternionf q;

    TestEnum testEnum;
    
    REFLECT(SerializaStruct)
    REFLECT_MEMBER(SerializaStruct, x);
    REFLECT_MEMBER(SerializaStruct, y);
    REFLECT_MEMBER(SerializaStruct, z);

    REFLECT_MEMBER(SerializaStruct, w);
    REFLECT_MEMBER(SerializaStruct, array);
    REFLECT_MEMBER(SerializaStruct, q);

    REFLECT_MEMBER(SerializaStruct, testEnum);


};
const SerializaStruct serializaStruct1 =
    {
    .x = 1.214f,
    .y = 'd',
    .z = std::numeric_limits<uint64_t>::max(),
    .w = {123.f, 0.24531684f, 1200.0f, 0356.f},
    .array = {1656948.f, 0.21366354f, static_cast<float>(M_PI), static_cast<float>(-M_PI)},
    .q = Tbx::Quaternionf::Identity(),
    .testEnum = TestEnum::Blue
    };


TEST(Serialization, BasicSerialization)
{

    JsonSerializer s;
    {
        s.OpenFile("BasicSerialization.test", Serializer::SerializeOperation::Serialize);
        s.Serialize<SerializaStruct>(serializaStruct1);
        s.CloseFile();
    }
    
    SerializaStruct serializaStruct2;
    {
        s.OpenFile("BasicSerialization.test", Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<SerializaStruct>(&serializaStruct2);
        s.CloseFile();
    }
    
    EXPECT_FLOAT_EQ(serializaStruct1.x, serializaStruct2.x);
    EXPECT_EQ(serializaStruct1.y, serializaStruct2.y);
    EXPECT_EQ(serializaStruct1.z, serializaStruct2.z);

    for (size_t i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ(serializaStruct1.w[i], serializaStruct2.w[i]);
    }

    for (size_t i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ(serializaStruct1.array[i], serializaStruct2.array[i]);
    }

    EXPECT_EQ(serializaStruct1.q, serializaStruct2.q);
    EXPECT_EQ(serializaStruct1.testEnum, serializaStruct2.testEnum);

}

struct DataTest
{
    int x;
    float y;
    uint64_t z;

    auto operator<=>(const DataTest&) const = default;
    
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

    IMP_DYNAMIC_REFLECT()
    
    ~SerializedResource() override = default;
    
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

    WeakObjectPtr<SerializedResource> rRef = ResourceManager::Get<SerializedResource>();
    JsonSerializer s;
    {
        s.OpenFile("SerializedResource", PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<SerializedResource>(*rRef.Lock().get());
        s.CloseFile();
    }
    

    SerializedResource deserializedResource;
    {
        s.OpenFile("SerializedResource", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<SerializedResource>(&deserializedResource);
        s.CloseFile();
    }
    

    std::shared_ptr<SerializedResource> d = rRef.lock();
    EXPECT_TRUE(d->Name == deserializedResource.Name);
    EXPECT_TRUE(d->GetGuid() == deserializedResource.GetGuid());
    EXPECT_TRUE(d->GetGuid() == deserializedResource.GetGuid());
    
    EXPECT_EQ(d->m_DataTest.x, deserializedResource.m_DataTest.x);
    EXPECT_FLOAT_EQ(d->m_DataTest.y , deserializedResource.m_DataTest.y);
    EXPECT_EQ(d->m_DataTest.z , deserializedResource.m_DataTest.z);
}

REFLECT(std::vector<Tbx::Vector3f>)


TEST(Serialization, VectorTrivial)
{
    JsonSerializer s;

    std::vector<Tbx::Vector3f> m_vertexPositions;

    {
        s.OpenFile("SerializedResourceVector.test", PC_CORE::Serializer::SerializeOperation::Serialize);
        m_vertexPositions.emplace_back(1.f, 2.f, 3.f);
        m_vertexPositions.emplace_back(4.f, 5.f, 6.f);
        m_vertexPositions.emplace_back(7.f, 8.f, 9.f);

        s.Serialize<std::vector<Tbx::Vector3f>>(m_vertexPositions);
        s.CloseFile();
    }
    std::vector<Tbx::Vector3f> vec;
    {
        s.OpenFile("SerializedResourceVector.test", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<std::vector<Tbx::Vector3f>>(&vec);
        s.CloseFile();
    }

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

    JsonSerializer s;
    {
        s.OpenFile("SerializedResourceVector.test", PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<std::vector<SerializedResource>>(resourceVector);
        s.CloseFile();
    }

    std::vector<SerializedResource> deserializedResource;
    {
        s.OpenFile("SerializedResourceVector.test", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<std::vector<SerializedResource>>(&deserializedResource);
        s.CloseFile();
    }

    EXPECT_EQ(resourceVector.size(), deserializedResource.size());

    for (size_t i = 0 ; i < deserializedResource.size() ; i++)
    {
        EXPECT_TRUE(resourceVector[i].Name == deserializedResource[i].Name);
        EXPECT_TRUE(resourceVector[i].GetGuid() == deserializedResource[i].GetGuid());
    
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
    JsonSerializer s;
    {
        s.OpenFile("SerializedResourceUnordoredMap.test", PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<std::unordered_map<uint32_t, DataTest >>(map);
        s.CloseFile();
    }

    std::unordered_map<uint32_t, DataTest > map2;
    {
        s.OpenFile("SerializedResourceUnordoredMap.test", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<std::unordered_map<uint32_t, DataTest >>(&map2);
        s.CloseFile();
    }
    
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
    JsonSerializer s;

    std::bitset<100> bitset;

    for (size_t i = 0; i < bitset.size(); i++)
    {
        bitset.set(i, i % 2);
    }
    {
        s.OpenFile("SerializedResourceBitSet.test", PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<std::bitset<100>>(bitset);
        s.CloseFile();
    }
    std::bitset<100> bitset2;

    {
        s.OpenFile("SerializedResourceBitSet.test", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<std::bitset<100>>(&bitset2);
        s.CloseFile();
    }

    EXPECT_EQ(bitset,  bitset2);
}

REFLECT(SpareSet<DataTest>);


TEST(Serialization, SpareSet)
{

    SpareSet<DataTest> spareSet;
    spareSet.Add(5, DataTest(1,2,3));
    spareSet.Add(3, DataTest(4,5,6));
    spareSet.Add(0, DataTest(7,8,9));

    JsonSerializer s;
    

    {
        s.OpenFile("SpareSet.test", PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<SpareSet<DataTest>>(spareSet);
        s.CloseFile();
    }
    SpareSet<DataTest> spareSet2;

    {
        s.OpenFile("SpareSet.test", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<SpareSet<DataTest>>(&spareSet2);
        s.CloseFile();
    }
  

    EXPECT_EQ(spareSet2[5],spareSet[5]);
    EXPECT_EQ(spareSet2[3], spareSet[3]);
    EXPECT_EQ(spareSet2[0], spareSet[0]);

    
}

REFLECT(std::filesystem::path)

TEST(TestReflection, FileSystemPath)
{
    auto path = std::filesystem::current_path();

    JsonSerializer s;
    {
        s.OpenFile("FileSytem.test", PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<std::filesystem::path>(path);
        s.CloseFile();
    }
    std::filesystem::path path2;

    {
        s.OpenFile("FileSytem.test", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<std::filesystem::path>(&path2);
        s.CloseFile();
    }
    EXPECT_TRUE(path == path2);
}

struct Header
{
    std::string name;
    PC_CORE::Guid guid;

    REFLECT(Header);
    REFLECT_MEMBER(Header, name);
    REFLECT_MEMBER(Header, guid);

};

TEST(Serialization, MutlipleObjectSerializationRes)
{
    Header h1{ "header", Guid::New() };
    SerializaStruct data1 = serializaStruct1;


    JsonSerializer s;
    {
        s.OpenFile("MutlipleObjectSerializationRes.test", PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<Header, SerializaStruct>(h1, data1);
        s.CloseFile();
    }
    Header h2;
    SerializaStruct data2;
    {
        s.OpenFile("MutlipleObjectSerializationRes.test", PC_CORE::Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<Header, SerializaStruct>(&h2, &data2);
        s.CloseFile();
    }

    EXPECT_TRUE(h1.name == h2.name);
    EXPECT_TRUE(h1.guid == h2.guid);
    EXPECT_FLOAT_EQ(data1.x, data2.x);
    EXPECT_EQ(data1.y, data2.y);
    EXPECT_EQ(data1.z, data2.z);

    for (size_t i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ(data1.w[i], data2.w[i]);
    }

    for (size_t i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ(data1.array[i], data2.array[i]);
    }

    EXPECT_EQ(data1.q, data2.q);
    EXPECT_EQ(data1.testEnum, data2.testEnum);

}