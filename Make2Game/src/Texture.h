#pragma once

#include "Renderer.h"

class Texture
{
private:
	uint32_t m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned char slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeigth() const { return m_Height; }
	inline uint32_t GetId() const { return m_RendererID; };
};
