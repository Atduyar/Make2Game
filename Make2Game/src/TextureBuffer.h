#pragma once

class TextureBuffer
{
	unsigned int m_TextureID;
	unsigned int m_width;
	unsigned int m_height;
public:

	TextureBuffer(unsigned int x, unsigned int y);
	~TextureBuffer();
	
	void ReSize(unsigned int w, unsigned int h);

	void Bind() const;
	void Unbind() const;

	unsigned int GetId() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
};
