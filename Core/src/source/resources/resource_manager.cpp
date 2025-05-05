#include "resources/resource_manager.hpp"

#include "stb_image.h"
#include "rendering/material.hpp"
#include "resources/mesh.hpp"
#include "resources/texture.hpp"

#include "serialize/serializer.h"

using namespace PC_CORE;

static constexpr bool CreateBasicsResource = true;

#define RESSOURCE_MAP_FILE "paraconquer_resource.res"


void ResourceManager::InitPath()
{
    if constexpr (CreateBasicsResource)
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

    }
    else
    {
        Serializer::DeSerialize(&m_ResourcesMap, RESSOURCE_MAP_FILE);
        for (auto& it : m_ResourcesMap)
        {
            it.second->Build();
            assert(it.second.use_count() != 0);
        }
    }
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
    //std::ofstream myfile(RESSOURCE_MAP_FILE);
    /*
    using BaseMap = std::unordered_map<std::string, std::shared_ptr<Resource>>;
    

    auto funcPtr = Reflector::m_UnordoredMapReflectFunction.at(Reflector::GetTypeKey< BaseMap>());
    std::unordered_map<uint8_t, uint8_t>& mapRef = *reinterpret_cast<std::unordered_map<uint8_t, uint8_t>*>(&m_ResourcesMap);

    using ByteMap = std::unordered_map<uint8_t, uint8_t>;
    using ByteIter = ByteMap::iterator;

   using InsertFunction = ByteMap::mapped_type& (ByteMap::*)(const typename ByteMap::key_type&);

   InsertFunction inser = *reinterpret_cast<InsertFunction*>(&funcPtr.insertFunction);

   std::string caca = "caca";
   std::shared_ptr<Resource> r = std::make_shared<Material>("qsdq");

   using InsertIteratorFuncTypeBase = BaseMap::mapped_type& (BaseMap::*)(const typename BaseMap::key_type&);
   InsertIteratorFuncTypeBase baseFunInsert = static_cast<BaseMap::mapped_type & (BaseMap::*)(const typename BaseMap::key_type&)>(&BaseMap::operator[]);
   uint64_t baseFunInserUnint = *((uint64_t*)(&baseFunInsert));
   assert(baseFunInserUnint == funcPtr.insertFunction);

   //m_ResourcesMap[caca] = r;
   auto* ref = &(mapRef.*inser)(reinterpret_cast<uint8_t&>(caca));

   std::shared_ptr<uint8_t[]>* ptr = reinterpret_cast<std::shared_ptr<uint8_t[]>*>(ref);
   // SOULD BE THE MEMBER CREATE FUNCTION
   *ptr = std::reinterpret_pointer_cast<uint8_t[]>(r);
   
   assert(m_ResourcesMap.contains("caca"));
   assert(ptr->use_count() == 2);

   auto r2 = m_ResourcesMap.at("caca");*/

    Serializer::Serialize(m_ResourcesMap, RESSOURCE_MAP_FILE);
}


