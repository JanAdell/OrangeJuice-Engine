#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Component.h"
#include "Globals.h"
#include "Transform.h"
#include "Camera.h"
#include "Image.h"
#include "Geometry.h"
#include "Application.h"
#include "ModuleScene.h"

#include "MathGeoLib/MathGeoLib.h"

#include <string>
#include <vector>

class Camera;
struct MouseHit;
struct BoundingBox
{
	AABB aabb;
	OBB obb;
};

class GameObject
{
public:
	GameObject(GameObject* parent = nullptr);
	~GameObject();

	void Update();

	Component* CreateComponent(COMPONENT_TYPE type);
	uint CreateUUID();
	
	void ShowNormalVertex(const bool&);
	void ShowNormalFaces(const bool&);
	void ShowObjectProperties(GameObject*, uint&, uint&);
	void SaveMesh(FILE*);
	void ImportMesh(char*& cursor, char* end_object);

	void Select();
	bool IsSelected();

	Component* GetComponent(COMPONENT_TYPE type);
	void GetProperties();
	Transform* GetTransform();
	virtual void GetHierarchy();
	uint GetUUID();
	uint GetParentUUID();

	void CreateBBOX();
	AABB GetBBOX();

	void Draw();
	void DrawBBox(const AABB& bbox) const;
	void RecalculateBBox();

	void TransformBBox(math::float4x4 matrix);

	void ChangeName(std::string name);

	Component* CreateCamera(float z_near, float z_far);

	void LookForRayCollision(LineSegment raySegment, std::vector<MouseHit>& hit);
	void LookForMeshCollision(LineSegment raySegment, std::vector<MouseHit>& hit);

public:
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	// all the components from this game object
	Transform* transform;
	Camera* cam;
	Geometry* mesh;
	Image* material;

	bool toDelete = false;
	bool isEnable = true;
	bool isSelected = false;
	bool isStatic = false;

	bool showInspectorWindow = false;
	bool showVertexNormals = false;
	bool showNormals = false;

	uint num_vertices;

	uint UUID;
	uint parentUUID;

	BoundingBox* bbox = nullptr;
};

#endif 
