#include "Mouse.h"

#include <iostream>

#include "imgui.h"

Mouse::Mouse()
{
}

void Mouse::MouseUp(SDL_MouseButtonEvent me)
{
	if (me.button == SDL_BUTTON_MIDDLE) {
		MouseDragStartPos.x = me.x;
		MouseDragStartPos.y = me.y;
	}
}

void Mouse::MouseMove(SDL_MouseButtonEvent me, void(*onMoved)(MousePos))
{
	if (me.button == SDL_BUTTON_MIDDLE) {
		MousePos mouseMoved = MouseDragStartPos;
		mouseMoved.x  = me.x - mouseMoved.x;
		mouseMoved.y -= me.y;
		onMoved(mouseMoved);
		return ;
	}
}

void Mouse::MouseDown(SDL_MouseButtonEvent me)
{
	if (me.button == SDL_BUTTON_MIDDLE) {
		MouseDragStartPos.x = me.x;
		MouseDragStartPos.y = me.y;
	}
}

void Mouse::MouseWheelScroll(SDL_MouseWheelEvent mwe, void(*onWeeled)(int))
{
	onWeeled(mwe.y);
}
