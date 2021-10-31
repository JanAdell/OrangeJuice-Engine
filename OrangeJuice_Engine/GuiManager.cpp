#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include <stdio.h>
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "GuiManager.h"
#include "ModuleRenderer3D.h"
#include "json.hpp"
#include "ModuleMesh.h"

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
	
	return true;
}

bool GuiManager::Start()
{

	return true;
}

update_status GuiManager::PreUpdate(float dt)
{
	bool ret = true;
	

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Configuration")) showConfigWindow = true;
			if (ImGui::MenuItem("Demo Window")) showDemoWindow = true;
			if (ImGui::MenuItem("Exit", "exit", false)) ret = false;


			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Primitives")) showPrimitivesWindow = true;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) 
		{
			if (ImGui::MenuItem("Console")) showConsoleWindow = true;
			if (ImGui::MenuItem("Inspector")) showHierachyWindow = true;
			if (ImGui::MenuItem("Textures")) showTextureWindow = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Repository"))
				App->RequestBrowser("https://github.com/JanAdell/OrangeJuice-Engine");

			if (ImGui::MenuItem("Last release"))
				App->RequestBrowser("https://github.com/JanAdell/OrangeJuice-Engine/releases");

			if (ImGui::MenuItem("Report a bug"))
				App->RequestBrowser("https://github.com/JanAdell/OrangeJuice-Engine/issues");

			if (ImGui::MenuItem("About",NULL, showAboutWindow)) showAboutWindow = !showAboutWindow;;
				

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (showConsoleWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		ShowAppConsole(&showConsoleWindow);
	}

	if (showPrimitivesWindow) PrimitivesWindow();

	if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
	
	if (showConfigWindow)
	{
		if (ImGui::Begin("Configuration", &showConfigWindow, window_flags))
		{
			char a[100] = "";
			
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
			
			if (ImGui::InputText("", a, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				test = a;
				
			}
			ImGui::Text(test.c_str());

			ConfigWindow();
			
		}
		ImGui::End();
	}

	if (showAboutWindow)
	{
		if (ImGui::Begin("About", &showAboutWindow), window_flags)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
			AboutWindow();
		}
		ImGui::End();
	}

	if (showHierachyWindow) HierarchyWindow();

	if (showTextureWindow) TextureWindow();

	const char* p_file = App->input->DragAndDrop();
	if (p_file != nullptr)
	{
		App->mesh->LoadFile(p_file);
		LOG("%s", p_file);
	}
		
	return ret ? UPDATE_CONTINUE : UPDATE_STOP;
}

update_status GuiManager::Update(float dt)
{
	if(showConsoleWindow) ShowAppConsole(showConsoleWindow);
	
	return UPDATE_CONTINUE;
}

update_status GuiManager::PostUpdate(float dt)
{
	
	return UPDATE_CONTINUE;
}

bool GuiManager::CleanUp()
{

	fpsLog.clear();
	msLog.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();
	return true;
}

bool GuiManager::Save(nlohmann::json & j) const
{
	return true;
}

bool GuiManager::Load(nlohmann::json & j)
{
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
	json j;
	std::ifstream ifs("config.json");
	if (!ifs.is_open())
		LOG("Error to load file", SDL_GetError());

	ifs >> j;
	//-------- APPLICATION TAB
	if (ImGui::CollapsingHeader("Application"))
	{
		
		const char* str1 = "NONE";
		const char* str2 = "NONE";

		
		root = json_parse_file((std::string("config.json")).data());

		str1 = json_object_get_string(json_object_get_object(json_value_get_object(root),"info"), "name");
		str2 = json_object_get_string(json_object_get_object(json_value_get_object(root), "info"), "Organisation");
				
		ImGui::Text("App Name:    %s ", str1);
		
		ImGui::Text("Organitsation:");
		ImGui::SameLine();
		if (ImGui::Button(str2, ImVec2(357, 0))) App->RequestBrowser("https://www.citm.upc.edu/");

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

			//LOG("%i", fpsLog.size());

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

		VramConsumption();
		
	}//hardware tab

	//-------- WINDOW TAB
	if (ImGui::CollapsingHeader("Window"))
	{
		bool active = true;
		int width = 0;
		int height = 0;
		ImGui::Checkbox(" Active", &App->renderer3D->vsync);
		ImGui::TextWrapped("Icon:  *default*");
		ImGui::SliderFloat(" Brightness", &App->window->brightness, 0, 1);
		ImGui::SliderInt(" Width", &width, 0, 3820);
		ImGui::SliderInt(" Height", &height, 0, 2048);
		SDL_SetWindowBrightness(App->window->window, App->window->brightness);
		
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

			else 
			{
				SDL_SetWindowFullscreen(App->window->window, 0);
				fullscreen = false;
			}
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
			else 
			{
				SDL_SetWindowFullscreen(App->window->window, 0);
				resizable = false;
			}
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

	//-------- RENDERER TAB
	if (ImGui::CollapsingHeader("Renderer"))
	{
		if (ImGui::Checkbox("GL Depth", &App->renderer3D->glDepthOn))
			(&App->renderer3D->glDepthOn) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

		else if (ImGui::Checkbox("GL Cull Face", &App->renderer3D->glCullingOn))
			(App->renderer3D->glCullingOn) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

		else if (ImGui::Checkbox("GL Lighting", &App->renderer3D->glLightingOn))
			(App->renderer3D->glLightingOn) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

		else if (ImGui::Checkbox("GL Color Material", &App->renderer3D->glMatColorOn))
			(App->renderer3D->glMatColorOn) ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);

		else if (ImGui::Checkbox("GL Texture 2D", &App->renderer3D->glTex2dOn))
			(App->renderer3D->glTex2dOn) ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);

		else if (ImGui::Checkbox("GL Line Smooth", &App->renderer3D->glSmoothLineOn))
			(App->renderer3D->glSmoothLineOn) ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);

		else if (ImGui::Checkbox("Hard Poly", &App->renderer3D->glHardOn))
			(App->renderer3D->glHardOn) ? glShadeModel(GL_FLAT) : glShadeModel(GL_SMOOTH);

		else if (ImGui::Checkbox("Wireframe mode", &App->renderer3D->glWireframeOn))
		{
			if (App->renderer3D->glWireframeOn)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

}//config window

void GuiManager::ShowAppConsole(bool show_console)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	
	console.Draw("Orange Juice Engine", &show_console);

	ImGui::End();

}//console 

void GuiManager::GetLog(const char* log)
{
	console.AddLog(log);

}//console logs

void GuiManager::PrimitivesWindow()
{
	if (showPrimitivesWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		par_shapes_mesh* m = nullptr;
		if (ImGui::Begin("Create Primitives", &showPrimitivesWindow))
		{
			static float col[4] = { 0.4f,0.7f,0.0f,0.5f };
			static int scale[3] = { 1,1,1 };
			static int translation[3] = { 1,1,1 };
			static int rad = 0;
			static float axis[3] = { 0,0,0 };
			if (ImGui::CollapsingHeader("Primitives"))
			{
				ImGui::ColorEdit4("Color", col);
				if (ImGui::CollapsingHeader("Cube"))
				{
					ImGui::SliderInt3("Size", scale, 1, 10);
					ImGui::SliderInt3("Translation", translation, -100, 100);
					ImGui::TextWrapped("Rotation");
					ImGui::Separator();
					ImGui::SliderInt("Radiant", &rad, 0, 360);
					const char* items[] = { "X", "Y", "Z" };
					static int item_current = 0;
					ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
					switch (item_current)
					{
					case 0:
						axis[0] = 1;
						axis[1] = 0;
						axis[2] = 0;
						break;
					case 1:
						axis[0] = 0;
						axis[1] = 1;
						axis[2] = 0;
						break;
					case 2:
						axis[0] = 0;
						axis[1] = 0;
						axis[2] = 1;
						break;
					}
					DegToRad(rad);
					ImGui::ColorEdit4("Color", col);
					if (ImGui::Button("Create"))
						CreatePrimitives(m, Primitives::CUBE, col, scale, translation, rad, axis);
				}
				
				if (ImGui::CollapsingHeader("Sphere"))
				{
					ImGui::SliderInt3("Size", scale, 1, 10);
					ImGui::SliderInt3("Translation", translation, -100, 100);
					ImGui::TextWrapped("Rotation");
					ImGui::Separator();
					ImGui::SliderInt("Radiant", &rad, 0, 360);
					const char* items[] = { "X", "Y", "Z" };
					static int item_current = 0;
					ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
					switch (item_current)
					{
					case 0:
						axis[0] = 1;
						axis[1] = 0;
						axis[2] = 0;
						break;
					case 1:
						axis[0] = 0;
						axis[1] = 1;
						axis[2] = 0;
						break;
					case 2:
						axis[0] = 0;
						axis[1] = 0;
						axis[2] = 1;
						break;
					}
					DegToRad(rad);
					ImGui::ColorEdit4("Color", col);
					if (ImGui::Button("Create"))
						CreatePrimitives(m, Primitives::SPHERE, col, scale, translation, rad, axis);
				}
				if (ImGui::CollapsingHeader("Cone"))
				{
					ImGui::SliderInt3("Size", scale, 1, 10);
					ImGui::SliderInt3("Translation", translation, -100, 100);
					ImGui::TextWrapped("Rotation");
					ImGui::Separator();
					ImGui::SliderInt("Radiant", &rad, 0, 360);
					const char* items[] = { "X", "Y", "Z" };
					static int item_current = 0;
					ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
					switch (item_current)
					{
					case 0:
						axis[0] = 1;
						axis[1] = 0;
						axis[2] = 0;
						break;
					case 1:
						axis[0] = 0;
						axis[1] = 1;
						axis[2] = 0;
						break;
					case 2:
						axis[0] = 0;
						axis[1] = 0;
						axis[2] = 1;
						break;
					}
					DegToRad(rad);
					ImGui::ColorEdit4("Color", col);
					if (ImGui::Button("Create"))
						CreatePrimitives(m, Primitives::CONE, col, scale, translation, rad, axis);
				}
				if (ImGui::CollapsingHeader("Cylinder"))
				{
					ImGui::SliderInt3("Size", scale, 1, 10);
					ImGui::SliderInt3("Translation", translation, -100, 100);
					ImGui::TextWrapped("Rotation");
					ImGui::Separator();
					ImGui::SliderInt("Radiant", &rad, 0, 360);
					const char* items[] = { "X", "Y", "Z" };
					static int item_current = 0;
					ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
					switch (item_current)
					{
					case 0:
						axis[0] = 1;
						axis[1] = 0;
						axis[2] = 0;
						break;
					case 1:
						axis[0] = 0;
						axis[1] = 1;
						axis[2] = 0;
						break;
					case 2:
						axis[0] = 0;
						axis[1] = 0;
						axis[2] = 1;
						break;
					}
					DegToRad(rad);
					ImGui::ColorEdit4("Color", col);
					if (ImGui::Button("Create"))
						CreatePrimitives(m, Primitives::CYILINDER, col, scale, translation, rad, axis);
				}
				if (ImGui::CollapsingHeader("Plane"))
				{
					ImGui::SliderInt3("Size", scale, 1, 10);
					ImGui::SliderInt3("Translation", translation, -100, 100);
					ImGui::TextWrapped("Rotation");
					ImGui::Separator();
					ImGui::SliderInt("Radiant", &rad, 0, 360);
					const char* items[] = { "X", "Y", "Z" };
					static int item_current = 0;
					ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
					switch (item_current)
					{
					case 0:
						axis[0] = 1;
						axis[1] = 0;
						axis[2] = 0;
						break;
					case 1:
						axis[0] = 0;
						axis[1] = 1;
						axis[2] = 0;
						break;
					case 2:
						axis[0] = 0;
						axis[1] = 0;
						axis[2] = 1;
						break;
					}
					DegToRad(rad);
					ImGui::ColorEdit4("Color", col);
					if (ImGui::Button("Create"))
						CreatePrimitives(m, Primitives::PLANE, col, scale, translation, rad, axis);
				}
			}
			ImGui::End();

		}
	}
}

void GuiManager::DrawGeometry()
{
	//	for (std::vector<Geometry*>::iterator it = App->mesh->geometry.begin(); it != App->mesh->geometry.end(); it++)
//	{
//		/*(*it)->Draw();
//		if (debug_draw)
//			(*it)->DebugDraw();*/
//	}
//	for (std::vector<Geometry*>::iterator it = geoms.begin(); it != geoms.end(); it++)
//	{
//		(*it)->DrawPrimitives();
//		if (debug_draw)
//			(*it)->DebugDraw();
//	}
}

void GuiManager::CreatePrimitives(par_shapes_mesh* p_mesh, Primitives prim, float col[4], int scale[3], int translation[3], float rad, const float* axis)
{
	switch (prim)
	{
	case Primitives::CUBE:
		p_mesh = par_shapes_create_cube();
		break;
	case Primitives::SPHERE:
		p_mesh = par_shapes_create_subdivided_sphere(5);
		break;
	case Primitives::CONE:
		p_mesh = par_shapes_create_cone(30, 3);
		break;
	case Primitives::CYILINDER:
		p_mesh = par_shapes_create_cylinder(30, 3);
		break;
	case Primitives::PLANE:
		p_mesh = par_shapes_create_plane(30, 3);
		break;
	default:
		LOG("Unknown primtive selected");
		break;
	}

	GameObject* game_object = new GameObject();
	Geometry* geo = dynamic_cast<Geometry*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH));
	geo->CreatePrimitive(p_mesh, col[0], col[1], col[2], col[3]);
	App->scene->gameObjects.push_back(game_object);
	//App->camera->GoAroundGeometry(geo);
	par_shapes_free_mesh(p_mesh);
}

void GuiManager::HierarchyWindow()
{
	if (showHierachyWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Objects", &showHierachyWindow))
		{
			if (ImGui::CollapsingHeader("Geometry"))
			{
				static int selection_mask = (1 << 0);
				static int node_clicked = 0;

				for (uint i = 0; i < App->scene->gameObjects.size(); ++i)
				{
					GameObject* game_object = App->scene->gameObjects[i];

					if (game_object->parent == nullptr)
					{
						// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
						ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
						if (selection_mask & (1 << i))
							node_flags |= ImGuiTreeNodeFlags_Selected;

						bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, game_object->name.c_str());
						if (ImGui::IsItemClicked())
						{
							selection_mask = (1 << i);
							game_object->showInspectorWindow = true;

							//al show inspector windows = false
							std::vector<GameObject*>::iterator iterator = App->scene->gameObjects.begin();
							while (iterator != App->scene->gameObjects.end())
							{
								if (*iterator != game_object) (*iterator)->showInspectorWindow = false;
								++iterator;
							}
						}

						if (game_object->children.size() != 0)
						{
							std::vector<GameObject*>::iterator child = game_object->children.begin();
							while (child != game_object->children.end())
							{
								if (*child != game_object)
									(*child)->showInspectorWindow = false;
								++child;
							}
						}
						if (game_object->showInspectorWindow)
						{
							game_object->GetPropierties();
							App->scene->gameObjectSelect = game_object;
						}
						if (node_open)
						{
							game_object->GetHierarcy(); ImGui::TreePop();
						}
					}
				}
			}
		}
		ImGui::End();
	}
}

void GuiManager::TextureWindow()
{
	if (showTextureWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Textures", &showTextureWindow))
		{
			for (uint i = 0; i < App->scene->textures.size(); ++i)
			{
				std::string node_name = "Texture " + std::to_string(i + 1);
				if (ImGui::TreeNodeEx(node_name.c_str()))
				{
					ImVec2 size = { 200,200 };
					int id = App->scene->textures[i];
					ImGui::Image((ImTextureID)id, size);
					ImGui::TextColored(ImVec4(0, 0, 255, 255), "%i x %i", (int)size.x, (int)size.y);
					ImGui::TreePop();
				}
			}
		} 
		ImGui::End();
	}
}

void GuiManager::VramConsumption()
{
	const GLubyte* v = glGetString(GL_VENDOR);
	char vendor[30];

	for (int i = 0; i < 30; i++)
	{
		vendor[i] = v[i];
	}
	//Nvidia VRAM usage
	if (strcmp(vendor, "NVIDIA Corporation") == 0)
	{
		GLint total_mem_kb = 0;

		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_mem_kb);

		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);

		//Calculate VRAM usage
		char vram[25];
		vramLog.push_back(((float)total_mem_kb) / 1000.f);
		if (vramLog.size() > 100)
		{
			for (int i = 0; vramLog.size() > i - 1; i++)
			{
				vramLog[i] = vramLog[i + 1];
			}
			vramLog.pop_back();
		}
		sprintf_s(vram, 25, "VRAM usage: %.3f Mb", ((float)total_mem_kb) / 1000.f);
		ImGui::PlotHistogram("##VRAM usage: ", &vramLog[0], vramLog.size(), 0, vram, 0.0f, 100.0f, ImVec2(310, 100));

		//Calculate VRAM aviable
		char vram_curr[50];
		currVramLog.push_back(((float)cur_avail_mem_kb) / 1000.f);
		if (currVramLog.size() > 100)
		{
			for (int i = 0; currVramLog.size() > i - 1; i++)
			{
				currVramLog[i] = currVramLog[i + 1];
			}
			currVramLog.pop_back();
		}
		sprintf_s(vram_curr, 50, "VRAM Aviable: %.3f Mb", ((float)cur_avail_mem_kb) / 1000.f);
		ImGui::PlotHistogram("##VRAM Aviable: ", &currVramLog[0], currVramLog.size(), 0, vram_curr, 0.0f, 100.0f, ImVec2(310, 100));

	}
	//AMD VRAM usage still in project
	/*else if (strcmp(vendor, "ATI Technologies Inc.") == 0)
	{
		GLint nCurAvailMemoryInKB = 0;
		glGetIntegerv(GL_ATI_meminfo,
			&nCurAvailMemoryInKB);
		char vram_curr[50];
		currVramLog.push_back(((float)nCurAvailMemoryInKB) / 1000.f);
		if (currVramLog.size() > 100)
		{
			for (int i = 0; currVramLog.size() > i - 1; i++)
			{
				currVramLog[i] = currVramLog[i + 1];
			}
			currVramLog.pop_back();
		}
		sprintf_s(vram_curr, 50, "VRAM Aviable: %.3f Mb", ((float)nCurAvailMemoryInKB) / 1000.f);
		ImGui::PlotHistogram("##VRAM Aviable: ", &currVramLog[0], currVramLog.size(), 0, vram_curr, 0.0f, 100.0f, ImVec2(310, 100));

	}*/
	else (ImGui::TextColored(ImVec4(255, 0, 0, 255), "VRam Usage only available for NVIDIA devices at the moment"));
}