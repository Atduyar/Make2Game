#include "TextureBuffer.h"

#include <GL/glew.h>
#include <GL/gl.h>

TextureBuffer::TextureBuffer(unsigned int w, unsigned int h)
	:m_width(w), m_height(h)
{
    glGenTextures(1, &m_TextureID);
    //glBindTexture(GL_TEXTURE_2D, m_TextureID);
    //Bind();
    ReSize(m_width, m_height);
}

TextureBuffer::~TextureBuffer()
{
    glDeleteTextures(1, &m_TextureID);
}

void TextureBuffer::ReSize(unsigned int w, unsigned int h)
{
    m_width = w;
    m_height = h;

	Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TextureBuffer::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void TextureBuffer::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int TextureBuffer::GetId() const
{
    return m_TextureID;
}

unsigned TextureBuffer::GetWidth() const
{
    return m_width;
}

unsigned TextureBuffer::GetHeight() const
{
    return m_height;
}
