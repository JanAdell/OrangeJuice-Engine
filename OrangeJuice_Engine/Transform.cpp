//#include "Application.h"
#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
//#include "ModuleScene.h"
#include "Geometry.h"
#include "par_shapes.h"
#include <vector>
#include "ImGui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Assimp/include/scene.h"
#include "../OrangeJuice_Engine/MathGeoLib/MathGeoLib.h"

Transform::Transform(GameObject* parent) :Component(parent, COMPONENT_TYPE::COMPONENT_TRANSFORM)
{
	//translation = { 0.0f,0.0f,0.0f };
	scl = { scale[0], scale[1], scale[2] };
	rad = 0;
	axis = { 0.0f,0.0f,0.0f };

	mesh = nullptr;

	localMatrix = float4x4::identity;
	globalMatrix = float4x4::identity;
	rotMat = float4x4::identity;
	quatRotation = Quat::identity;

	scale = float3::one;
	eulerAng = float3::zero;
	pos = float3::zero;

	float R[3][3] =
	{ 1, 0, 0,
	0, 1, 0,
	0, 0, 1 };

	isChanged = false;
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
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

float3 Transform::GetTranslation()
{
	return pos;
}

float3 Transform::GetScale()
{
	return scl;
}

void Transform::CalculateMatrix()
{
	localMatrix.Set(float4x4::FromTRS(pos, quatRotation, scl));
}

bool Transform::LoadTransformation(Geometry* mesh)
{
	bool ret = false;

	float newPos[3] = { pos[0], pos[1] ,pos[2] };

	math::float3 current_pos = pos;
	math::float3 current_angles = eulerAng;	//scale

	if (ImGui::InputFloat3("scale", (float*)&scale, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		rotMat = math::float4x4::FromTRS(pos.zero, quatRotation.identity, scale);
		ret = true;
	}
	//position
	if (ImGui::InputFloat3("position", (float*)&pos, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		rotMat = math::float4x4::FromTRS(pos - current_pos, quatRotation.identity, scale.one);
		ret = true;
	}
	//rotation
	if (ImGui::InputFloat3("rotation", (float*)&eulerAng, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		quatRotation = math::Quat::FromEulerXYZ(math::DegToRad(eulerAng - current_angles).x, math::DegToRad(eulerAng - current_angles).y, math::DegToRad(eulerAng - current_angles).z);
		rotMat = math::float4x4::FromTRS(pos.zero, quatRotation, scale.one);
		ret = true;
	}

	/*static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		mCurrentGizmoOperation = ImGuizmo::SCALE;*/

	ImGuiIO& io = ImGui::GetIO();
	//ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	float4x4 view_matrix = App->camera->camera->frustum.ViewMatrix();
	float4x4 proj_matrix = App->camera->camera->frustum.ProjectionMatrix();
	view_matrix.Transpose();
	proj_matrix.Transpose();
	float4x4 trs_matrix = globalMatrix;
	//ImGuizmo::Manipulate(view_matrix.ptr(), proj_matrix.ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, trs_matrix.ptr(), NULL, NULL);

	/*if (ImGuizmo::IsUsing())
	{
		trs_matrix.Transpose();
		float3 new_pos;
		float3 new_scale;
		Quat new_q;
		trs_matrix.Decompose(new_pos, new_q, new_scale);
		new_pos.Normalize();

		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
			rotMat = math::float4x4::FromTRS(new_pos * 0.5, quatRotation.identity, scale.one);
		if (mCurrentGizmoOperation == ImGuizmo::SCALE)
			rotMat = math::float4x4::FromTRS(float3::zero, quatRotation.identity, new_scale);
		if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
		{
			rotMat = math::float4x4::FromQuat(new_q.Conjugated());
		}

		ret = true;
	}*/

	if (ret)
	{
		RotateObjects(parent);
		App->scene->octree->Remove(parent);
		App->scene->octree->Insert(parent);	
	}

return ret;
}

void Transform::UnLoadTransformation()
{
	//ChangeScale(mesh, scale[0], scale[1], scale[3]);
	ChangePosition(mesh, pos.x, pos.y, pos.z);
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

void Transform::SetTranslation(float3 position)
{
	pos = position;
	CalculateMatrix();
}

void Transform::SetScale(float3 scale)
{
	scl = scale;
	CalculateMatrix();
}

void Transform::SetRotation(float3 rot)
{
	axis = rot;
	this->quatRotation = Quat::FromEulerXYZ(axis.x * DEGTORAD, axis.y * DEGTORAD, axis.z * DEGTORAD);
	CalculateMatrix();
}

void Transform::SetQuatRotation(Quat quatRot)
{
	this->quatRotation = quatRotation;
	axis = this->quatRotation.ToEulerXYZ() * RADTODEG;
	CalculateMatrix();
}

float3 Transform::GetEulerRotation()
{
	return axis;
}

Quat Transform::GetQuatRotation()
{
	return quatRotation;
}

void Transform::RotateObjects(GameObject* objectToRotate)
{
	for (std::vector<Component*>::iterator component_iterator = objectToRotate->components.begin(); component_iterator != objectToRotate->components.end(); ++component_iterator)
	{
		if ((*component_iterator)->type == COMPONENT_TYPE::COMPONENT_TRANSFORM)
		{
			Transform* mesh = dynamic_cast<Transform*>(*component_iterator);
			mesh->globalMatrix = mesh->globalMatrix * globalMatrix;

		}
	}
	if (objectToRotate->children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = objectToRotate->children.begin(); it != objectToRotate->children.end(); ++it)
		{
			RotateObjects(*it);
		}
	}
	objectToRotate->TransformBBox(globalMatrix);
}
