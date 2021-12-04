#pragma once
#include "Module.h"
#include "Globals.h"
#include "Camera.h"
#include "glmath.h"
#include "json.hpp"
#include <vector>
#include "../OrangeJuice_Engine/MathGeoLib/Math/MathAll.h"

class Camera;

struct MouseHit
{
	float distance = 0.f;
	GameObject* object = nullptr;
};

struct LesThanKey
{
	inline bool operator()(const MouseHit& struct1, const MouseHit& struct2)
	{
		return (struct1.distance < struct2.distance);
	}
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	bool Save(nlohmann::json& j) const override;
	bool Load(nlohmann::json& j) override;

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void BeInCamera(const AABB& bbox);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void GoAroundGeometry(GameObject* obj);
	void MousePicking();
	bool LessThan(float i, float j) { return i < j; }

	std::vector<float3> AABBVertex(GameObject* obj, std::vector<float3> vertices);

private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference, ViewVector;
	float cameraSpeed = 0.01;
	bool write = false;

	Camera* camera;
	AABB LookedBBox;

	LineSegment ray;

private:
	Frustum* camFrustum = nullptr;
	mat4x4 ViewMatrix, ViewMatrixInverse;
};