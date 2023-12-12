#pragma once

#include <SDL2/SDL.h>
#include "../Buffers/VkVertexBuffer.hpp"

namespace cage
{
	namespace graphics
	{
		class VkMesh
		{
		public:

			VkVertexBuffer m_VertexBuffer;
			int m_VertexCount;
			int m_MeshID;
			SDL_Surface* m_Texture;

			VkMesh(Device& device) : m_VertexBuffer(false, device), m_VertexCount(0), m_MeshID(-1), m_Texture(nullptr)
			{

			}

			~VkMesh()
			{

			}
		};
	}
}