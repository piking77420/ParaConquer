#include "resources/material.hpp"

#include "rendering/render_harware_interface/RHI.hpp"
#include "app.hpp"

#include <utility>
using namespace PC_CORE;




void Material::CreateMaterialInstance(const std::string& _materialIntance)
{
    if (m_MaterialInstanceMapToIndex.contains(_materialIntance))
    {
        PC_LOGERROR("There is alrealy a materialInstance with this name")
    }

    size_t index = m_MaterialInstanceDescriptor.size();
    m_MaterialInstanceMapToIndex.insert({ _materialIntance , index});
    m_MaterialInstanceDescriptor.resize(index + 1);

    std::array<DescriptorSetHandle, MAX_FRAMES_IN_FLIGHT>& descritptorSet = m_MaterialInstanceDescriptor[index];

    CreateDescriptorSet(1 ,descritptorSet.data(), static_cast<uint32_t>(descritptorSet.size()));
}

PC_CORE_API void Material::DestroyMaterialInstace(const std::string& _materialIntance)
{
    if (!m_MaterialInstanceMapToIndex.contains(_materialIntance))
    {
        PC_LOGERROR("There is no a materialInstance with this name to destroy")
    }

    size_t index = m_MaterialInstanceMapToIndex.at(_materialIntance);

    if (m_MaterialInstanceMapToIndex.size() == 1)
    {
        RHI::GetInstance().FreeDescriptorSet(m_Shader->GetName(), m_MaterialInstanceDescriptor[index].data(), static_cast<uint32_t>(m_MaterialInstanceDescriptor[index].size()));
        m_MaterialInstanceDescriptor.clear();
        m_MaterialInstanceMapToIndex.erase(_materialIntance);
    }
    

    std::unordered_map<std::string, uint32_t>::iterator lastPairInVector = std::find_if(m_MaterialInstanceMapToIndex.begin(), m_MaterialInstanceMapToIndex.end(), [&](std::pair<std::string, uint32_t> const& a, std::pair<std::string, uint32_t> const& b)
        {
            return a.second > b.second;
        });

    if (lastPairInVector == m_MaterialInstanceMapToIndex.end())
    {
        PC_LOGERROR("Error while DestroyMaterialInstace")
        return;
    }

    FreeDescriptorSet(m_MaterialInstanceDescriptor[index].data(), static_cast<uint32_t>(m_MaterialInstanceDescriptor[index].size()));
    std::swap(m_MaterialInstanceDescriptor[index], m_MaterialInstanceDescriptor[lastPairInVector->second]);
    lastPairInVector->second = index;

    m_MaterialInstanceDescriptor.pop_back();
    m_MaterialInstanceMapToIndex.erase(_materialIntance);
}

void Material::BuildMaterialInstance(const std::string& _materialIntance, std::array<DescriptorWriteSet, MAX_FRAMES_IN_FLIGHT>& _descritproWrite)
{
    

    if (!m_MaterialInstanceMapToIndex.contains(_materialIntance))
    {
        PC_LOGERROR("There is no a materialInstance with this name to destroy")
    }

    const size_t index = m_MaterialInstanceMapToIndex.at(_materialIntance);
    std::array<DescriptorSetHandle, MAX_FRAMES_IN_FLIGHT>& descritproSet = m_MaterialInstanceDescriptor[index];

    for (size_t i = 0; i < descritproSet.size(); i++)
    {
        _descritproWrite[i].dstDescriptorSetHandle = descritproSet[i];
    }

    RHI::GetInstance().UpdateDescriptorSet(_descritproWrite.size(), _descritproWrite.data());
}



Material::Material(const ProgramShaderCreateInfo& _createInfo, const std::vector<ShaderSource*>& _shaderSources) : ShaderProgram(_createInfo
    , _shaderSources)
{

}


