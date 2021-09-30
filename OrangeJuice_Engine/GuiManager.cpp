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
			if (ImGui::MenuItem("Configuration")) show_window = true;
			if (ImGui::MenuItem("Demo Window")) show_demo_window = true;
			if (ImGui::MenuItem("Exit", "exit", false)) ret = false;


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) ImGui::EndMenu();

		if (ImGui::BeginMenu("Help")) ImGui::EndMenu();

		ImGui::EndMainMenuBar();
	}

	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
	if (show_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Configuration", &show_demo_window), window_flags)
		{
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

			if (ImGui::CollapsingHeader("Hardware"))
			{
				ImGui::TextWrapped("SDL Version: %i.%i.%i", App->system_specs.sdl_version.major, App->system_specs.sdl_version.minor, App->system_specs.sdl_version.patch);
				ImGui::TextWrapped("CPUs: %i", App->system_specs.cpus);
				ImGui::TextWrapped("System RAM: %f", App->system_specs.system_ram);
				ImGui::TextWrapped("Caps: ");
				if (App->system_specs.altivec)
					ImGui::TextWrapped("AltiVec");
				if (App->system_specs.rdtsc)
					ImGui::TextWrapped("RDTSC");
				if (App->system_specs.mmx)
					ImGui::TextWrapped("MMX");
				if (App->system_specs.sse)
					ImGui::TextWrapped("SSE");
				if (App->system_specs.sse2)
					ImGui::TextWrapped("SSE2");
				if (App->system_specs.sse3)
					ImGui::TextWrapped("SSE3");
				if (App->system_specs.sse41)
					ImGui::TextWrapped("SSE41");
				if (App->system_specs.sse42)
					ImGui::TextWrapped("SSE42");
				if (App->system_specs.three_d_now)
					ImGui::TextWrapped("3DNow");
				if (App->system_specs.avx)
					ImGui::TextWrapped("AVX");
				ImGui::Text("GPU vendor: %s", glGetString(GL_VENDOR));
				ImGui::Text("GPU Model: %s", glGetString(GL_RENDERER));
				ImGui::Text("GPU Drivers version: %s", glGetString(GL_VERSION));
			}
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