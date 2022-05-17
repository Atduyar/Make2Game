#pragma once
#include <string>
#include <vector>

#include "Layer.h"
#include "TextureAtlases.h"
#include "glm/glm.hpp"


class World
{
	const TextureAtlases &m_TextureAtlases;
public:
	//std::unordered_map<size_t, std::vector<glm::vec2>> blocks;
	std::unordered_map<std::string, int> blockNames;
	std::unordered_map<std::string, Layer> layers;

	World(const TextureAtlases& textureAtlases);
	void ReadFile(std::string filepath);
	void RenderWorld(BatchRenderer& batchRenderer);
};