#pragma once
#include "Renderer.h"
#include "TextureAtlases.h"

struct App
{
	BatchRenderer* batchRenderer;
	Renderer* renderer;
	TextureAtlases* textureAtlases;
};
