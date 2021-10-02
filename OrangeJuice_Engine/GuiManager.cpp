#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include <stdio.h>
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "GuiManager.h"
#include "ModuleRenderer3D.h"

GuiManager::GuiManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

GuiManager::~GuiManager()
{
}

bool GuiManager::Init()
{
	bool ret = true;
	ImGui::CreateContext();


	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	return true;
}

bool GuiManager::Start()
{

	return true;
}

update_status GuiManager::PreUpdate(float dt)
{
	bool ret = true;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Configuration")) show_config_window = true;
			if (ImGui::MenuItem("Demo Window")) show_demo_window = true;
			if (ImGui::MenuItem("Exit", "exit", false)) ret = false;


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) ImGui::EndMenu();

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Repository"))
				App->RequestBrowser("https://github.com/JanAdell/OrangeJuice-Engine");

			if (ImGui::MenuItem("About",NULL, show_about_window)) show_about_window = !show_about_window;;
				

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
	
	if (show_config_window)
	{
		if (ImGui::Begin("Configuration", &show_config_window), window_flags)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
			ConfigWindow();
		}
		ImGui::End();
	}

	if (show_about_window)
	{
		if (ImGui::Begin("About", &show_about_window), window_flags)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
			AboutWindow();
		}
		ImGui::End();
	}

	return ret ? UPDATE_CONTINUE : UPDATE_STOP;
}

update_status GuiManager::Update(float dt)
{

	return UPDATE_CONTINUE;
}

update_status GuiManager::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

bool GuiManager::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();
	return true;
}

void GuiManager::AboutWindow()
{
	ImGui::Text("Orange Juice Engine v0.1");
	ImGui::NewLine();
	ImGui::Text("A 3D engine created by Jan Adell Mill and Oscar Reguera Parera");
	ImGui::NewLine();
}

void GuiManager::ConfigWindow()
{
	//-------- APPLICATION TAB
	if (ImGui::CollapsingHeader("Application"))
	{
		static char str0[128] = "Orange Juice Engine";

		ImGui::Text("App Name:     ");
		ImGui::SameLine(); ImGui::InputText(" ", str0, IM_ARRAYSIZE(str0));

		ImGui::Text("Organitzation:");
		ImGui::SameLine();
		if (ImGui::Button("UPC CITM", ImVec2(357, 0))) App->RequestBrowser("https://www.citm.upc.edu/");

		ImGui::Text("FPS:");
		uint min = 0;
		uint max = 144;
		ImGui::SliderScalar("Max FPS", ImGuiDataType_U32, &App->maxFrames, &min, &max, "%d");
	}

	//-------- HARDWARE TAB
	if (ImGui::CollapsingHeader("Hardware"))
	{
		ImGui::TextWrapped("SDL version:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%i.%i.%i", App->systemSpecs.sdlVersion.major, App->systemSpecs.sdlVersion.minor, App->systemSpecs.sdlVersion.patch);
		ImGui::TextWrapped("CPUs:", App->systemSpecs.cpus);
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%i", App->systemSpecs.cpus);
		ImGui::TextWrapped("System Ram:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%f", App->systemSpecs.systemRam);
		ImGui::TextWrapped("Caps:");
		if (App->systemSpecs.altivec)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "AltiVec,");
		}
		if (App->systemSpecs.rdtsc)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "RDTSC,");
		}
		if (App->systemSpecs.mmx)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "MMX,");
		}
		if (App->systemSpecs.sse)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "SSE,");
		}
		if (App->systemSpecs.sse2)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "SSE2,");
		}
		if (App->systemSpecs.sse3)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "SSE3,");
		}
		if (App->systemSpecs.sse41)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "SSE41,");
		}
		if (App->systemSpecs.sse42)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "SSE42,");
		}
		if (App->systemSpecs.threeDnow)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "3DNow,");
		}
		if (App->systemSpecs.avx)
		{
			ImGui::SameLine();
			ImGui::TextColored({ 255, 255, 0, 255 }, "AVX,");
		}
		ImGui::Text("GPU vendor:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%s", glGetString(GL_VENDOR));
		ImGui::Text("GPU Model:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%s", glGetString(GL_RENDERER));
		ImGui::Text("GPU Drivers version:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%s", glGetString(GL_VERSION));
	}//hardware tab

	//-------- WINDOW TAB
	if (ImGui::CollapsingHeader("Window"))
	{
		bool active = true;
		float brightness = 1.0f;
		int width = 0;
		int height = 0;
		ImGui::Checkbox("Active", &active);
		ImGui::TextWrapped("Icon:  *default*");
		ImGui::SliderFloat("Brightness", &brightness, 0, 1);
		ImGui::SliderInt("Width", &width, 0, 3820);
		ImGui::SliderInt("Height", &height, 0, 2048);
		ImGui::TextWrapped("Refresh Rate: ");

		if (ImGui::Checkbox("Fullscreen", &fullscreen))
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
				fullscreen = true;
			}

			else
				fullscreen = false;
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
		{
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Restart to apply");
			if (resizable)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_RESIZABLE);
				resizable = true;
			}
			else
				resizable = false;
		}

		if (ImGui::Checkbox("Borderless", &borderless))
		{
			if (borderless)
			{
				SDL_SetWindowBordered(App->window->window, SDL_FALSE);
				borderless = true;
			}
			else
			{
				SDL_SetWindowBordered(App->window->window, SDL_TRUE);
				borderless = false;
			}
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &full_desktop))
		{
			if (full_desktop)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				full_desktop = true;
			}
			else
				full_desktop = false;
		}
	}//window tab

}//config window