#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "Globals.h"

#include "MathGeoLib/MathBuildConfig.h"
#include "MathGeoLib/MathGeoLib.h"
#include "ConsoleManager.h"
#include "Geometry.h"

#include "parson/parson.h"
#include "par_shapes.h"

#include <vector>


typedef unsigned char GLubyte;
typedef float GLfloat;


class GuiManager : public Module
{
public:
	GuiManager(Application* app, bool start_enabled = true);
	~GuiManager();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void AboutWindow();
	void ConfigWindow();
	void ShowAppConsole(bool show_console);
	void PrimitivesWindow();
	void DrawGeometry();
	void CreatePrimitives(par_shapes_mesh* p_mesh, Primitives prim, float color[4], int scale[3], int translation[3], float rad, const float* axis);
	void HierarchyWindow();

	void GetLog(const char* log);

	bool Save(nlohmann::json& j) const override;
	bool Load(nlohmann::json& j) override;

private:

	bool showDemoWindow = false;
	bool showConfigWindow = false;
	bool showAboutWindow = false;
	bool showConsoleWindow = false;
	bool showPrimitivesWindow = false;
	bool showHierachyWindow = false;

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;

	uint idVertex = 0;
	uint idIndex = 0;
	
	//float brightness = 1.0f;

	std::vector<float> fpsLog;
	std::vector<float> msLog;

	SDL_WindowFlags window_flags;
	SDL_GLContext gl_context;
	
	AppConsole console;
	ImGuiTextBuffer text;

	Primitives prim = Primitives::UKNOWN;

	JSON_Value* root;
};


#endif
