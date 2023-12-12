#pragma once

#include <SDL2/SDL.h>
#include <utility>
#include <Graphics/Context/Device.hpp>
#include "../Buffers/Buffer.hpp"

namespace cage
{
	namespace graphics
	{
		// A CAGE texture
		class VkTexture
		{
			friend class Renderer;

			unsigned int m_id;
			int m_textureUnit;
			std::pair <unsigned int, unsigned int> m_size;
			SDL_Surface* m_surface;

			void allocate(vk::DeviceSize size);
			//GLenum getGLPixelFormat(SDL_PixelFormat* sdlFormat);
			void uploadTexData(SDL_Surface* surface);

			Buffer* m_stagingBuffer;
			vk::DeviceMemory m_texMemory;
			vk::Image m_image;
			vk::ImageView m_view;

		public:
			// Contructs a texture from the given SDL_Surface. If the texture does not keep a local copy of the surface, it will be automatically freed upon construction
			VkTexture(SDL_Surface* textureData, bool keepLocalCopy = false);
			~VkTexture();

			inline std::pair<unsigned int, unsigned int> GetSize() const { return m_size; }

			void RecordTransfer(vk::CommandBuffer& cmd);
			void CreateDescriptorSet(vk::DescriptorPool pool, vk::DescriptorSetLayout);

			inline vk::ImageView GetView() const { return m_view; }

			vk::DescriptorSet m_DescriptorSet;

			static Device* s_Device;
			static VkTexture* s_MissingTexture;

		};
	}
}