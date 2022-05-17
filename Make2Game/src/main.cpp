// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
//#define GLEW_STATIC
#include <GL/glew.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif
 
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Log.h"
#include "Mouse.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureAtlases.h"
#include "VertexBufferLayout.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <fstream>
#include <streambuf>
#include <dinput.h>
#include <tchar.h>
#include "ImGuiColorTextEdit/TextEditor.h"

#include <filesystem>
#include <lua.hpp>

#include "EventSystem.h"
#include "LuaScript.h"
#include "World.h"

// Main code
int main(int, char**)
{
#ifdef _WIN32
    SetProcessDPIAware();// Fix DPI settings
#endif
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 330";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow(
        "Dear ImGui SDL2+OpenGL3 example",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    GLErrorHandler();
    {
        /* Print Status */
        SDL_version linked;
        SDL_GetVersion(&linked);
        fprintf(stdout, "Status: Using SDL    %u.%u.%u\n", linked.major, linked.minor, linked.patch);
        fprintf(stdout, "Status: Using GLEW   %s\n", glewGetString(GLEW_VERSION));
        fprintf(stdout, "Status: Using OpenGl %s\n", glGetString(GL_VERSION));
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags imgui_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	imgui_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	imgui_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ///////////////////////////////////////////////////
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ///////////////////////////////////////////////////
    {



		int wx, wy;
		SDL_GetWindowSize(window, &wx, &wy);
		glm::mat4 proj = glm::ortho(0.0f, (float)wx, 0.0f, (float)wy, -1.0f, 1.0f);

		//glm::mat4 view	= glm::translate(glm::mat4(1.0f), glm::vec3(100, 100, 0));
		//glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)), glm::vec3(5, 5, 5));

		//glm::mat4 MVP = proj * view * model;

		Log::GetInstance().AddLog(std::to_string(wx) + " - " + std::to_string(wy));

		Shader shader("./res/shaders/Texture.shader");
		shader.Bind();
		//shader.SetUniform4f("u_Color", 0, 1, 1, 1);

		Texture texture("./res/img/tilemapV2.png");
		texture.Bind(); 
    	shader.SetUniform1i("u_Texture", 0);
		//shader.SetUniformMat4f("u_MVP", MVP);
		TextureAtlases textureAtlases(&texture);
		{
			textureAtlases.ChangeEndPosition(4);
			textureAtlases.Push(16);

			textureAtlases.ChangeStartPosition(4);
			textureAtlases.ChangeEndPosition(6);
			textureAtlases.ResetPosition();
			textureAtlases.Push(8);

			textureAtlases.ChangeStartPosition(6);
			textureAtlases.ChangeEndPosition(7);
			textureAtlases.ResetPosition();
			textureAtlases.Push(4);

			textureAtlases.ChangeStartPosition(0, 4);
			textureAtlases.ChangeEndPosition(4, 8);
			textureAtlases.ResetPosition();
			textureAtlases.Push(16);

			textureAtlases.ChangeStartPosition(6, 0);
			textureAtlases.ChangeEndPosition(10);
			textureAtlases.ResetPosition();
			textureAtlases.Push(6);
		}

		World word(textureAtlases);
		std::cout << SDL_GetBasePath() << std::endl;

		std::string worldPath = SDL_GetBasePath();
		worldPath += "world1-1.txt";

		word.ReadFile(worldPath);

		TextureAtlasesGui textureAtlasesGui(textureAtlases);

		//SubTexture& subTexture = SubTexture::Create({ 16.0f / 160 * 0, 16.0f / 160 * 9 }, { 16.0f / 160, 16.0f / 160 });
		//SubTexture& subTexture1 = SubTexture::Create({ 16.0f / 160 * 1, 16.0f / 160 * 9 }, { 16.0f / 160, 16.0f / 160 });

		shader.Unbind();
		//va.Unbind();
		//ib.Unbind();
		//vb.Unbind();

		BatchRenderer batchRenderer;
		
		Renderer renderer;

		Mouse mouse;



		///////////////////////////////////////////////////////////////////////
		// TEXT EDITOR SAMPLE

		std::string fileToEdit = SDL_GetBasePath();
		fileToEdit += "main.lua";

		TextEditor editor;
		auto lang = TextEditor::LanguageDefinition::Lua();
		editor.SetLanguageDefinition(lang);

    	LuaScript luaScript(fileToEdit);
		{
			std::ifstream t(fileToEdit);
			if (t.good())
			{
				std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
				editor.SetText(str);
			}
		}
		EventSystem eventSystem;
    	// Main loop
	    bool done = false;
	    while (!done)
	    {

	        // Start the Dear ImGui frame
	        ImGui_ImplOpenGL3_NewFrame();
	        ImGui_ImplSDL2_NewFrame();
	        ImGui::NewFrame();
	        // Poll and handle events (inputs, window resize, etc.)
	        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.



			static glm::vec3 viewTran = glm::vec3(.0f, .0f, .0f);
			static glm::vec3 viewTranTemp = glm::vec3(.0f, .0f, .0f);

			static float zoom = 3.0f;
			
	    	static glm::vec3 mousePos = glm::vec3(.0f, .0f, .0f);

			static bool lctrl = false, rctrl = false;

			ImGui::Begin("Event Heandler");

			static int mousex = 0;
			static int mousey = 0;
			bool dontImGuiEvent = true;
			//eventSystem.ProcessEvent();
			SDL_Event event;
	        while (SDL_PollEvent(&event))
	        {
				ImGui_ImplSDL2_ProcessEvent(&event);
				switch (event.type) {
				case SDL_QUIT:
						done = true;
				case SDL_WINDOWEVENT:
					if(event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
						done = true;
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						wx = event.window.data1;
						wy = event.window.data2;
						//SDL_GetWindowSize(window, &wx, &wy);
						proj = glm::ortho(0.0f, (float)wx, 0.0f, (float)wy, -1.0f, 1.0f);
						glViewport(0, 0, wx, wy);
						Log::GetInstance().AddLog(std::to_string(wx) + " - " + std::to_string(wy));
						//glViewport(0, 0, event.window.data1, event.window.data2);
					}
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						done = true;
						break;
					case SDLK_RCTRL:
						rctrl = true;
						break;
					case SDLK_LCTRL:
						lctrl = true;
						break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					case SDLK_RCTRL:
						rctrl = false;
						break;
					case SDLK_LCTRL:
						lctrl = false;
						break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					mouse.MouseDown(event.button);
					if (event.button.button == SDL_BUTTON_LEFT) {
						auto item = textureAtlasesGui.GetSelectedItem();
						if (item == -1)
						{
							word.layers["arka"].DeleteBlockByPositio({mousex, mousey});
						}
						else
						{
							word.layers["arka"].blocks[item].push_back({mousex, mousey});
						}
					}
					viewTranTemp = viewTran;
					break;
				case SDL_MOUSEMOTION:
					mousePos.x = event.button.x;
					mousePos.y = wy - event.button.y;
					mouse.MouseMove(event.button, [](Mouse::MousePos mp)
					{
						viewTran.x = viewTranTemp.x + mp.x;
						viewTran.y = viewTranTemp.y + mp.y;
					});

					mousex = (mousePos.x - viewTran.x) * 1 / zoom;
					mousey = (mousePos.y - viewTran.y) * 1 / zoom;
					if (mousex < 0) mousex -= 16;
					if (mousey < 0) mousey -= 16;
					mousex -= mousex % 16;
					mousey -= mousey % 16;
					break;
				case SDL_MOUSEBUTTONUP:
					mouse.MouseUp(event.button);
					break;
				case SDL_MOUSEWHEEL:
					if(lctrl)
					{
						mouse.MouseWheelScroll(event.wheel, [](int weel)
							{
								float incr = (float)zoom / 3.0f;
								if (weel < 0)
									incr *= -1;
								if (weel < 0 && zoom + incr <= 0)
									return;

								zoom += incr;
							});
					}
					break;
				}
	        }
			//if(dontImGuiEvent) ImGui_ImplSDL2_ProcessEvent(&event);

			ImGui::Text("%d - %d", lctrl, rctrl);
			ImGui::Text("%f - %f", mousePos.x, mousePos.y);
			ImGui::Text("%f", zoom);
	    	ImGui::End();

			glm::mat4 view = glm::mat4(1.0f);
			
			view = glm::translate(view, viewTran);
			view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));

			glm::mat4 MVP = proj * view;
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", MVP);

			//---------------PENCEREDE OPENGL-------------------
	        //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	        //glEnable(GL_DEPTH_TEST);
	        //glViewport(0, 0, 200, 200);
	        //glClearColor(0.0f, 1.0f, 0.3f, 1.0f);
	        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

	        //glBegin(GL_TRIANGLES);
	        //    glColor3f(1.0f, 0.2f, 0.3f);
	        //    glVertex3f(0.0f, 0.35f, 0.0f);
	        //    glVertex3f(0.5f, -0.35f, 0.0f);
	        //    glVertex3f(-0.5f, -0.35f, 0.0f);
	        //glEnd();
			//---------------PENCEREDE OPENGL-------------------



			//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			//glEnable(GL_DEPTH_TEST);
			////glViewport(0, 0, 200, 200);
			////glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
			//
			//glClearColor(0.0f, 1.0f, 0.3f, 1.0f);
			//renderer.Clear();
			//
			//shader.Bind();
			//renderer.Draw(va, ib, shader);



			//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

			glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
			renderer.Clear();

			shader.Bind();
			//renderer.Draw(va,ib,shader);

	    	batchRenderer.BeginBatch();

	    	word.RenderWorld(batchRenderer);

			auto item = textureAtlasesGui.GetSelectedItem();
			if (item == -1)
				batchRenderer.DrawQuad({ mousex, mousey }, { 16,16 }, textureAtlases.subTextures[word.blockNames["redBox"] % (textureAtlases.Size())]);
			else
				batchRenderer.DrawQuad({ mousex, mousey }, { 16,16 }, textureAtlases.subTextures[item % (textureAtlases.Size())]);


			batchRenderer.EndBatch();

			batchRenderer.Draw();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			luaScript.Update();


	    	//int mousex = (mousePos.x - viewTran.x) * 1 / zoom;
			//int mousey = (mousePos.y - viewTran.y) * 1 / zoom;
			//if (mousex < 0) mousex -= 16;
			//if (mousey < 0) mousey -= 16;
			//mousex -= mousex % 16;
			//mousey -= mousey % 16;

			ImGui::Text("Mouse -> %d, %d", mousex, mousey);
			//static int batchx = 200;
			//static int batchy = 204;
			//for (int y = 0; y < batchy; y++)
			//{
			//	for (int x = 0; x < batchx; x++)
			//	{
			//		batchRenderer.DrawQuad({ x*50, y*50 }, { 50,50 }, textureAtlases.subTextures[((y * batchy) + x) % (textureAtlases.Size())]);
			//	}
			//}
			//batchRenderer.DrawQuad({ 0, 0 }, { 16,16 }, textureAtlases.subTextures[0 % (textureAtlases.Size())]);

			//static char buf1[64] = "200"; ImGui::InputText("X", buf1, 64, ImGuiInputTextFlags_CharsDecimal);
			//static char buf2[64] = "204"; ImGui::InputText("Y", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
			//if(*buf1 != 0) batchx = std::stoi(buf1);
			//if(*buf2 != 0) batchy = std::stoi(buf2);


	        static bool ViewWindow[] = { true, true, true };


	        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	        // because it would be confusing to have two docking targets within each others.
	        const ImGuiViewport* viewport = ImGui::GetMainViewport();
	        ImGui::SetNextWindowPos(viewport->WorkPos);
	        ImGui::SetNextWindowSize(viewport->WorkSize);
	        ImGui::SetNextWindowViewport(viewport->ID);
	        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	        // and handle the pass-thru hole, so we ask Begin() to not render a background.

	        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	        // all active windows docked into it will lose their parent and become undocked.
	        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	        ImGui::Begin("DockSpace Demo", nullptr, imgui_window_flags);
	        ImGui::PopStyleVar();

	        ImGui::PopStyleVar(2);

	        // Submit the DockSpace
	        ImGuiIO& io = ImGui::GetIO();
	        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	        {
	            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	        }

	        if (ImGui::BeginMenuBar())
	        {
	            if (ImGui::BeginMenu("View"))
	            {
	                ImGui::MenuItem("Demo", NULL, &ViewWindow[0]);
	                ImGui::MenuItem("Viewport", NULL, &ViewWindow[1]);
	                ImGui::MenuItem("Map", NULL, &ViewWindow[2]);
	                ImGui::Separator();
	                // Disabling fullscreen would allow the window to be moved to the front of other windows,
	                // which we can't undo at the moment without finer window depth/z control.
	                ImGui::EndMenu();
	            }

	            if (ImGui::BeginMenu("Options"))
	            {
	                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, true))
	                {
		                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
						imgui_window_flags ^= ImGuiWindowFlags_NoBackground;
	                }

	                // Disabling fullscreen would allow the window to be moved to the front of other windows,
	                // which we can't undo at the moment without finer window depth/z control.
	                ImGui::EndMenu();
	            }

	            ImGui::EndMenuBar();
	        }


			//////////////////////////
			
			auto cpos = editor.GetCursorPosition();
			ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
			ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save", "Ctrl-S"))
					{
						auto textToSave = editor.GetText();
						/// save text....
						{
							std::fstream t(fileToEdit, std::ios::out | std::ios::trunc);
							if (t.good())
							{
								t << textToSave;
							}
						}
					}
					if (ImGui::MenuItem("Quit Editor", "Alt-F4"))
						;//break;///////////////////////////////////////////////////
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = editor.IsReadOnly();
					if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
						editor.SetReadOnly(ro);
					ImGui::Separator();

					if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
						editor.Undo();
					if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
						editor.Redo();

					ImGui::Separator();

					if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
						editor.Copy();
					if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
						editor.Cut();
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
						editor.Delete();
					if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
						editor.Paste();

					ImGui::Separator();

					if (ImGui::MenuItem("Select all", nullptr, nullptr))
						editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
				editor.IsOverwrite() ? "Ovr" : "Ins",
				editor.CanUndo() ? "*" : " ",
				editor.GetLanguageDefinition().mName.c_str(), fileToEdit.c_str());

			editor.Render("TextEditor");
			ImGui::End();

			//////////////////////////






			textureAtlasesGui.RenderImGui();
			Layer::RenderImGui(word);


	        ImGui::Begin("Viewport");
			//ImGui::Image((ImTextureID)textureColorbuffer, wsize, ImVec2(0.2f, 0.31f), ImVec2(0.25f, 0.25f));
			//ImGui::Image((ImTextureID)textureColorbuffer, ImVec2{ 160.f, 160.f }, ImVec2(0, 1), ImVec2(1, 0));
			//ImGui::Image((ImTextureID)textureColorbuffer, wsize, ImVec2(16.0f / 160.0f * 0, 1), ImVec2(16.0f / 160.0f * 1, 16.0f / 160.0f * 9));
			//sol üst ----- sağ alt
			//ImGui::Image((ImTextureID)textureAtlases.GetTextureId(), {200,200}, 
			//	ImVec2(textureAtlases.subTextures[0].imageCoord[3].x, textureAtlases.subTextures[0].imageCoord[3].y),
			//	ImVec2(textureAtlases.subTextures[0].imageCoord[0].x, textureAtlases.subTextures[0].imageCoord[0].y));
	        ImGui::End();


	        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	        ImGui::ShowDemoWindow();

			static bool open_log = true;
			Log::GetInstance().Draw("Log", &open_log);

	        ImGui::End();






			shader.Bind();
	        // Rendering
	        ImGui::Render();
	        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	        // Update and Render additional Platform Windows
	        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	        {
	            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
	            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
	            ImGui::UpdatePlatformWindows();
	            ImGui::RenderPlatformWindowsDefault();
	            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	        }

	        SDL_GL_SwapWindow(window);
	    }
	}
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
