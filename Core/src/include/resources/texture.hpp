#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/gpu_handle.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "low_renderer/rhi_typedef.h"


BEGIN_PCCORE


class Texture : public ResourceInterface<Texture>
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(Texture)

    PC_CORE_API void Build() override;
    
    PC_CORE_API Texture() = default;

    PC_CORE_API Texture(const CreateTextureInfo& createTextureInfo);
    
    PC_CORE_API Texture(const fs::path& _path);

    PC_CORE_API ~Texture() override;
    
    PC_CORE_API void Load(const std::array<std::string,6>& _maps);
   
    PC_CORE_API  std::shared_ptr<GpuHandle> GetHandle() const;

    PC_CORE_API  std::shared_ptr<GpuHandle> GetHandle(size_t _frameIndex) const;

private:
    int m_TextureChannel = -1;

    std::array<std::shared_ptr<GpuHandle>, MAX_FRAMES_IN_FLIGHT> m_TextureHandles;

    PC_CORE_API void CreateFromCreateInfo(const CreateTextureInfo& createTextureInfo);

    PC_CORE_API void LoadFromFile(const fs::path& _path);
};

REFLECT(Texture, Resource)


END_PCCORE