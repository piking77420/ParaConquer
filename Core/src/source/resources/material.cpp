#include "resources/material.hpp"

#include "rendering/render_harware_interface/RHI.hpp"
#include "app.hpp"

#include <utility>
using namespace PC_CORE;




void Material::CreateMaterialInstance(MaterialInstance* _materialInstance)
{
    if (_materialInstance->name.empty())
    {
        PC_LOGERROR("CreateMaterialInstance _materialInstance name is empty")
        return;
    }
    
    if (m_MaterialInstanceMapToIndex.contains(_materialInstance->name))
    {
        PC_LOGERROR("There is alrealy a materialInstance with this name")
    }

    size_t index = m_MaterialInstanceDescriptor.size();
    m_MaterialInstanceMapToIndex.insert({ _materialInstance->name , index});
    m_MaterialInstanceDescriptor.resize(index + 1);

    std::array<DescriptorSetHandle, MAX_FRAMES_IN_FLIGHT>& descritptorSet = m_MaterialInstanceDescriptor[index];
    constexpr uint32_t descritptorSetLayoutNbr = 1;

    CreateDescriptorSet(descritptorSetLayoutNbr ,descritptorSet.data(), static_cast<uint32_t>(descritptorSet.size()));
    
    _materialInstance->materialRef = this;
}

PC_CORE_API void Material::DestroyMaterialInstace(MaterialInstance* _materialInstance)
{
    if (!m_MaterialInstanceMapToIndex.contains(_materialInstance->name))
    {
        PC_LOGERROR("There is no a materialInstance with this name to destroy")
    }

    size_t index = m_MaterialInstanceMapToIndex.at(_materialInstance->name);

    if (m_MaterialInstanceMapToIndex.size() == 1)
    {
        RHI::GetInstance().FreeDescriptorSet(name, m_MaterialInstanceDescriptor[index].data(), static_cast<uint32_t>(m_MaterialInstanceDescriptor[index].size()));
        m_MaterialInstanceDescriptor.clear();
        m_MaterialInstanceMapToIndex.erase(_materialInstance->name);
    }


    std::unordered_map<std::string, uint32_t>::iterator lastPairInVector = std::find_if(
        m_MaterialInstanceMapToIndex.begin(), m_MaterialInstanceMapToIndex.end(), [&](const auto& pair)
        {
            return pair.first == _materialInstance->name;
        }
    );

    if (lastPairInVector == m_MaterialInstanceMapToIndex.end())
    {
        PC_LOGERROR("Error while DestroyMaterialInstace")
        return;
    }

    FreeDescriptorSet(m_MaterialInstanceDescriptor[index].data(), static_cast<uint32_t>(m_MaterialInstanceDescriptor[index].size()));
    std::swap(m_MaterialInstanceDescriptor[index], m_MaterialInstanceDescriptor[lastPairInVector->second]);
    lastPairInVector->second = static_cast<uint32_t>(index);

    m_MaterialInstanceDescriptor.pop_back();
    m_MaterialInstanceMapToIndex.erase(_materialInstance->name);

    _materialInstance->materialRef = nullptr;
}

DescriptorSetHandle Material::GetDesciptorSet(const MaterialInstance& _materialInstance) const
{
    if (!m_MaterialInstanceMapToIndex.contains(_materialInstance.name))
    {
        PC_LOGERROR("There is no a materialInstance with this name to destroy")
    }

    const uint32_t imageIndex = RHI::GetInstance().GetCurrentImageIndex();

    const size_t index = m_MaterialInstanceMapToIndex.at(_materialInstance.name);
    return m_MaterialInstanceDescriptor[index].at(imageIndex);
}

void Material::UpdateMaterialInstance(const MaterialInstance& _materialInstance, DescriptorWriteSet* _descriptorWriteSets, uint32_t descriptorCount, uint32_t imageIndex) const
{
    if (!m_MaterialInstanceMapToIndex.contains(_materialInstance.name))
    {
        PC_LOGERROR("There is no a materialInstance with this name to destroy")
    }

    const size_t index = m_MaterialInstanceMapToIndex.at(_materialInstance.name);
    _descriptorWriteSets->dstDescriptorSetHandle = m_MaterialInstanceDescriptor[index][imageIndex];
    
    RHI::GetInstance().UpdateDescriptorSet(descriptorCount, _descriptorWriteSets);
}


Material::Material(const ProgramShaderCreateInfo& _createInfo, const std::vector<ShaderSource*>& _shaderSources) : ShaderProgram(_createInfo
                                                                                                                                 , _shaderSources)
{

}


