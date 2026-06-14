#include <array>

#include "Rendering/RenderPasses/Renderpass.hpp"
#include "Resources/ResourceManager.hpp"

PC_CORE::Rendering::RenderPass::RenderPass()
{
	DYNAMIC_REFLECT_INIT
}

Tbx::Matrix4x4f PC_CORE::Rendering::RenderPass::GetLookAtMatrixFromCubeMapIndicies(size_t _Index, Tbx::Vector3f _Eye)
{
    assert(_Index < 6);

    Tbx::Vector3f Up;
    Tbx::Vector3f Direction;

    switch (_Index)
    {
    case 0: // +X
        Direction = Tbx::Vector3f::UnitX();
        Up = -Tbx::Vector3f::UnitY();
        break;

    case 1: // -X
        Direction = -Tbx::Vector3f::UnitX();
        Up = -Tbx::Vector3f::UnitY();
        break;

    case 2: // +Y
        Direction = Tbx::Vector3f::UnitY();
        Up = Tbx::Vector3f::UnitZ();
        break;

    case 3: // -Y
        Direction = -Tbx::Vector3f::UnitY();
        Up = -Tbx::Vector3f::UnitZ();
        break;

    case 4: // +Z
        Direction = Tbx::Vector3f::UnitZ();
        Up = -Tbx::Vector3f::UnitY();
        break;

    case 5: // -Z
        Direction = -Tbx::Vector3f::UnitZ();
        Up = -Tbx::Vector3f::UnitY();
        break;
    }

    return Tbx::LookAtRH(_Eye, _Eye + Direction, Up);
}

