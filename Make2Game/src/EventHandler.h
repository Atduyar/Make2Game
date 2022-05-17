#pragma once
#include "imgui_impl_sdl.h"

class EventHandler
{
public:
	virtual void ProcessEvent(SDL_Event* sdlEvent) {};
};
