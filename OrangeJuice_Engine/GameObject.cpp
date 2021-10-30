#include "GameObject.h"
#include "Image.h"
#include "Geometry.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Transform.h"

GameObject::GameObject(GameObject* parent) : parent(parent)
{
	if (parent == nullptr) name = "GameObject " + std::to_string(App->scene->gameObjects.size() + 1);
	else name = "GameObject " + std::to_string(App->scene->gameObjects.size() + 1) + "." + std::to_string(parent->children.size() + 1);
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
	if (children.empty())
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			if ((*it)->toDelete)
			{
				components.erase(it);
				break;
			}

			else if ((*it)->isEnable)
			{
				(*it)->Update();

			}
		}
	}
	else
	{
		for (std::vector<GameObject*>::iterator iter = children.begin(); iter != children.end(); ++iter)
			(*iter)->Update();
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
	static int selection_mask = (1 << 0);
	static int node_clicked = 0;

	for (uint i = 0; i < children.size(); ++i)
	{
		GameObject* game_object = children[i];

		//start to show inspector
			// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (selection_mask & (1 << i))
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, game_object->name.c_str());
		if (ImGui::IsItemClicked())
		{
			selection_mask = (1 << i);
			//al show inspector windows = false
			std::vector<GameObject*>::iterator iterator = children.begin();
			while (iterator != children.end())
			{
				if (*iterator != game_object)
					(*iterator)->showInspectorWindow = false;
				++iterator;
			}
			//parent inspector = false
			if (showInspectorWindow)
				showInspectorWindow = false;

			//show inspector
			game_object->showInspectorWindow = true;
		}
		//finish show inspector


		if (node_open)
		{
			if (game_object->children.size() != 0)
			{
				game_object->GetHierarcy();
			}
			ImGui::TreePop();
		}

		if (game_object->showInspectorWindow)
		{
			game_object->GetPropierties();
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
			//this was a test, leaving it here because we may need similar when we make selectable nodes
			char a[100] = "";
			memcpy(a, name.c_str(), name.size());
			if (ImGui::InputText("", a, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				name.assign(a);
			}
		}

		if (ImGui::Checkbox("show vertices normals", &showVertexNormals))
		{
			(&showVertexNormals) ? true : false;

			ShowNormalVertex(showVertexNormals);
		}

		ImGui::SameLine();

		if (ImGui::Checkbox("show faces normals", &showVertexNormals))
		{
			(&showVertexNormals) ? true : false;

			ShowNormalFaces(showNormals);
		}

		Component* mesh = nullptr;
		std::vector<Component*>::iterator it = components.begin();
		int id = 0;
		while (it != components.end())
		{
			if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
			{
				mesh = *it;
				mesh->ShowProperties();
				break;
			}
			++it;
		}

		Component* tex = nullptr;
		std::vector<Component*>::iterator it2 = components.begin();
		while (it2 != components.end())
		{
			if ((*it2)->type == COMPONENT_TYPE::COMPONENT_MATERIAL)
			{
				tex = *it2;
				break;
			}
			++it2;
		}

		if (tex != nullptr)
			id = tex->GetTextureId();

		if (id != 0)
		{
			ImVec2 size = { 200,200 };
			ImGui::Image((ImTextureID)id, size);
			ImGui::TextColored(ImVec4(0, 0, 255, 255), "%i x %i", (int)size.x, (int)size.y);
		}

		ImGui::End();
	}
}

void GameObject::ShowNormalVertex(const bool& x)
{
	for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
	{
		(*iter)->showVertexNormals = x;
	}
}

void GameObject::ShowNormalFaces(const bool& x)
{
	for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
	{
		(*iter)->showNormals = x;
	}

}