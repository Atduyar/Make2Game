#pragma once
#include <SDL_events.h>


class Mouse
{
public:
	struct MousePos
	{
		int32_t x;
		int32_t y;
	};

	MousePos MouseDragStartPos;
public:

	Mouse();
	void MouseUp(SDL_MouseButtonEvent);
	void MouseMove(SDL_MouseButtonEvent, void(*)(MousePos));
	void MouseDown(SDL_MouseButtonEvent);
	void MouseWheelScroll(SDL_MouseWheelEvent mwe, void(*onWeeled)(int));
private:
};
