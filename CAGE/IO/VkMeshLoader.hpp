#pragma once

#include <Graphics/Models/VkMesh.hpp>

namespace cage
{
	namespace io
	{
		graphics::VkMesh* LoadMesh(const std::string& path, graphics::Device& device);
		std::vector<graphics::VkMesh*> LoadModel(const std::string& path, graphics::Device& device);
	}
}