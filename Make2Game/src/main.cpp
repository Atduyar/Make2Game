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
#include "Shader.h"
#include "Texture.h"
#include "VertexBufferLayout.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


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
    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);


    ///////////////////////////////////////////////////
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // generate texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 200, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    //unsigned int rbo;
    //glGenRenderbuffers(1, &rbo);
    //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1366, 768);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ///////////////////////////////////////////////////
    {
	    float positions[]{
	         50.0f,  00.0f, 1.0f, 0.0f,
	    	 00.0f,  00.0f, 0.0f, 0.0f,
	         50.0f,  50.0f, 1.0f, 1.0f,
	         00.0f,  50.0f, 0.0f, 1.0f
	    };

	    uint32_t indices[] = {
	        0, 1, 2,
	        1, 2, 3
	    };

        VertexArray va;
	    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
        va.AddBuffer(vb, layout);
		
	    IndexBuffer ib(indices, 6);

		int wx, wy;
		SDL_GetWindowSize(window, &wx, &wy);

		glm::mat4 proj	= glm::ortho(0.0f, (float)wx, 0.0f, (float)wy, -1.0f, 1.0f);
		glm::mat4 view	= glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
		glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)), glm::vec3(5, 5, 5));
		;

		glm::mat4 MVP = proj * view * model;

		Log::GetInstance().AddLog(std::to_string(wx) + " - " + std::to_string(wy));

		Shader shader("./res/shaders/Texture.shader");
		shader.Bind();
		//shader.SetUniform4f("u_Color", 0, 1, 1, 1);

		Texture texture("./res/img/apple.png");
		texture.Bind(); 
    	shader.SetUniform1i("u_Texture", 0);
		shader.SetUniformMat4f("u_MVP", MVP);

		shader.Unbind();
		va.Unbind();
		ib.Unbind();
		vb.Unbind();
		
		Renderer renderer;
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
	        SDL_Event event;
	        while (SDL_PollEvent(&event))
	        {
	            ImGui_ImplSDL2_ProcessEvent(&event);
	            if (event.type == SDL_QUIT)
	                done = true;
	            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
	                done = true;
				if (event.type == SDL_KEYDOWN){
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							done = true;
							break;
						}
					break;
				}
	        }



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



			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glEnable(GL_DEPTH_TEST);
			//glViewport(0, 0, 200, 200);
			//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
			
			glClearColor(0.0f, 1.0f, 0.3f, 1.0f);
			renderer.Clear();
			
			shader.Bind();
			renderer.Draw(va, ib, shader);



			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

			glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
			renderer.Clear();

			shader.Bind();
			renderer.Draw(va,ib,shader);









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












	        ImGui::Begin("Viewport");
	        ImVec2 wsize = ImGui::GetWindowSize();
	        wsize.y -= 35;
	        wsize.y = 200;
	        wsize.x = 200;
	        ImGui::Image((ImTextureID)textureColorbuffer, wsize, ImVec2(0, 1), ImVec2(1, 0));
	        ImGui::End();

	        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	        ImGui::ShowDemoWindow();

			static bool open_log = true;
			Log::GetInstance().Draw("Log", &open_log);

	        ImGui::End();







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
