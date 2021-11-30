#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "MathGeoLib/MathGeoLib.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	//bool Save(nlohmann::json &j) const override;
	//bool Load(nlohmann::json &j) override;

	void OnResize(int width, int height);

	void CalculateGlobalMatrix(GameObject* gameObject);

	void DrawBoundingBox(float3* points);

public:
	bool vsync;

	bool glDepthOn = true;
	bool glCullingOn = true;
	bool glLightingOn = true;
	bool glMatColorOn = true;
	bool glTex2dOn = true;
	bool glSmoothLineOn = true;
	bool glHardOn = true;
	bool glWireframeOn = false;
	bool showBBox = false;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};