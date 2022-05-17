#include "Layer.h"

#include "imgui.h"
#include "World.h"
#include "World.h"

void Layer::RenderImGui(const World& word)
{
	ImGui::Begin("Layersaa");
	static int selected = 0;
	int i = 0;
	for (auto const& [key, val] : word.layers)
	{
		if (ImGui::Selectable(key.c_str(), selected == i))
			selected = i;
		++i;
	}
	ImGui::End();
}

void Layer::RenderLayer(BatchRenderer& batchRenderer, const TextureAtlases& textureAtlases)
{
	for (auto const& block : blocks)
	{
		for (size_t i = 0; i < block.second.size(); i++)
		{
			auto itemSize = textureAtlases.subTextures[block.first].size;
			//auto itemSize = textureAtlases.GetSize();
			batchRenderer.DrawQuad(block.second[i], itemSize, textureAtlases.subTextures[block.first]);
		}
	}
}

void Layer::DeleteBlockByPositio(glm::ivec2 posi)
{
	for (auto& block : blocks)
	{
		auto& vec = block.second;
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (vec[i].x == posi.x &&
				vec[i].y == posi.y)
			{
				vec.erase(block.second.begin()+i);
				return;
			}
		}
	}
}
