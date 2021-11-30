#include "Camera.h"

Camera::Camera(GameObject* gameObject) : Component(gameObject, type)
{
	type = COMPONENT_TYPE::COMPONENT_CAMERA;

	frustum.SetPerspective(40.0f, 30.0f);
	frustum.SetPos(float3(0.0f, 0.0f, 0.0f));
	frustum.SetFront(float3(0.0f, 0.0f, 1.0f));
	frustum.SetUp(float3(0.0f, 1.0f, 0.0f));
	frustum.SetViewPlaneDistances(0.3f, 5.0f);

	nearPlane =frustum.NearPlaneDistance();
	farPlane = frustum.FarPlaneDistance();
	horizontalFov = frustum.HorizontalFov();
	verticalFov = frustum.VerticalFov();

	parent = gameObject;

	RecalculateBBox();
}

Camera::~Camera()
{
}

void Camera::Enable()
{
}

void Camera::Update()
{
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

bool Camera::ContainsAABB(const AABB& boundingBox)
{
	const int planesNum = 6;
	const int cornersNum = 8;
	int counter = 0;
	int checker;

	Plane planes[planesNum];
	float3 corners[cornersNum];

	boundingBox.GetCornerPoints(corners);
	frustum.GetPlanes(planes);

	for (int i = 0; i < planesNum; i++)
	{
		//This number checks if the boundig box is outside of a plane
		checker = counter;

		for (int j = 0; j < cornersNum; j++)
		{
			if (!planes[i].IsOnPositiveSide(corners[j]))
			{
				counter++;
				break;
			}
		}
		if (checker == counter)
		{
			return false;
		}
	}

	if (counter == 6)
	{
		return true;
	}

	return false;
}

void Camera::CameraBBox()
{
	glLineWidth(1.5f);
	glColor3f(1, 0.5f, 1);

	glBegin(GL_LINES);

	glVertex3f(cameraBBox.CornerPoint(0).x, cameraBBox.CornerPoint(0).y, cameraBBox.CornerPoint(0).z);
	glVertex3f(cameraBBox.CornerPoint(1).x, cameraBBox.CornerPoint(1).y, cameraBBox.CornerPoint(1).z);

	glVertex3f(cameraBBox.CornerPoint(0).x, cameraBBox.CornerPoint(0).y, cameraBBox.CornerPoint(0).z);
	glVertex3f(cameraBBox.CornerPoint(2).x, cameraBBox.CornerPoint(2).y, cameraBBox.CornerPoint(2).z);

	glVertex3f(cameraBBox.CornerPoint(0).x, cameraBBox.CornerPoint(0).y, cameraBBox.CornerPoint(0).z);
	glVertex3f(cameraBBox.CornerPoint(4).x, cameraBBox.CornerPoint(4).y, cameraBBox.CornerPoint(4).z);

	glVertex3f(cameraBBox.CornerPoint(3).x, cameraBBox.CornerPoint(3).y, cameraBBox.CornerPoint(3).z);
	glVertex3f(cameraBBox.CornerPoint(1).x, cameraBBox.CornerPoint(1).y, cameraBBox.CornerPoint(1).z);

	glVertex3f(cameraBBox.CornerPoint(3).x, cameraBBox.CornerPoint(3).y, cameraBBox.CornerPoint(3).z);
	glVertex3f(cameraBBox.CornerPoint(2).x, cameraBBox.CornerPoint(2).y, cameraBBox.CornerPoint(2).z);

	glVertex3f(cameraBBox.CornerPoint(3).x, cameraBBox.CornerPoint(3).y, cameraBBox.CornerPoint(3).z);
	glVertex3f(cameraBBox.CornerPoint(7).x, cameraBBox.CornerPoint(7).y, cameraBBox.CornerPoint(7).z);

	glVertex3f(cameraBBox.CornerPoint(6).x, cameraBBox.CornerPoint(6).y, cameraBBox.CornerPoint(6).z);
	glVertex3f(cameraBBox.CornerPoint(2).x, cameraBBox.CornerPoint(2).y, cameraBBox.CornerPoint(2).z);

	glVertex3f(cameraBBox.CornerPoint(6).x, cameraBBox.CornerPoint(6).y, cameraBBox.CornerPoint(6).z);
	glVertex3f(cameraBBox.CornerPoint(4).x, cameraBBox.CornerPoint(4).y, cameraBBox.CornerPoint(4).z);

	glVertex3f(cameraBBox.CornerPoint(6).x, cameraBBox.CornerPoint(6).y, cameraBBox.CornerPoint(6).z);
	glVertex3f(cameraBBox.CornerPoint(7).x, cameraBBox.CornerPoint(7).y, cameraBBox.CornerPoint(7).z);

	glVertex3f(cameraBBox.CornerPoint(5).x, cameraBBox.CornerPoint(5).y, cameraBBox.CornerPoint(5).z);
	glVertex3f(cameraBBox.CornerPoint(1).x, cameraBBox.CornerPoint(1).y, cameraBBox.CornerPoint(1).z);

	glVertex3f(cameraBBox.CornerPoint(5).x, cameraBBox.CornerPoint(5).y, cameraBBox.CornerPoint(5).z);
	glVertex3f(cameraBBox.CornerPoint(4).x, cameraBBox.CornerPoint(4).y, cameraBBox.CornerPoint(4).z);

	glVertex3f(cameraBBox.CornerPoint(5).x, cameraBBox.CornerPoint(5).y, cameraBBox.CornerPoint(5).z);
	glVertex3f(cameraBBox.CornerPoint(7).x, cameraBBox.CornerPoint(7).y, cameraBBox.CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);
}

void Camera::RecalculateBBox()
{
	const int cornersNum = 8;

	float3 corner[cornersNum];
	frustum.GetCornerPoints(corner);

	cameraBBox.SetNegativeInfinity();
	cameraBBox.Enclose(corner, cornersNum);
}




