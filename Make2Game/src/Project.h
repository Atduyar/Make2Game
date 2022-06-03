#pragma once
#include <filesystem>
#include <lua.hpp>
#include <string>
#include <tuple>

#include "App.h"
#include "FrameBuffer.h"
#include "TextureBuffer.h"
#include "imgui_impl_sdl.h"
#include "Renderer.h"
#include "TextureAtlases.h"
#include "ImGuiColorTextEdit/TextEditor.h"

class Project
{
private:
	lua_State* L;
	std::string m_ProjectFolder;
	FrameBuffer m_FrameBuffer;
	TextureBuffer m_TextureBuffer;
	std::vector<std::tuple<std::filesystem::path, TextEditor*>> m_luaFiles;
	std::vector<std::filesystem::path> m_luaConfig;
	std::vector<std::filesystem::path> m_luaLibs;
	bool m_StopAfter= false;
public:
	bool isRuning = false;

public:
	Project(std::string projectFolder);
	void Toggle();
	void GameLoop(BatchRenderer& batchRenderer, TextureAtlases& textureAtlases);

	void Start();
	void Input(SDL_Event &event);
	void Update();
	void Render();
	void Stop();
	
	void Unbind();

	void CodeEditor();

	void ImGuiBegin();
	void ImGuiEnd();
};
