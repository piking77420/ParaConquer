#pragma once
#include <String>

#include "CoreHeader.hpp"
#include "Resources/Texture2d.hpp"

BEGIN_PCCORE
    class Skybox
    {
    public:
        enum class SkyBoxType
        {
            CubeMap,
            Procedural
        };

        SkyBoxType skyboxType = SkyBoxType::CubeMap;

        //Texture2D* texture = nullptr;

        PC_CORE_API void Load(std::array<std::string, 6>& _maps);

        PC_CORE_API void Destroy();

        Skybox() = default;

        ~Skybox() = default;
    };

END_PCCORE
