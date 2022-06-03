#pragma once
#include "Texture.h"
#include "glm/vec2.hpp"


class SubTexturee
{
public:
	glm::vec2 imageCoord[4];
};

class SubTexture
{
public:
	glm::vec2 imageCoord[4];
	glm::vec2 size;
	static SubTexture& Create(glm::vec2 position, glm::vec2 size, const glm::vec2& imageSize);
	inline SubTexture() {}
	inline SubTexture(SubTexturee subTexturee) {
		imageCoord[0] = subTexturee.imageCoord[0];
		imageCoord[1] = subTexturee.imageCoord[1];
		imageCoord[2] = subTexturee.imageCoord[2];
		imageCoord[3] = subTexturee.imageCoord[3];
	}
};


class TextureAtlases
{
	Texture* m_Texture;
	glm::vec2 m_Size;
	glm::vec2 m_Position = { 0, 0 };
	glm::vec2 m_ImageSize;
	glm::vec2 m_StartPosition = { 0, 0 };
	glm::vec2 m_EndPosition;
public:
	std::vector<SubTexture> subTextures;

	TextureAtlases(Texture* texture, glm::vec2 size = { 16, 16 });

	void SetSubTextureSize(uint32_t w, uint32_t h);

	void Push(uint32_t count = 1, uint8_t mulW = 1, uint8_t mulH = 1);
	void Skip(int32_t count = 1);
	void SkipLine(int32_t count = 1);
	void ResetPosition();
	void ChangeStartPosition(float x = 0, float y = 0);
	void ChangeEndPosition(float x = -1, float y = -1);

	size_t Size() const;

	inline Texture* GetTexture() const { return m_Texture; };
	inline uint32_t GetTextureId() const { return m_Texture->GetId(); };
	inline glm::vec2 GetImageSize() const { return m_ImageSize; };
	inline glm::vec2 GetSize() const { return m_Size; };
};

class TextureAtlasesGui
{
	TextureAtlases m_TextureAtlases;
	int m_SelectedItem;
public:
	inline TextureAtlasesGui(const TextureAtlases& ta)
		: m_TextureAtlases(ta) {};

	void RenderImGui();

	inline int GetSelectedItem() { return m_SelectedItem; };
};