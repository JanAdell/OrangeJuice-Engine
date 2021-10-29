#include "GameObject.h"
#include "Image.h"
#include "Geometry.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Transform.h"

GameObject::GameObject()
{
	name = "GameObject " + std::to_string(App->scene->gameObjects.size());
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	components.clear();

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	children.clear();
}

void GameObject::Update()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->toDelete)
		{
			components.erase(it);
			break;
		}
		else if ((*it)->isEnable)
			(*it)->Update();
	}
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* component;
	switch (type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		component = new Transform(this);
		components.push_back(component);
		break;
	case COMPONENT_TYPE::COMPONENT_MESH:
		component = new Geometry(this);
		components.push_back(component);
		break;
	case COMPONENT_TYPE::COMPONENT_MATERIAL:
		component = new Image(this);
		components.push_back(component);
		break;
	case COMPONENT_TYPE::NO_COMPONENT:
		break;
	default:
		break;
	}
	return component;
}

void GameObject::GetHierarcy()
{
	//if (showInspectorWindow)
	//	GetPropierties();

	for (uint i = 0; i < children.size(); ++i)
	{
		GameObject* game_object = children[i];

		if (game_object->children.size() != 0)
		{
			if (ImGui::TreeNodeEx(game_object->name.c_str()))
			{
				game_object->GetHierarcy();
				ImGui::TreePop();
			}
		}
		else
		{
			if (ImGui::TreeNodeEx(game_object->name.c_str()))
			{
				ImGui::TreePop();
			}
		}
	}
}

void GameObject::GetPropierties()
{
	if (ImGui::Begin("Inspector", &showInspectorWindow))
	{
		if (ImGui::CollapsingHeader("Properties"))
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Delete"))
					toDelete = true;

				ImGui::EndMenu();
			}
			if (ImGui::Checkbox("Active", &isEnable))
				(&isEnable) ? true : false;

			ImGui::SameLine();
			char* str0;
			str0 = PAR_MALLOC(char, strlen(name.c_str()) + 1);
			strcpy(str0, name.c_str());
			ImGuiInputTextCallback size = (ImGuiInputTextCallback)name.size();
			WM_KEYDOWN / WM_KEYUP / WM_CHAR;
			ImGui::GetIO().WantCaptureKeyboard = true;
			ImGui::GetIO().WantTextInput = true;
			if (ImGui::InputText(" ", str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_CallbackResize, size, &str0))
			{
				name.assign(str0);
			}
		}
		Component* mesh = nullptr;
		std::vector<Component*>::iterator it = components.begin();
		while (it != components.end())
		{
			if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
			{
				mesh = *it;
				break;
			}
			if (mesh != nullptr)
				mesh->ShowProperties();
		}
		ImGui::End();
	}
}