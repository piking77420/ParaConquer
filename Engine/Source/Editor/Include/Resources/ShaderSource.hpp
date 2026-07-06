#pragma once

#include "EditorHeader.hpp"
#include "Resources/Resource.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Rendering/ShaderProgram.hpp"


BEGIN_EDITOR_PCCORE

    enum ShaderFeature : uint64_t {
        None = 0,
        Lit = 1ull << 0,
        UseUV = 1ull << 1,
        UseNormalMap = 1ull << 2,
        UseColor = 1ull << 3,
        DrawTriangle = 1ull << 4,
        Instanced = 1ull << 5,
        Frustum = 1ull << 6,
        SkyboxForceFarDepth = 1ull << 7,

        Last
    };
    static_assert(static_cast<uint64_t>(ShaderFeature::Last) <= (1ull << 63));

    // after pipiline cache
    /*enum class ShaderFeature : uint64_t {
    None = 0,

    Lit = 1ull << 0,
    UseUV = 1ull << 1,
    UseNormal = 1ull << 2,
    UseTangent = 1ull << 3,
    UseColor = 1ull << 4,

    UseAlbedoMap = 1ull << 5,
    UseNormalMap = 1ull << 6,
    UseORMMap = 1ull << 7,
    UseAOMap = 1ull << 8,
    UseEmissiveMap = 1ull << 9,

    AlphaCutoff = 1ull << 10,
    DoubleSided = 1ull << 11,

    Instanced = 1ull << 12,
    Skinned = 1ull << 13,

    DrawFullscreenTriangle = 1ull << 14,

    DepthOnly = 1ull << 15,
    ShadowCaster = 1ull << 16,
    ReceiveShadows = 1ull << 17,

    IBL = 1ull << 18,
    Skybox = 1ull << 19,
    SkyboxForceFarDepth = 1ull << 20,
    };
    */

    using ShaderFeatureFlags = uint64_t;

    class ShaderSource : public PC_CORE::Resource
    {
    public:
        explicit ShaderSource();

        explicit ShaderSource(const std::string& _name, ShaderFeatureFlags _ShaderFeatureFlag = 0);

        explicit ShaderSource(const std::string& _name, const std::filesystem::path& path, ShaderFeatureFlags _ShaderFeatureFlag = 0);

        ~ShaderSource() override = default;

        IMP_DYNAMIC_REFLECT()

        void Reload() override;

        bool GetCompiledShaderSource(std::vector<uint32_t>* _buffer, const std::string& _BinaryShaderFileTarget);

        ShaderFeatureFlags GetShaderFeatures() const
        {
            return m_ShaderFeatureFlag;
        }

    private:
        PC_CORE::RhiPipeline::ShaderStageTypeBits m_ShaderType;

        std::filesystem::path m_PathToSource;

        ShaderFeatureFlags m_ShaderFeatureFlag = 0;

        std::vector<std::wstring> GetDefineFromShaderFeatures() const;

        std::string GetShaderBinaryPath();

        static void AddPreProcessorDefVulkan();
    };

    REFLECT(ShaderSource)

END_PCCORE
