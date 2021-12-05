#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"
#include "par_shapes.h"
#include "MathGeoLib/MathGeoLib.h"

class Geometry;

class Transform :public Component
{

public:

	Transform(GameObject* parent);
	~Transform();

	void Enable();
	void Update();
	void Disable();

	void Init(const int& x, const int& y, const int& z);

	bool LoadTransformation(Geometry* p_mesh);
	void UnLoadTransformation();

	void ShowProperties() {};

	void ChangeScale(Geometry* mesh, float x, float y, float z);
	void ChangePosition(Geometry* mesh, float x, float y, float z);

	void Rotate(Geometry* mesh, float rd, float axs[3]);
	void DoRotation(Geometry* mesh, float  r_matrix[3][3]);

	void RotateObjects(GameObject* objectToRotate);

	void SetTranslation(float3 position);
	void SetScale(float3 scale);
	void SetRotation(float3 rot);
	void SetQuatRotation(Quat quatRot);

	float3 GetTranslation();
	float3 GetScale();
	float3 GetEulerRotation();
	Quat GetQuatRotation();

	void CalculateMatrix();

	int GetTextureId() { return 0; };
	std::string GetTexturePath() { return std::string(); };
	
public:

	float3 translation;
	float3 scale;
	float3 eulerAng;
	float3 pos;

	float3 scl;
	int rad;
	float3 axis;

	Geometry* mesh;

	float4x4 localMatrix;
	float4x4 globalMatrix;
	float4x4 rotMat;

	Quat quatRotation;

	float R[3][3];

	bool isChanged;
};

#endif