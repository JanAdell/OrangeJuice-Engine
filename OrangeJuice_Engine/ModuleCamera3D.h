#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "json.hpp"
#include <vector>
#include "../OrangeJuice_Engine/MathGeoLib/Math/MathAll.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Save(nlohmann::json& j) const override;
	bool Load(nlohmann::json& j) override;

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void GoAroundGeometry(GameObject* obj);

	std::vector<float3> AABBVertex(GameObject* obj, std::vector<float3> vertices);


private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference, ViewVector;
	float camera_speed = 0.01;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};