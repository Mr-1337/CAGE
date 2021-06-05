#pragma once

#include <Glad/glad/glad.h>

namespace cage
{
	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		void Bind();
		void Bind(GLenum bufferTarget);
		void BindRead();
		void BindWrite();

	private:
		unsigned int m_fboID;
	};
}