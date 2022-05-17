#pragma once
#include <unordered_map>

#include "TextureAtlases.h"
#include "glm/vec2.hpp"

class World;

class Layer
{
public:
	std::unordered_map<size_t, std::vector<glm::vec2>> blocks;
	bool background;

public:
	static void RenderImGui(const World& word);
	void RenderLayer(BatchRenderer& batchRenderer, const TextureAtlases& textureAtlases);
	void DeleteBlockByPositio(glm::ivec2);
};
