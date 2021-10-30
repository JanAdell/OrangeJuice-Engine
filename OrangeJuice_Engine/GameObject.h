#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <string>
#include <vector>
#include "Component.h"

class GameObject
{
public:
	GameObject(GameObject* parent = nullptr);
	~GameObject();

	void Update();

	Component* CreateComponent(COMPONENT_TYPE type);
	virtual void GetHierarcy();
	void GetPropierties();
	void ShowNormalVertex(const bool&);
	void ShowNormalFaces(const bool&);

public:
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;
	bool showInspectorWindow = false;
	bool toDelete = false;
	bool isEnable = true;

	bool showVertexNormals = false;
	bool showNormals = false;
};

#endif 
