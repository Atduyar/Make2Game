#include "EventSystem.h"

#include <SDL.h>
#include "imgui_impl_sdl.h"

EventSystem::EventSystem()
{
}

void EventSystem::AddEvent(EventHandler eventHandler)
{
	eventHandlers.push_back(eventHandler);
}

void EventSystem::ProcessEvent()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		for (auto eh : eventHandlers)
		{
			eh.ProcessEvent(&event);
			//switch (event.type) {
			//case SDL_QUIT:
			//	done = true;
			//case SDL_WINDOWEVENT:
			//	if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
			//		done = true;
			//	if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
			//		wx = event.window.data1;
			//		wy = event.window.data2;
			//		//SDL_GetWindowSize(window, &wx, &wy);
			//		proj = glm::ortho(0.0f, (float)wx, 0.0f, (float)wy, -1.0f, 1.0f);
			//		glViewport(0, 0, wx, wy);
			//		Log::GetInstance().AddLog(std::to_string(wx) + " - " + std::to_string(wy));
			//		//glViewport(0, 0, event.window.data1, event.window.data2);
			//	}
			//case SDL_KEYDOWN:
			//	switch (event.key.keysym.sym) {
			//	case SDLK_ESCAPE:
			//		done = true;
			//		break;
			//	case SDLK_RCTRL:
			//		rctrl = true;
			//		break;
			//	case SDLK_LCTRL:
			//		lctrl = true;
			//		break;
			//	}
			//	break;
			//case SDL_KEYUP:
			//	switch (event.key.keysym.sym) {
			//	case SDLK_RCTRL:
			//		rctrl = false;
			//		break;
			//	case SDLK_LCTRL:
			//		lctrl = false;
			//		break;
			//	}
			//	break;
			//case SDL_MOUSEBUTTONDOWN:
			//	mouse.MouseDown(event.button);
			//	if (event.button.button == SDL_BUTTON_LEFT) {
			//		auto item = textureAtlasesGui.GetSelectedItem();
			//		if (item == -1)
			//		{
			//			word.layers["arka"].DeleteBlockByPositio({ mousex, mousey });
			//		}
			//		else
			//		{
			//			word.layers["arka"].blocks[item].push_back({ mousex, mousey });
			//		}
			//	}
			//	viewTranTemp = viewTran;
			//	break;
			//case SDL_MOUSEMOTION:
			//	mousePos.x = event.button.x;
			//	mousePos.y = wy - event.button.y;
			//	mouse.MouseMove(event.button, [](Mouse::MousePos mp)
			//		{
			//			viewTran.x = viewTranTemp.x + mp.x;
			//			viewTran.y = viewTranTemp.y + mp.y;
			//		});

			//	mousex = (mousePos.x - viewTran.x) * 1 / zoom;
			//	mousey = (mousePos.y - viewTran.y) * 1 / zoom;
			//	if (mousex < 0) mousex -= 16;
			//	if (mousey < 0) mousey -= 16;
			//	mousex -= mousex % 16;
			//	mousey -= mousey % 16;
			//	break;
			//case SDL_MOUSEBUTTONUP:
			//	mouse.MouseUp(event.button);
			//	break;
			//case SDL_MOUSEWHEEL:
			//	if (lctrl)
			//	{
			//		mouse.MouseWheelScroll(event.wheel, [](int weel)
			//			{
			//				float incr = (float)zoom / 3.0f;
			//				if (weel < 0)
			//					incr *= -1;
			//				if (weel < 0 && zoom + incr <= 0)
			//					return;

			//				zoom += incr;
			//			});
			//	}
			//	break;
			//}
		}
	}
}
