#pragma once

#include <SDL2/SDL.h>
#include <utility>
#include <Glad/glad/glad.h>

namespace cage
{
	// A CAGE texture
	class Texture
	{
		int m_width, m_height;
		unsigned int m_id;
		int m_textureUnit;
		void* m_pixelData;

		void uploadTexData(SDL_Surface* surface);
		GLenum getGLPixelFormat(SDL_PixelFormat* sdlFormat);

	public:
		// Contructs a texture from the given SDL_Surface. If the texture does not keep a local copy of the surface, it will be automatically freed upon construction
		Texture(SDL_Surface* textureData, bool keepLocalCopy = false);

		inline std::pair<int, int> GetSize() const { return { m_width, m_height }; }

		virtual ~Texture();
		void Bind();

	};
}