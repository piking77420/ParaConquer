#include <Rendering/Material.hpp>
#include <ScopeGuard.hpp>

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include <LowRenderer/RhiRenderPass.hpp>
#include <Resources/StaticMesh.hpp>

namespace PC_CORE::Rendering
{

Material::Material()
{
    DYNAMIC_REFLECT_INIT
}

Material::Material(std::string _Name)
    : Resource(std::move(_Name))
{
    DYNAMIC_REFLECT_INIT
}

Material::~Material()
{

}

void Material::Build()
{
    PC_LOG_VERBOSE("Build Material Name {}", Name);

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    WritePso();

    m_RhiMaterialBuffer.reset(App::Instance->RenderHarwareInteface.CreateBuffer());
    m_RhiMaterialBuffer
        ->SetMemoryUsage(RhiMemoryUsage::CPUVisible) // may use static
        .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Dynamic)
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Uniform)
        .SetSizeInBytes(sizeof(Gpu::MaterialBuffer) * MaxFramesInFlight)
        .SetObjectSize(sizeof(Gpu::MaterialBuffer))
        .SetMaxObjectPerFrame(1)
        .SetName(Name + " UniformBuffer")
        .Build();


    m_RhiDescriptorSets.reset(App::Instance->RenderHarwareInteface.CreateDescriptorSet());
    m_RhiDescriptorSets->BindUniformBuffer(RhiShaderStageBits::Pixel, 0, m_RhiMaterialBuffer.get());

    const RhiSampler& sampler = (m_MaterialDomain == MaterialDomain::Opaque) ? *App::Instance->SamplerLinearReapet.get() : *App::Instance->SamplerLinearReapet.get();
    for (size_t i = 0; i < m_Textures.size(); i++)
    {
        const MaterialAttribute att = static_cast<MaterialAttribute>(i);

        // + 1 because of uniform buffer
        if (auto Text = m_Textures[i].Lock())
        {
            m_RhiDescriptorSets
                ->BindTexture(RhiShaderStageBits::Pixel, static_cast<uint32_t>(i) + 1, Text->Get(), &sampler);
        }
        else
        {
            m_RhiDescriptorSets
                ->BindTexture(RhiShaderStageBits::Pixel, static_cast<uint32_t>(i) + 1, App::Instance->DummyTexture.get(), &sampler);
        }
        
    }
    m_RhiDescriptorSets->SetName(Name + "DescriptorSet")
        .Build();

    Upload();
}

void Material::WritePso()
{
    for (auto& Pso : m_PipelineData)
    {
        Pso.Reset();
        MakeScopeGuard([&Pso]() {
            Pso.SetNeedRebuild(true);
            });

        PC_CORE::Rendering::PipelineCache::ModuleEntryList& List = Pso.ModuleList;

        List.Reserve(2);
        auto& vertex = List.Next();
        vertex.ShaderSourcePath = "/Shaders/TriangleBased.vs.hlsl";
        vertex.FeaturesFlags =
            PC_CORE::Rendering::ShaderFeature::Lit |
            PC_CORE::Rendering::ShaderFeature::UseUV |
            PC_CORE::Rendering::ShaderFeature::UseNormalMap;

        auto& fragment = List.Next();
        fragment.ShaderSourcePath = "/Shaders/Lit.ps.hlsl";
        fragment.FeaturesFlags =
            PC_CORE::Rendering::ShaderFeature::Lit |
            PC_CORE::Rendering::ShaderFeature::UseUV |
            PC_CORE::Rendering::ShaderFeature::UseNormalMap;


        RhiGraphicPipeline::Descriptor& Descritptor = Pso.GraphicPipelineDescriptor;
        constexpr PC_CORE::RhiGraphicPipeline::BlendState blenstate =
        {
            .ColorSrcFactor = PC_CORE::BlendFactor::SrcAlpha,
            .ColorDstFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
            .ColorOp = PC_CORE::BlendOp::Add,

            .AlphaSrcFactor = PC_CORE::BlendFactor::One,
            .AlphaDstFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
            .AlphaOp = PC_CORE::BlendOp::Add,

            .BlendMask = PC_CORE::ColorComponent::ColorComponentRGBA
        };
        switch (m_MaterialDomain)
        {
        case PC_CORE::Rendering::MaterialDomain::Opaque:
            Descritptor
                .SetCullMode(RhiGraphicPipeline::CullModeFlagBits::CullBack);
            break;
        case PC_CORE::Rendering::MaterialDomain::Transparent:
            Descritptor
                .SetBlendState(blenstate);
            break;
        default:
            break;
        }

        Descritptor
            .SetDepthTest(true)
            .SetDepthWrite(true)
            .SetVertexAttributeDescriptions(StaticMeshVertex::GetAttributeDescriptions(0))
            .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0) });

    }
}

void Material::Upload()
{
    Gpu::MaterialBuffer MaterialBufferData;
    PopulateGpuMaterial(MaterialBufferData);

    if (char* ptr = m_RhiMaterialBuffer->BeginFullDynamicBufferUpdateForCurrentFrame())
    {
        assert(m_RhiMaterialBuffer->GetStride() == m_RhiMaterialBuffer->GetFrameStride());
        for (size_t i = 0; i < MaxFramesInFlight; i++)
        {
            Gpu::MaterialBuffer* materialGpu = reinterpret_cast<Gpu::MaterialBuffer*>(ptr + (i * m_RhiMaterialBuffer->GetFrameStride()));
            *materialGpu = MaterialBufferData;
        }

        m_RhiMaterialBuffer->EndBufferUpdate();
    }
}

const RhiDescriptorSet* Material::GetDescriptorSet() const
{
    return m_RhiDescriptorSets.get();
}

size_t Material::GetMaterialStride() const
{
    return m_RhiMaterialBuffer->GetFrameStride();
}

void Material::PopulateGpuMaterial(Gpu::MaterialBuffer& _MaterialBuffer)
{
    for (size_t i = 0; i < m_Textures.size(); i++)
    {
        const MaterialAttribute att = static_cast<MaterialAttribute>(i);
        if (auto Text = m_Textures[i].Lock())
        {
            if (i >= 4)
            {
                _MaterialBuffer.TextureDescriptor2[i - 4] = 1;
            }
            else
            {
                _MaterialBuffer.TextureDescriptor[i] = 1;
            }
        }

        switch (att)
        {
        case MaterialAttribute::AlbedoFactors:
            std::memcpy(_MaterialBuffer.AlbedoFactors.data.data(), &m_Albedo.x, sizeof(_MaterialBuffer.AlbedoFactors.data));
            break;
        case MaterialAttribute::Normal:
            break;
        case MaterialAttribute::Emisive:
            std::memcpy(_MaterialBuffer.EmissiveFactor.data.data(), &m_Emmisive, sizeof(_MaterialBuffer.EmissiveFactor.data));
            break;
        case MaterialAttribute::AoRoughnessMetallic:
            _MaterialBuffer.AORoughnessMetallicFactors.data[0] = m_AmbiantOcclusion;
            _MaterialBuffer.AORoughnessMetallicFactors.data[1] = m_Roughness;
            _MaterialBuffer.AORoughnessMetallicFactors.data[2] = m_Metallic;
            break;
        case MaterialAttribute::Ao:
            break;
        default:
            break;
        }
    }
    _MaterialBuffer.UseAlpha = m_UseAlpha;
}

}
