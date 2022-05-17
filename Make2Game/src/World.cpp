#include "World.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "imgui.h"

World::World(const TextureAtlases& textureAtlases)
	:m_TextureAtlases(textureAtlases)
{
}

static enum class WorldEnum
{
	None, TextureAtlases, Blocks
};
void World::ReadFile(std::string filepath)
{
    std::ifstream file(filepath);
	if (file.is_open())
    {
		WorldEnum worldEnum = WorldEnum::None;

		std::string line;
        while (getline(file, line))
        {
			if (line == "")
				continue;

			//std::cout << line << std::endl;
			std::stringstream ss(line);

			if (line.find("[") != std::string::npos)
			{
				if (line.find("TextureAtlases") != std::string::npos)
				{
					worldEnum = WorldEnum::TextureAtlases;
				}
				else if (line.find("Blocks") != std::string::npos)
				{
					worldEnum = WorldEnum::Blocks;
				}
			}
			else
			{
				std::string dummy;
				if(worldEnum == WorldEnum::TextureAtlases)
				{
					std::string name;
					int position;
					ss >> name;
					ss >> dummy;
					ss >> position;
					blockNames[name] = position;
				}
				else if (worldEnum == WorldEnum::Blocks)
				{
					static size_t objektID;
					static std::string layerName = "";
					if (line.find("<") != std::string::npos)
					{
						std::string objektName;
						ss >> objektName;
						objektName.erase(remove(objektName.begin(), objektName.end(), '<'), objektName.end());
						objektName.erase(remove(objektName.begin(), objektName.end(), '>'), objektName.end());
						objektID = blockNames.at(objektName);
					}
					else if (line.find("|") != std::string::npos)
					{
						ss >> layerName;
						layerName.erase(remove(layerName.begin(), layerName.end(), '|'), layerName.end());
					}
					else {
						if (layerName == "")
						{
							std::cout << "Layer Verilmeden islem yapılamaz." << std::endl;
						}
						int x, y;
						ss >> x;
						ss >> y;
						layers[layerName].blocks[objektID].push_back({ x, y });
						//blocks[objektID].push_back({ x, y });
					}
				}
			}
        }
    }
	//blocks.push_back(std::make_pair(0, std::vector<glm::vec2>{ {0, 16} }));
	//blocks.push_back(std::make_pair(1, std::vector<glm::vec2>{ {16, 16}, { 32, 16 }}));
	//blocks.push_back(std::make_pair(2, std::vector<glm::vec2>{ {48, 16} }));
}

void World::RenderWorld(BatchRenderer& batchRenderer)
{
	//batchRenderer.BeginBatch();
	for(auto& layer : layers)
	{
		layer.second.RenderLayer(batchRenderer, m_TextureAtlases);
	}
	//batchRenderer.EndBatch();

	//batchRenderer.Draw();
}
