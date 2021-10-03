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

#include "json.hpp"
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

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
		/*json j;

		std::ifstream ifs("config.json");
		if (!ifs.is_open())
			LOG("Error to load file", SDL_GetError());

		ifs >> j;

		std::string str0 = j["info"]["name"].get<std::string>();*/

		static char str1;
		//std::strcpy(&str1, str0.c_str());

		ImGui::Text("App Name:     ");
		ImGui::SameLine(); ImGui::InputText(" ", &str1, IM_ARRAYSIZE(&str1));

		ImGui::Text("Organitzation:");
		ImGui::SameLine();
		if (ImGui::Button("UPC CITM", ImVec2(357, 0))) App->RequestBrowser("https://www.citm.upc.edu/");

		//------------- FPS GRAPH
		if (ImGui::CollapsingHeader("FPS"))
		{
			uint min = 0;
			uint max = 144;
			ImGui::SliderScalar(" Max FPS", ImGuiDataType_U32, &App->maxFrames, &min, &max, "%d");

			ImGui::Text("Limit FPS:");
			int frames;
			float millisec;
			App->GetFrames(frames, millisec);

			if (fpsLog.size() > 100)
			{
				fpsLog.erase(fpsLog.begin());
				//fpsLog.pop_back();
				//msLog.pop_back();
			}

			fpsLog.push_back(frames);
			msLog.push_back(millisec);

			LOG("%i", fpsLog.size());

			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fpsLog[fpsLog.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fpsLog[0], fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %0.1f", msLog[msLog.size() - 1]);
			ImGui::PlotHistogram("##Milliseconds", &msLog[0], msLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		}
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
		int width = 0;
		int height = 0;
		ImGui::Checkbox(" Active", &App->renderer3D->vsync);
		ImGui::TextWrapped("Icon:  *default*");
		ImGui::SliderFloat(" Brightness", &brightness, 0, 1);
		ImGui::SliderInt(" Width", &width, 0, 3820);
		ImGui::SliderInt(" Height", &height, 0, 2048);
		SDL_SetWindowBrightness(App->window->window, brightness);
		
		const char* items[] = { "800x600", "1024x768", "1152x864", "1176x664","1280x720", "1280x768", "1280x800", "1280x960", "1280x1024", "1360x768", "1440x900", "1920x1080" };
		static int item_current = 9;
		ImGui::Combo(" Resolution", &item_current, items, IM_ARRAYSIZE(items));
		switch (item_current)
		{
		case 0:
			SDL_SetWindowSize(App->window->window, 800, 600);
			break;
		case 1:
			SDL_SetWindowSize(App->window->window, 1152, 864);
			break;
		case 2:
			SDL_SetWindowSize(App->window->window, 1152, 864);
			break;
		case 3:
			SDL_SetWindowSize(App->window->window, 1176, 664);
			break;
		case 4:
			SDL_SetWindowSize(App->window->window, 1280, 720);
			break;
		case 5:
			SDL_SetWindowSize(App->window->window, 1280, 768);
			break;
		case 6:
			SDL_SetWindowSize(App->window->window, 1280, 800);
			break;
		case 7:
			SDL_SetWindowSize(App->window->window, 1280, 960);
			break;
		case 8:
			SDL_SetWindowSize(App->window->window, 1280, 1024);
			break;
		case 9:
			SDL_SetWindowSize(App->window->window, 1360, 768);
			break;
		case 10:
			SDL_SetWindowSize(App->window->window, 1440, 900);
			break;
		case 11:
			SDL_SetWindowSize(App->window->window, 1920, 1080);
			break;
		default:
			break;
		}

		ImGui::TextWrapped("Refresh Rate: ");

		if (ImGui::Checkbox(" Fullscreen", &fullscreen))
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
				fullscreen = true;
			}

			else fullscreen = false;
		}

		ImGui::SameLine();
		if (ImGui::Checkbox(" Resizable", &resizable))
		{
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Restart to apply");
			if (resizable)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_RESIZABLE);
				resizable = true;
			}
			else resizable = false;
		}

		if (ImGui::Checkbox(" Borderless", &borderless))
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
		if (ImGui::Checkbox(" Full Desktop", &full_desktop))
		{
			if (full_desktop)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				full_desktop = true;
			}
			else full_desktop = false;
		}
	}//window tab

}//config window