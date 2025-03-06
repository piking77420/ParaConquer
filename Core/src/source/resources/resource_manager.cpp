#include "resources/resource_manager.hpp"

#include "stb_image.h"
#include "rendering/material.hpp"
#include "resources/mesh.hpp"
#include "resources/texture.hpp"

#include "serialize/serializer.h"

using namespace PC_CORE;

void ResourceManager::InitPath()
{

    ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/rounded_cube.obj");
    ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/quad.obj");
    auto it = ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/sphere.obj");

    ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/viking_room.obj");
    ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/cube.obj");
    ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/untitled.obj");
    ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/suzanne.obj");


    auto it2 = ResourceManager::Create<Texture>((fs::path)"assets/textures/diamond_block.jpg");

    ResourceManager::Create<Texture>((fs::path)"assets/textures/emerauld_block.png");

    /*
    ResourceManager::Create<Texture>("assets/textures/viking_room.png");
    ResourceManager::Create<Texture>("assets/textures/skydom.png");*/

}

void ResourceManager::Destroy()
{
    SerializeResource();

    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {

        if (it->second.use_count() > 1)
        {
            PC_LOGERROR("There is a remaining reference before destroyed by the resource manager " + it->second->name);
        }
        it->second.reset();
        it->second = nullptr;
    }
    m_ResourcesMap.clear();
}

std::shared_ptr<Resource> ResourceManager::GetByGuid(const Guid& _guid)
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (it->second->guid == _guid)
            return it->second;
    }

    return nullptr;
}

void ResourceManager::ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba)
{
    if (!Reflector::ContaintTypeFromTypeID(typeID))
        return;
        
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        const std::shared_ptr<const ResourceInterface<Material>> interface = std::reinterpret_pointer_cast<const ResourceInterface<Material>>(it->second);
        
        if (typeID != interface->GetType().typeId)
            continue;
        
        _lamba(it->second);
    }
    
}

void ResourceManager::SerializeResource()
{
    //std::ofstream myfile(ResourceMapFile);

    using BaseMap = std::unordered_map<std::string, std::shared_ptr<Resource>>;
    

    auto funcPtr = Reflector::m_UnordoredMapReflectFunction.at(Reflector::GetTypeKey< BaseMap>());

    using ByteMap = std::unordered_map<uint8_t, uint8_t>;
    using ByteIter = ByteMap::iterator;

    using UnrefIterator = std::pair<const std::uint8_t, std::uint8_t>* (ByteIter::*)();
    using IncrementIterator = ByteIter & (ByteIter::*)();

    UnrefIterator unrefIterator = *reinterpret_cast<UnrefIterator*>(&funcPtr.unrefIterator);
    IncrementIterator incrementFunc = *reinterpret_cast<IncrementIterator*>(&funcPtr.incrementIterator);
    std::unordered_map<uint8_t, uint8_t>& mapRef = *reinterpret_cast<std::unordered_map<uint8_t, uint8_t>*>(&m_ResourcesMap);
   assert(mapRef.size() == m_ResourcesMap.size());


   using UnrefIteratorFuncTypeBase = std::pair<const std::string, std::shared_ptr<Resource>>* (BaseMap::iterator::*)() const;
   UnrefIteratorFuncTypeBase baseFunUnfer = &BaseMap::iterator::operator->;
   uint64_t baseFunUnferUnint = *((uint64_t*)(&baseFunUnfer));
   assert(baseFunUnferUnint == funcPtr.unrefIterator);

   using IncrementIteratorFuncTypeBase = BaseMap::iterator& (BaseMap::iterator::*)();
   IncrementIteratorFuncTypeBase baseFunIncrement = &BaseMap::iterator::operator++;
   uint64_t baseFunIncrementerUnint = *((uint64_t*)(&baseFunIncrement));
   assert(baseFunIncrementerUnint == funcPtr.incrementIterator);

   using InsertFunction = ByteMap::mapped_type& (ByteMap::*)(const typename ByteMap::key_type&);

   InsertFunction inser = *reinterpret_cast<InsertFunction*>(&funcPtr.insertFunction);

   std::string caca = "caca";
   std::shared_ptr<Resource> r = std::make_shared<Material>("qsdq");
   std::pair<std::string, std::shared_ptr<Resource>> addedPair = std::make_pair(caca, r);

   const std::pair<uint8_t, uint8_t>& pairPtr = reinterpret_cast<const std::pair<uint8_t, uint8_t>&>(addedPair);

   using InsertIteratorFuncTypeBase = BaseMap::mapped_type& (BaseMap::*)(const typename BaseMap::key_type&);
   InsertIteratorFuncTypeBase baseFunInsert = static_cast<BaseMap::mapped_type & (BaseMap::*)(const typename BaseMap::key_type&)>(&BaseMap::operator[]);
   uint64_t baseFunInserUnint = *((uint64_t*)(&baseFunInsert));
   assert(baseFunInserUnint == funcPtr.insertFunction);

   //m_ResourcesMap[caca] = r;
   auto* ref = &(mapRef.*inser)(reinterpret_cast<uint8_t&>(caca));

   std::shared_ptr<Resource>* rtmpr = reinterpret_cast<std::shared_ptr<Resource>*>(ref);
   *rtmpr = r;

   assert(m_ResourcesMap.contains("caca"));

   auto r2 = m_ResourcesMap.at("caca");

   auto it = mapRef.begin();
    while (it != mapRef.end())
    {
        auto* bytePair = (it.*unrefIterator)();

        std::pair<std::string, std::shared_ptr<Resource>>* pairWatchers = reinterpret_cast<std::pair<std::string, std::shared_ptr<Resource>>*>(bytePair);

        std::cout << pairWatchers->first.c_str() << " -> " << std::string(pairWatchers->second->guid) << std::endl;

        (it.*incrementFunc)();
    }

}


