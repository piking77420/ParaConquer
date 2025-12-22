#include "LowRenderer/RhiResourceUpdate.hpp"

#include "LowRenderer/CommandList.hpp"

namespace PC_CORE::Rhi
{

ResourceUpdate::ResourceUpdate()
{

}

ResourceUpdate::~ResourceUpdate()
{

}

void ResourceUpdate::Record(CommandList& _CommandList)
{

}


void Texture2DUploadUpdate::Record(CommandList& _CommandList)
{
	m_Texture.UploadData2D()
}

}
