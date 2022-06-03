#pragma once
#include "TextureBuffer.h"

class FrameBuffer
{
	unsigned int m_FrameID;
public:
	FrameBuffer();
	~FrameBuffer();

	void AttachTexture(TextureBuffer& textureBuffer);

	void Bind() const;
	void Unbind() const;
};
