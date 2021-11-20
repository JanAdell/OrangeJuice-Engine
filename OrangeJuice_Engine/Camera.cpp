#include "Camera.h"

Camera::Camera(GameObject* gameObject) : Component(gameObject, type)
{
	type = COMPONENT_TYPE::COMPONENT_CAMERA;

	frustum.SetPerspective(40.0f, 30.0f);
	frustum.SetPos(float3(0.0f, 0.0f, 0.0f));
	frustum.SetFront(float3(0.0f, 0.0f, 1.0f));
	frustum.SetUp(float3(0.0f, 1.0f, 0.0f));
	frustum.SetViewPlaneDistances(0.3f, 20.0f);

	parent = gameObject;
}

Camera::~Camera()
{
}

void Camera::Enable()
{
}

void Camera::Update()
{
	//const vec pos = { parent->transform->translation.x, parent->transform->translation.y, parent->transform->translation.z };
	frustum.SetPos(parent->transform->GetTranslation());
	float3 points[8];
	frustum.GetCornerPoints(points);
	App->renderer3D->DrawBoundingBox(points);
}

void Camera::Disable()
{
}

std::string Camera::GetTexturePath()
{
	return std::string();
}

int Camera::GetTextureId()
{
	return 0;
}

int Camera::SetTextureId(int id)
{
	return 0;
}