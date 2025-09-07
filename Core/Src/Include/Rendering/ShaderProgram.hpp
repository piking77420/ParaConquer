#pragma once

#include <array>
#include <vector>
#include <string>

#include "CoreHeader.hpp"
#include "Objectptr.hpp"
#include "Reflection/Reflector.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "LowRenderer/RhiRenderPass.hpp"
#include "LowRenderer/RhiShaderProgram.hpp"
#include "LowRenderer/Vertex.hpp"
#include "Resources/ShaderSourceBinary.hpp"

BEGIN_PCCORE

    using SourceList = std::vector<std::pair<ShaderStageType, WeakObjectPtr<ShaderSourceBinary>>>;

    // MAKE COMPUTE RAY AND GRAPHIC PROGRAMM
    // TO DO MAY SEPARATE RESOURCE AND GPU RESOURCE
    class ShaderProgram : public Resource , public IGpuResource
    {
    public:

        PC_CORE_API virtual std::shared_ptr<RhiResource> GetRhiHandle() const
        {
            return m_RhiShaderProgram;
        }

        PC_CORE_API void OnParentReload(const Guid& _parentGuid) override;

        ShaderProgramPipelineType GetPipelineType() const
        {
            return m_ShaderProgramPipelineType;
        } 

        // TODO ABSTRACT THIS 
        PC_CORE_API void AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets, size_t set);
        // TODO ABSTRACT THIS 
        PC_CORE_API void FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets);

        IMP_DYNAMIC_REFLECT()
    
        PC_CORE_API ShaderProgram(const std::string& _shaderName,
            ShaderProgramPipelineType _shaderProgramPipelineType, const std::vector<std::pair<ShaderStageType, WeakObjectPtr<ShaderSourceBinary>>>& _sources);

        PC_CORE_API ShaderProgram(const std::string& _shaderName,
            ShaderProgramPipelineType _shaderProgramPipelineType, const WeakObjectPtr<ShaderSourceBinary>& _source);

        PC_CORE_API ShaderProgram(std::string&& _shaderName, ShaderProgramPipelineType _shaderProgramPipelineType, const WeakObjectPtr<ShaderSourceBinary>& _source);
        
        PC_CORE_API ShaderProgram()
        {
            DYNAMIC_REFLECT_INIT
        }

        PC_CORE_API virtual ~ShaderProgram() override
        {
            if (m_RhiShaderProgram.use_count() > 1)
            {
                PC_LOGERROR("There is still a reference to the m_RhiShaderProgram, {}", name)
            }


            PC_LOG("Destroy ShaderProgram, {}", name);
        }

    protected:
        std::shared_ptr<RhiShaderProgram> m_RhiShaderProgram;

        ShaderProgramPipelineType m_ShaderProgramPipelineType;

        static std::vector<ShaderModule> SourceListToShaderModules(const SourceList& _sourceList);
    
        REFLECT(ShaderProgram, Resource);

    
    };

END_PCCORE
