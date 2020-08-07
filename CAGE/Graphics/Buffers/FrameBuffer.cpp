#include "FrameBuffer.hpp"

namespace cage
{
	FrameBuffer::FrameBuffer()
	{
		glGenFramebuffers(1, &m_fboID);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_fboID);
	}

	void FrameBuffer::Bind()
	{
		Bind(GL_FRAMEBUFFER);
	}

	void FrameBuffer::Bind(GLenum bufferTarget)
	{
		glBindFramebuffer(bufferTarget, m_fboID);
	}

	void FrameBuffer::BindRead()
	{
		Bind(GL_READ_FRAMEBUFFER);
	}

	void FrameBuffer::BindWrite()
	{
		Bind(GL_DRAW_FRAMEBUFFER);
	}
}