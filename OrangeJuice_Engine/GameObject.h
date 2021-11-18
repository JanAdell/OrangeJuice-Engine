#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <string>
#include <vector>
#include "Component.h"
#include "Globals.h"
#include "Transform.h"

class Geometry;

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

	Component* GetComponent(COMPONENT_TYPE type);
	void GetPropierties();
	Transform* GetTransform();
	virtual void GetHierarchy();
	uint GetUUID();
	uint GetParentUUID();

	void ChangeName(std::string name);

public:
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;
	Transform* transform;

	bool toDelete = false;
	bool isEnable = true;

	bool showInspectorWindow = false;
	bool showVertexNormals = false;
	bool showNormals = false;

private:

	uint UUID;
	uint parentUUID;
};

#endif 
