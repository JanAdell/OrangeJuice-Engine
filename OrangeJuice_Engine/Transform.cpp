#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
#include "Geometry.h"
#include "par_shapes.h"
#include <vector>
#include "ImGui/imgui.h"
#include "Assimp/include/scene.h"
#include "../OrangeJuice_Engine/MathGeoLib/MathGeoLib.h"

Transform::Transform(GameObject* parent) :Component(parent, COMPONENT_TYPE::COMPONENT_TRANSFORM)
{
}

Transform::~Transform()
{
}

void Transform::Enable()
{
}

void Transform::Update()
{
}

void Transform::Disable()
{
}

void Transform::Init(const int& x, const int& y, const int& z)
{
	translation[0] = x;
	translation[1] = y;
	translation[2] = z;
}

bool Transform::LoadTransformation(Geometry* mesh)
{
	bool ret = false;

	float new_position[3] = { translation[0], translation[1] ,translation[2] };
	//change name
	//scale
	if (ImGui::InputFloat3("scale", scale, "%i", ImGuiInputTextFlags_EnterReturnsTrue))
		ret = true;
	if (ImGui::InputFloat3("position", new_position, "%i", ImGuiInputTextFlags_EnterReturnsTrue))
	{
		float translation_x = new_position[0] - translation[0];
		float translation_y = new_position[1] - translation[1];
		float translation_z = new_position[2] - translation[2];
		//ChangePosition(mesh, translation_x, translation_y, translation_z);
		for (uint i = 0; i < 3; ++i)
		{
			translation[i] = new_position[i];
		}
		ret = true;
	}
	ImGui::TextWrapped("Rotation");
	ImGui::Separator();
	if (ImGui::SliderInt("Radiant", &rad, 0, 360))
		ret = true;

	static int item_current = 0;
	const char* items[] = { "X", "Y", "Z" };
	ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
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
	if (mesh != nullptr)
	{
		if (parent != nullptr)
		{
			for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it)
			{
				Transform* comp = dynamic_cast<Transform*>((*it)->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
				comp->LoadTransformation(mesh);
			}
		}
	}

return ret;
}

void Transform::UnLoadTransformation()
{
	//ChangeScale(mesh, scale[0], scale[1], scale[3]);
	ChangePosition(mesh, translation[0], translation[1], translation[2]);
	//par_shapes_rotate(mesh, rad, axis);

	//if parent have childs apply the transformation in all of them 
	if (parent->children.size() != 0)
	{
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it)
		{
			UnLoadTransformation();
		}
	}

	toDelete = true;
}


void Transform::ChangeScale(Geometry* mesh, float x, float y, float z)
{
	glScalef(x, y, z);
	for (uint i = 0; i < mesh->numVertices; i += 3) {
		mesh->vertices[i] *= x;
		mesh->vertices[i + 1] *= y;
		mesh->vertices[i + 2] *= z;
	}
}


void Transform::ChangePosition(Geometry* mesh, float x, float y, float z)
{
	for (uint i = 0; i < mesh->numVertices * 3; i += 3)
	{
		mesh->vertices[i] += x;
		mesh->vertices[i + 1] += y;
		mesh->vertices[i + 2] += z;

		if (mesh->normals != nullptr)
		{
			mesh->normals[i] += x;
			mesh->normals[i + 1] += y;
			mesh->normals[i + 2] += z;
		}

		if (mesh->faceNormals != nullptr)
		{
			mesh->faceNormals[i] += x;
			mesh->faceNormals[i + 1] += y;
			mesh->faceNormals[i + 2] += z;
		}
	}
}

void Transform::Rotate(Geometry* mesh, float rd, float axs[3])
{
	
	float rdCos = cosf(rd);
	float rdSin = sinf(rd);

	float rMatrix[3][3] =
	{
		(axs[0] * axs[0]) * (1 - rdCos) + rdCos,(axs[0] * axs[1] * (1 - rdCos)) + (axs[2] * rdSin), (axs[2] * axs[0] * (1 - rdCos)) - (axs[1] * rdSin),
		(axs[0] * axs[1] * (1 - rdCos)) - (axs[2] * rdSin),(((axs[1] * axs[1]) * (1 - rdCos)) + rdCos), (axs[1] * axs[2] * (1 - rdCos)) + (axs[0] * rdSin),
		((axs[2] * axs[0] * (1 - rdCos)) + (axs[1] * rdSin)),((axs[1] * axs[2] * (1 - rdCos)) - (axs[0] * rdSin)), (((axs[2] * axs[2]) * (1 - rdCos)) + rdCos),
	};

	
	DoRotation(mesh, rMatrix);

	
	for (uint i = 0; i < 3; ++i)
	{
		for (uint j = 0; j < 3; ++j)
		{
			for (uint k = 0; k < 3; ++k)
			{
				for (uint l = 0; l < 3; ++l)
				{
					R[i][j] += R[k][l] * rMatrix[l][k];
				}
			}
		}
	}
}

void Transform::DoRotation(Geometry* mesh, float  r_matrix[3][3])
{
	for (uint i = 0; i < mesh->numVertices * 3; i += 3)
	{

		float v1 = r_matrix[0][0] * mesh->vertices[i] + r_matrix[1][0] * mesh->vertices[i + 1] + r_matrix[2][0] * mesh->vertices[i + 2];
		float v2 = r_matrix[0][1] * mesh->vertices[i] + r_matrix[1][1] * mesh->vertices[i + 1] + r_matrix[2][1] * mesh->vertices[i + 2];
		float v3 = r_matrix[0][2] * mesh->vertices[i] + r_matrix[1][2] * mesh->vertices[i + 1] + r_matrix[2][2] * mesh->vertices[i + 2];
		mesh->vertices[i] = v1;
		mesh->vertices[i + 1] = v2;
		mesh->vertices[i + 2] = v3;

		if (mesh->normals)
		{
			float n1 = r_matrix[0][0] * mesh->normals[i] + r_matrix[1][0] * mesh->normals[i + 1] + r_matrix[2][0] * mesh->normals[i + 2];
			float n2 = r_matrix[0][1] * mesh->normals[i] + r_matrix[1][1] * mesh->normals[i + 1] + r_matrix[2][1] * mesh->normals[i + 2];
			float n3 = r_matrix[0][2] * mesh->normals[i] + r_matrix[1][2] * mesh->normals[i + 1] + r_matrix[2][2] * mesh->normals[i + 2];
			mesh->normals[0] = n1;
			mesh->normals[1] = n2;
			mesh->normals[2] = n3;
		}
	}
}