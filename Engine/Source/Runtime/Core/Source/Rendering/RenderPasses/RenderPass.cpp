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
	case 0: // Forward +Z
		Direction = Tbx::Vector3f::UnitZ();
		Up = Tbx::Vector3f::UnitY();
		break;

	case 1: // Backward -Z
		Direction = -Tbx::Vector3f::UnitZ();
		Up = Tbx::Vector3f::UnitY();
		break;

	case 2: // Up +Y
		Direction = Tbx::Vector3f::UnitY();
		Up = Tbx::Vector3f::UnitZ();
		break;

	case 3: // Down -Y
		Direction = -Tbx::Vector3f::UnitY();
		Up = Tbx::Vector3f::UnitZ();
		break;

	case 4: // Right +X
		Direction = Tbx::Vector3f::UnitX();
		Up = Tbx::Vector3f::UnitY();
		break;

	case 5: // Left -X
		Direction = -Tbx::Vector3f::UnitX();
		Up = Tbx::Vector3f::UnitY();
		break;

	default:
		assert(false);
		Direction = Tbx::Vector3f::UnitZ();
		Up = Tbx::Vector3f::UnitY();
		break;
	}

	return Tbx::LookAtRH(_Eye, _Eye + Direction, Up);
}

