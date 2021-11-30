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

	void Select();
	bool IsSelected();

	Component* GetComponent(COMPONENT_TYPE type);
	void GetPropierties();
	Transform* GetTransform();
	virtual void GetHierarchy();
	uint GetUUID();
	uint GetParentUUID();

	void Draw();
	void DrawBBox(const AABB& bbox) const;
	void RecalculateBBox();

	void ChangeName(std::string name);

public:
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	// all the components from this game object
	Transform* transform = nullptr;
	Camera* cam = nullptr;
	Geometry* mesh;

	bool toDelete = false;
	bool isEnable = true;
	bool isSelected = false;

	bool showInspectorWindow = false;
	bool showVertexNormals = false;
	bool showNormals = false;

	uint num_vertices;

	uint UUID;
	uint parentUUID;

	AABB bbox;
	OBB obb;	
};

#endif 
