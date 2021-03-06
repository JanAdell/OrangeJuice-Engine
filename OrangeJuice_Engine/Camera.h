#pragma once

#include "Component.h"
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"

#include "MathGeoLib/MathGeoLib.h"
#include "ImGui/imgui.h"

#include <vector>

class Camera : public Component
{
public:

	Camera(GameObject* gameObject);
	~Camera();

	virtual void Enable() override;
	virtual void Update() override;
	virtual void Disable() override;

	void ShowProperties() {};
	std::string GetTexturePath();
	int GetTextureId();
	int SetTextureId(int id);

	bool ContainsAABB(const AABB& boundingBox);
	void CameraBBox();
	void RecalculateBBox();

public:

	Frustum frustum;
	AABB cameraBBox;

	float nearPlane;
	float farPlane;
	float horizontalFov;
	float verticalFov;
};

