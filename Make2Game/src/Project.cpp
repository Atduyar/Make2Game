#include "Project.h"

#include <fstream>
#include <iostream>

#include "TextureAtlases.h"
static void DumpStack(lua_State* L) {
	int top = lua_gettop(L);

	std::cout << "<Stack " << top << ">" << std::endl;

	for (int i = top; i >= 1; i--) {
		std::cout << "\t[Stack " << i << "] " << "\t" << luaL_typename(L, i);
		switch (lua_type(L, i)) {
		case LUA_TNUMBER:
			std::cout << lua_tonumber(L, i);
			break;
		case LUA_TSTRING:
			std::cout << lua_tostring(L, i);
			break;
		case LUA_TBOOLEAN:
			std::cout << (lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNIL:
			std::cout << "nil";
			break;
		default:
			std::cout << luaL_typename(L, i) << ": " << lua_topointer(L, i);
			break;
		}

		std::cout << std::endl;
	}
}
#define DumpStack DumpStack(L);
static bool CheckLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << "[LUA] Error: " << errormsg << std::endl;
		return false;
	}
	return true;
}

static const char* g_ProjectFolder;
extern App* app;

static int textureAtlasesNewTexture(lua_State* L) {
	std::string path = lua_tostring(L, -1);
	path = g_ProjectFolder + path;
	//std::cout << "[c++] " << path << std::endl;

	Texture** texture = (Texture**)lua_newuserdata(L, sizeof(Texture*));
	*texture = new Texture(path);
	(*texture)->Bind();
	
	lua_pushinteger(L, (*texture)->GetId());
	lua_pushinteger(L, (*texture)->GetWidth());
	lua_pushinteger(L, (*texture)->GetHeigth());
	//std::cout << "[c++ as] " << (*texture)->GetWidth() << (*texture)->GetHeigth() << std::endl;
	return 4;
}

static int textureAtlasesCreateSubTexture(lua_State* L) {
	float x = lua_isinteger(L, -4) ? lua_tointeger(L, -4) : lua_tonumber(L, -4);
	float y = lua_isinteger(L, -3) ? lua_tointeger(L, -3) : lua_tonumber(L, -3);
	float w = lua_isinteger(L, -2) ? lua_tointeger(L, -2) : lua_tonumber(L, -2);
	float h = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : lua_tonumber(L, -1);

	lua_getfield(L, -5, "info");
	lua_getfield(L, -1, "size");

	lua_getfield(L, -1, "w");
	float imageSizeW = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : lua_tonumber(L, -1);

	lua_getfield(L, -2, "h");
	float imageSizeH = lua_isinteger(L, -2) ? lua_tointeger(L, -2) : lua_tonumber(L, -2);
	
	lua_pop(L, 4);

	//std::cout << "[c++] " << x << "\t" << y << "\t" << w << "\t" << h << "\t" << imageSizeW << "\t" << imageSizeH << std::endl;

	SubTexturee** subTexture = (SubTexturee**)lua_newuserdata(L, sizeof(SubTexturee*));
	*subTexture = new SubTexturee;

	(*subTexture)->imageCoord[0] = { (x + w)	/ imageSizeW,				(y)		/ imageSizeH };
	(*subTexture)->imageCoord[1] = { (x)		/ imageSizeW,				(y)		/ imageSizeH };
	(*subTexture)->imageCoord[2] = { (x + w)	/ imageSizeW,				(y + h)	/ imageSizeH };
	(*subTexture)->imageCoord[3] = { (x)		/ imageSizeW,				(y + h)	/ imageSizeH };
	return 1;
}

static int textureAtlasesBind(lua_State* L) {
	lua_getfield(L, -1, "info");
	lua_getfield(L, -1, "texturePtr");

	Texture** texture = (Texture**)lua_touserdata(L, -1);
	
	(*texture)->Bind();

	lua_pop(L, 2);

	return 0;
}

static int appCls(lua_State* L) {
	lua_getfield(L, -1, "clearColor");
	lua_rawgeti(L, -1, 1);
	lua_rawgeti(L, -2, 2);
	lua_rawgeti(L, -3, 3);
	lua_rawgeti(L, -4, 4);

	float r = lua_isinteger(L, -4) ? lua_tointeger(L, -4) : lua_tonumber(L, -4);
	float g = lua_isinteger(L, -3) ? lua_tointeger(L, -3) : lua_tonumber(L, -3);
	float b = lua_isinteger(L, -2) ? lua_tointeger(L, -2) : lua_tonumber(L, -2);
	float a = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : lua_tonumber(L, -1);
	r = r / 255.0f;
	g = g / 255.0f;
	b = b / 255.0f;
	a = a / 255.0f;
	//std::cout << "[c++] " << r << "\t" << g << "\t" << b << "\t" << a << std::endl;

	glClearColor(r, g, b, a);
	app->renderer->Clear();

	lua_pop(L, 5);

	return 0;
}
static int textureAtlasesDraw(lua_State* L) {
	//SubTexturee** subTexture = (SubTexturee**)lua_touserdata(L, -3);
	std::string blockName = "";
	SubTexturee** subTexture = nullptr;

	if (lua_isstring(L, -3)) {
		blockName = lua_tostring(L, -3);
	}

	lua_rawgeti(L, -2, 1);// pos x
	int x = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, -2, 2);// pos y
	int y = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, -1, 1);// pos x
	int w = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, -1, 2);// pos y
	int h = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	if(blockName == "")
	{
		subTexture = (SubTexturee**)lua_touserdata(L, -3);
	}
	else
	{
		lua_getfield(L, -4, "subtex");
		lua_getfield(L, -1, blockName.c_str());
		subTexture = (SubTexturee**)lua_touserdata(L, -1);
		lua_pop(L, 2);
	}

	lua_getfield(L, -4, "info");
	lua_getfield(L, -1, "texturePtr");
	Texture** texture = (Texture**)lua_touserdata(L, -1);
	//(*texture)->Bind();

	lua_pop(L, 2);

	lua_getglobal(L, "app");
	lua_getfield(L, -1,  "lastTextureAtlases");

	if(lua_isnil(L, -1))
	{
		Texture** lastTexture = (Texture**)lua_newuserdata(L, sizeof(Texture*));
		*lastTexture = *texture;
		
		lua_setfield(L, -3, "lastTextureAtlases");
	}
	else
	{
		Texture** lastTexture = (Texture**)lua_touserdata(L, -1);
		//if texture same as last testure
		if(*lastTexture != *texture)
		{
			(*lastTexture)->Bind();
			//bind
			//draw
			app->batchRenderer->EndBatch();
			app->batchRenderer->Draw();
			app->batchRenderer->BeginBatch();

			*lastTexture = *texture;
		}
	}

	lua_pop(L, 2);// app lastTextureAtlases
	//std::cout << "[c++] " << x << "\t" << y << "\t" << (*subTexture)->imageCoord[0].x << std::endl;
	//std::cout << "[c++] " << w << "\t" << h << "\t" << std::endl;

	app->batchRenderer->DrawQuad(glm::vec2{ x, y }, glm::vec2{w,h}, **subTexture);

	return 0;
}


Project::Project(std::string projectFolder)
	: m_ProjectFolder(projectFolder), m_TextureBuffer(1,1)
{
	m_FrameBuffer.AttachTexture(m_TextureBuffer);
	m_TextureBuffer.Unbind();

	//m_luaLibs
	for (auto& entry : std::filesystem::directory_iterator(m_ProjectFolder + "/lib"))
	{
		if (entry.path().extension() == ".lua")
		{
			m_luaLibs.push_back(entry.path());
		}
	}

	//m_luaConfig
	{
		for (auto& entry : std::filesystem::directory_iterator(m_ProjectFolder + "/config/TileMaps"))
		{
			if (entry.path().extension() == ".lua")
			{
				m_luaConfig.push_back(entry.path());
			}
		}
		for (auto& entry : std::filesystem::directory_iterator(m_ProjectFolder + "/config/Maps"))
		{
			if (entry.path().extension() == ".lua")
			{
				m_luaConfig.push_back(entry.path());
			}
		}
		for (auto& entry : std::filesystem::directory_iterator(m_ProjectFolder + "/config"))
		{
			if (entry.path().extension() == ".lua")
			{
				m_luaConfig.push_back(entry.path());
			}
		}
	}

	//lua
	for (auto& entry : std::filesystem::directory_iterator(m_ProjectFolder))
	{
		if (entry.path().extension() == ".lua")
		{
			TextEditor* editor = new TextEditor;
			auto lang = TextEditor::LanguageDefinition::Lua();
			editor->SetLanguageDefinition(lang);
			{
				std::ifstream ifs(entry.path().string());
				if (ifs.good())
				{
					std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
					editor->SetText(str);
				}
			}


			m_luaFiles.push_back(std::make_tuple(entry.path(), editor));
			//std::cout << entry.path() << std::endl;
		}
	}
}
	
void Project::Start()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	std::cout << "[Lua] Start" << std::endl;

	// save all editor
	for (auto& luafile : m_luaFiles)
	{
		auto& [luafilePath, editor] = luafile;

		auto textToSave = editor->GetText();
		{
			std::fstream t(luafilePath.string(), std::ios::out | std::ios::trunc);
			if (t.good())
			{
				t << textToSave;
			}
		}
	}

	// Start all lua libs
	for (auto& luaLib : m_luaLibs)
	{
		if (!CheckLua(L, luaL_dofile(L, luaLib.string().c_str())))
		{
			isRuning = false;
			m_StopAfter = false;
			return;
		}
	}

	// Start all c++ libs
	{
		//TextureAtlases.lib
		{
			g_ProjectFolder = m_ProjectFolder.c_str();
			lua_getglobal(L, "textureAtlases");						// 1

			//luaL_register(L, NULL, module_graphics);
			lua_pushcfunction(L, textureAtlasesNewTexture);			// 2
			lua_setfield(L, -2, "newTexture");

			lua_pushcfunction(L, textureAtlasesCreateSubTexture);	// 2
			lua_setfield(L, -2, "createSubTexture");

			lua_pushcfunction(L, textureAtlasesBind);	// 2
			lua_setfield(L, -2, "use");
			lua_pushcfunction(L, textureAtlasesBind);	// 2
			lua_setfield(L, -2, "bind");

			lua_pushcfunction(L, textureAtlasesDraw);	// 2
			lua_setfield(L, -2, "draw");

			lua_pop(L, 1);											// pop textureAtlases
		}

		//Map.lua
		{
			lua_getglobal(L, "map");								// 1
			//lua_pushcfunction(L, mapCls);							// 2
			//lua_setfield(L, -2, "cls");
			//lua_pushcfunction(L, mapDrawBlock);							// 2
			//lua_setglobal(L, "drawBlock");

			lua_pop(L, 1);											// pop map
		}

		//App.lua
		{
			lua_getglobal(L, "app");								// 1
			lua_pushcfunction(L, appCls);							// 2
			lua_setfield(L, -2, "cls");
			//lua_pushcfunction(L, mapDrawBlock);							// 2
			//lua_setglobal(L, "drawBlock");

			lua_pop(L, 1);											// pop map
		}
	}

	// Start Configs
	for (auto& luaConfig : m_luaConfig)
	{
		if (!CheckLua(L, luaL_dofile(L, luaConfig.string().c_str())))
		{
			isRuning = false;
			m_StopAfter = false;
			return;
		}
	}
	
	// Start all lua files
	for (auto& luaFile : m_luaFiles)
	{
		auto& [luafilePath, editor] = luaFile;
		if (!CheckLua(L, luaL_dofile(L, luafilePath.string().c_str())))
		{
			isRuning = false;
			m_StopAfter = false;
			return;
		}
	}

	isRuning = true;
	
	ImGuiBegin();
	ImVec2 size = ImGui::GetContentRegionAvail();
	m_TextureBuffer.ReSize(size.x, size.y);
	ImGuiEnd();


}

void Project::Toggle()
{
	if (isRuning)
	{
		Stop();
		m_StopAfter = false;
	}
	else
		Start();
}

void Project::GameLoop(BatchRenderer& batchRenderer, TextureAtlases& textureAtlases)
{
	if (isRuning)
	{
		m_StopAfter = true;
		m_FrameBuffer.Bind();
		batchRenderer.BeginBatch();

		ImGuiBegin();

		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x != m_TextureBuffer.GetWidth() || size.y != m_TextureBuffer.GetHeight())
		{
			m_TextureBuffer.ReSize(size.x, size.y);
		}


		Update();
		Render();

		lua_getglobal(L, "app");
		lua_getfield(L, -1, "lastTextureAtlases");
		if(!lua_isnil(L, -1))
		{
			Texture** lastTexture = (Texture**)lua_touserdata(L, -1);
			(*lastTexture)->Bind();
		}
		lua_pop(L, 2);

		batchRenderer.EndBatch();
		batchRenderer.Draw();
		
		ImGui::Image((ImTextureID)m_TextureBuffer.GetId(), size, ImVec2(0, 1), ImVec2(1, 0));
		ImGuiEnd();

		m_FrameBuffer.Unbind();
	}
	else if(m_StopAfter)
	{
		Stop();
	}
}

void Project::Input(SDL_Event &event)
{
	ImGuiBegin();
	if(ImGui::IsWindowFocused() == 1)
	{
		lua_getglobal(L, "Input");
		if (lua_isfunction(L, -1))
		{
			if (CheckLua(L, lua_pcall(L, 0, 0, 0)));
		}
		else
		{
			std::cout << "Lua Input function not found." << std::endl;
		}
	}
	ImGuiEnd();
}

void Project::Update()
{
	lua_getglobal(L, "Update");
	if (lua_isfunction(L, -1))
	{
		if (CheckLua(L, lua_pcall(L, 0, 0, 0)));
	}
	else
	{
		std::cout << "Lua Updata function not found." << std::endl;
	}
}

void Project::Render()
{
	lua_getglobal(L, "Render");
	if (lua_isfunction(L, -1))
	{
		if (CheckLua(L, lua_pcall(L, 0, 0, 0)));
	}
	else
	{
		std::cout << "Lua Render function not found." << std::endl;
	}
}

void Project::Stop()
{
	std::cout << "[Lua] Stop" << std::endl;
	lua_close(L);

	m_StopAfter = false;
	isRuning = false;
}

void Project::Unbind()
{
	m_FrameBuffer.Unbind();
	m_TextureBuffer.Unbind();
}

void Project::CodeEditor()
{
	for (auto& luafile: m_luaFiles)
	{
		auto& [luafilePath, editor] = luafile;

		auto cpos = editor->GetCursorPosition();
		ImGui::Begin(luafilePath.filename().string().c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
		ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "Ctrl-S"))
				{
					auto textToSave = editor->GetText();
					/// save text....
					{
						std::fstream t(luafilePath.string(), std::ios::out | std::ios::trunc);
						if (t.good())
						{
							t << textToSave;
						}
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				bool ro = editor->IsReadOnly();

				if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor->CanUndo()))
					editor->Undo();
				if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor->CanRedo()))
					editor->Redo();

				ImGui::Separator();

				if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor->HasSelection()))
					editor->Copy();
				if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor->HasSelection()))
					editor->Cut();
				if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor->HasSelection()))
					editor->Delete();
				if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
					editor->Paste();

				ImGui::Separator();

				if (ImGui::MenuItem("Select all", nullptr, nullptr))
					editor->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor->GetTotalLines(), 0));

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor->GetTotalLines(),
			editor->IsOverwrite() ? "Ovr" : "Ins",
			editor->CanUndo() ? "*" : " ",
			editor->GetLanguageDefinition().mName.c_str(), luafilePath.filename().string().c_str());

		editor->Render("TextEditor");
		ImGui::End();

	}
}

void Project::ImGuiBegin()
{

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Project", &isRuning, window_flags);
}

void Project::ImGuiEnd()
{
	ImGui::End();
	ImGui::PopStyleVar(); // padding
}
