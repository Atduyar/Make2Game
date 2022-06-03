#include "FrameBuffer.h"

#include <GL/glew.h>


FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &m_FrameID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameID);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_FrameID);
}

void FrameBuffer::AttachTexture(TextureBuffer& textureBuffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer.GetId(), 0);
}

void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameID);
}

void FrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
