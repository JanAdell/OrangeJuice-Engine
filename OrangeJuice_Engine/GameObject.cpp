#include "GameObject.h"
#include "Camera.h"
#include "Component.h"
#include "ModuleRenderer3D.h"

GameObject::GameObject(GameObject* parent) : parent(parent)
{
	if (name.empty())
	{
		if (parent == nullptr) name = "GameObject " + std::to_string(App->scene->gameObjects.size() + 1);
		else name = "GameObject " + std::to_string(App->scene->gameObjects.size() + 1) + "." + std::to_string(parent->children.size() + 1);
	}

	transform = nullptr;
	cam = nullptr;
	mesh = nullptr;
	material = nullptr;

	bbox = AABB({ 0,0,0 }, { 0, 0, 0 });

	parentUUID = 0;
	UUID = CreateUUID();

	isStatic = true;
	if (isStatic) 
	{ 
		App->scene->octree->Insert(this); 
	}
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Disable();
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	components.clear();

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if ((*it) != nullptr)
		{
			App->scene->octree->Remove(*it);
			delete (*it);
		}
		(*it) = nullptr;
	}
	children.clear();
}

void GameObject::Update()
{
	Draw();

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->toDelete)
		{
			delete(*it);
			(*it) = nullptr;
			it = components.erase(it);
			break;
		}
		else if ((*it)->isEnable)
		{
			(*it)->Update();
		}
		else if ((*it)->GetComponentType() == COMPONENT_TYPE::COMPONENT_MESH)
		{
			ModuleMesh* mesh = (ModuleMesh*)*it;

			float3 points[8];
			//bbox.GetCornerPoints(points);
			App->renderer3D->DrawBoundingBox(points);
		}
	}

	if (!children.empty())
	{
		for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			if ((*it)->toDelete)
			{
				if (*it == App->scene->gameObjectSelect)
					App->scene->gameObjectSelect = nullptr;
				App->scene->octree->Remove(*it);
				delete(*it);
				(*it) = nullptr;
				it = children.erase(it);
				break;
			}

			else if ((*it)->isEnable)
				(*it)->Update();
		}
	}
}

void GameObject::CleanUp()
{
	//Delete components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); it++)
	{
		delete (*it);
	}

	//Delete children
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		(*it)->CleanUp();
		delete (*it);
	}
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* component;
	switch (type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		component = new Transform(this);
		transform = (Transform*)component;
		components.push_back(component);
		break;
	case COMPONENT_TYPE::COMPONENT_MESH:
		component = new Geometry(this);
		mesh = (Geometry*)component;
		components.push_back(component);
		break;
	case COMPONENT_TYPE::COMPONENT_MATERIAL:
		component = new Image(this);
		material = (Image*)component;
		components.push_back(component);
		break;
	case COMPONENT_TYPE::COMPONENT_CAMERA:
		component = new Camera(this);
		cam = (Camera*)component;
		components.push_back(component);
		break;
	case COMPONENT_TYPE::NO_COMPONENT:
		break;
	default:
		break;
	}
	return component;
}

uint GameObject::CreateUUID()
{
	return LCG().Int();
}

void GameObject::ShowNormalVertex(const bool& x)
{
	for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
		(*iter)->showVertexNormals = x;
}

void GameObject::ShowNormalFaces(const bool& x)
{
	for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
		(*iter)->showNormals = x;
}

void GameObject::ShowObjectProperties(GameObject* object, uint& ntriangles, uint& nvertices)
{
	for (std::vector<Component*>::iterator it = object->components.begin(); it < object->components.end(); ++it)
	{
		if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
		{
			ntriangles += dynamic_cast<Geometry*>(*it)->numIndices / 3;
			nvertices += dynamic_cast<Geometry*>(*it)->numVertices;
		}
	}
	if (!children.empty())
	{
		for (std::vector<GameObject*>::iterator iter = object->children.begin(); iter < object->children.end(); ++iter)
			ShowObjectProperties(*iter, ntriangles, nvertices);
	}
}

Component* GameObject::GetComponent(COMPONENT_TYPE type)
{
	for (std::vector<Component*>::iterator iter = components.begin(); iter < components.end(); ++iter)
	{
		if ((*iter)->type == type)
			return (*iter);
	}
	return nullptr;
}

void GameObject::GetProperties()
{
	if (ImGui::Begin("Inspector", &showInspectorWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::CollapsingHeader("Properties"))
		{
			if (ImGui::Button("Delete")) toDelete = true;

			if (ImGui::Checkbox("Active", &isEnable)) (&isEnable) ? true : false;

			if (ImGui::Checkbox("Static", &isStatic))
			{
				(&isStatic) ? true : false;
				if (isStatic) App->scene->octree->Insert(this);
				else App->scene->octree->Remove(this);
			}

			
			//this was a test, leaving it here because we may need similar something when we make selectable nodes
			char a[100] = "";
			memcpy(a, name.c_str(), name.size());
			if (bool write = ImGui::InputText("", a, 100, ImGuiInputTextFlags_EnterReturnsTrue)) name.assign(a);
			
			if (ImGui::IsItemActive()) App->camera->write = true;
			else App->camera->write = false;
			
			ImGui::NewLine();
			ImGui::Text("UUID: %i", UUID);
			ImGui::NewLine();
		}

		if (ImGui::Checkbox("Show vertices normals", &showVertexNormals))
		{
			(&showVertexNormals) ? true : false;
			ShowNormalVertex(showVertexNormals);
		}

		ImGui::SameLine();

		if (ImGui::Checkbox("Show faces normals", &showNormals))
		{
			(&showNormals) ? true : false;
			ShowNormalFaces(showNormals);
		}

		std::vector<Component*>::iterator it = components.begin();
		int id = 0;
		while (it != components.end())
		{
			if ((*it)->type == COMPONENT_TYPE::COMPONENT_TRANSFORM)
			{
				dynamic_cast<Transform*>(*it)->LoadTransformation();
				break;
			}
			++it;
		}

		
		if (ImGui::CollapsingHeader("Information"))
		{
			uint num_vertices = 0;
			uint num_triangles = 0;

			ShowProperties(this, num_triangles, num_vertices);

			ImGui::Text("triangles: %u", num_triangles);
			ImGui::Text("vertices: %u", num_vertices);

			if (ImGui::Checkbox("Bounding Box", &App->renderer3D->showBBox))
			{
				(&App->renderer3D->showBBox) ? true : false;
			}
		}

		if (cam != nullptr)
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::DragFloat("Near Plane", &cam->nearPlane, 0.05f, 0.f, 0.f, "%.2f");
				ImGui::DragFloat("Far Plane", &cam->farPlane, 0.05f, 0.f, 0.f, "%.2f");
				cam->frustum.SetViewPlaneDistances(cam->nearPlane, cam->farPlane);

				ImGui::DragFloat("Horizontal FOV", &cam->horizontalFov, 0.05f, 0.f, 0.f, "%.2f");
				ImGui::DragFloat("Vertical FOV", &cam->verticalFov, 0.01f, 0.0f, 60.0f);
				cam->frustum.SetPerspective(cam->horizontalFov, cam->verticalFov);

				ImGui::Spacing();
			}
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

		if (tex != nullptr) id = tex->GetTextureId();

		if (id != 0)
		{
			if (ImGui::CollapsingHeader("Material"))
			{
				if (ImGui::Checkbox("Checker Texture", &tex->showChecker))
				{
					Image* img = dynamic_cast<Image*>(tex);
					if (tex->showChecker)
					{
						img->SetTextureId(img->checkId);
					}
					else
					{
						img->SetTextureId(img->tmpId);
					}
				}

				ImGui::Checkbox("Show", &tex->show);
				ImVec2 size = { 200,200 };
				ImGui::Image((ImTextureID)id, size);
				ImGui::TextColored(ImVec4(255, 255, 0, 255), " Size: %i x %i", tex->texDimension[0], tex->texDimension[1]);
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "Path: %s", tex->GetTexturePath().c_str());
			}
		}

		ImGui::End();
	}
}

Transform* GameObject::GetTransform()
{
	return transform;
}

void GameObject::GetHierarchy()
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
			App->scene->gameObjectSelect = game_object;
			selection_mask = (1 << i);
			//al show inspector windows = false
			std::vector<GameObject*>::iterator iterator = children.begin();
			while (iterator != children.end())
			{
				if (*iterator != App->scene->gameObjectSelect)
					(*iterator)->showInspectorWindow = false;
				++iterator;
			}
			//parent inspector = false
			if (showInspectorWindow)
				showInspectorWindow = false;

			//show inspector
			App->scene->gameObjectSelect->showInspectorWindow = true;
		}
		//finish show inspector

		if (node_open)
		{
			if (game_object->children.size() != 0)
			{
				game_object->GetHierarchy();
			}
			ImGui::TreePop();
		}

		if (game_object != nullptr)
		{
			if (game_object->showInspectorWindow)
			{
				App->scene->gameObjectSelect = game_object;
			}
		}

	}
}

uint GameObject::GetUUID()
{
	return UUID;
}

uint GameObject::GetParentUUID() {
	return parent->GetUUID();
}

void GameObject::CreateBBOX()
{
	bbox.SetNegativeInfinity();
	bbox.Enclose((float3*)mesh->vertices, mesh->numVertices);
}

AABB GameObject::GetBBOX()
{
	return bbox;
}

void GameObject::Draw()
{
	Geometry* mesh = dynamic_cast<Geometry*>(GetComponent(COMPONENT_TYPE::COMPONENT_MESH));
	if (mesh != nullptr)
		mesh->DrawMesh();

	for (std::vector<GameObject*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		(*iter)->Draw();

	if (App->renderer3D->showBBox)
	{
		DrawBBox(bbox);
	}

	if (transform != nullptr && transform->isChanged)
	{
		GameObject* bigParent = this;

		while (bigParent->parent != nullptr)
			bigParent = bigParent->parent;

		bigParent->RecalculateBBox();

		transform->isChanged = false;
	}

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->Draw();
	}
}

void GameObject::DrawBBox(const AABB& bbox) const
{
	glLineWidth(1.5f);
	glColor3f(1, 1, 0);

	glBegin(GL_LINES);

	glVertex3f(bbox.CornerPoint(0).x, bbox.CornerPoint(0).y, bbox.CornerPoint(0).z);
	glVertex3f(bbox.CornerPoint(1).x, bbox.CornerPoint(1).y, bbox.CornerPoint(1).z);

	glVertex3f(bbox.CornerPoint(0).x, bbox.CornerPoint(0).y, bbox.CornerPoint(0).z);
	glVertex3f(bbox.CornerPoint(2).x, bbox.CornerPoint(2).y, bbox.CornerPoint(2).z);

	glVertex3f(bbox.CornerPoint(0).x, bbox.CornerPoint(0).y, bbox.CornerPoint(0).z);
	glVertex3f(bbox.CornerPoint(4).x, bbox.CornerPoint(4).y, bbox.CornerPoint(4).z);

	glVertex3f(bbox.CornerPoint(3).x, bbox.CornerPoint(3).y, bbox.CornerPoint(3).z);
	glVertex3f(bbox.CornerPoint(1).x, bbox.CornerPoint(1).y, bbox.CornerPoint(1).z);

	glVertex3f(bbox.CornerPoint(3).x, bbox.CornerPoint(3).y, bbox.CornerPoint(3).z);
	glVertex3f(bbox.CornerPoint(2).x, bbox.CornerPoint(2).y, bbox.CornerPoint(2).z);

	glVertex3f(bbox.CornerPoint(3).x, bbox.CornerPoint(3).y, bbox.CornerPoint(3).z);
	glVertex3f(bbox.CornerPoint(7).x, bbox.CornerPoint(7).y, bbox.CornerPoint(7).z);

	glVertex3f(bbox.CornerPoint(6).x, bbox.CornerPoint(6).y, bbox.CornerPoint(6).z);
	glVertex3f(bbox.CornerPoint(2).x, bbox.CornerPoint(2).y, bbox.CornerPoint(2).z);

	glVertex3f(bbox.CornerPoint(6).x, bbox.CornerPoint(6).y, bbox.CornerPoint(6).z);
	glVertex3f(bbox.CornerPoint(4).x, bbox.CornerPoint(4).y, bbox.CornerPoint(4).z);

	glVertex3f(bbox.CornerPoint(6).x, bbox.CornerPoint(6).y, bbox.CornerPoint(6).z);
	glVertex3f(bbox.CornerPoint(7).x, bbox.CornerPoint(7).y, bbox.CornerPoint(7).z);

	glVertex3f(bbox.CornerPoint(5).x, bbox.CornerPoint(5).y, bbox.CornerPoint(5).z);
	glVertex3f(bbox.CornerPoint(1).x, bbox.CornerPoint(1).y, bbox.CornerPoint(1).z);

	glVertex3f(bbox.CornerPoint(5).x, bbox.CornerPoint(5).y, bbox.CornerPoint(5).z);
	glVertex3f(bbox.CornerPoint(4).x, bbox.CornerPoint(4).y, bbox.CornerPoint(4).z);

	glVertex3f(bbox.CornerPoint(5).x, bbox.CornerPoint(5).y, bbox.CornerPoint(5).z);
	glVertex3f(bbox.CornerPoint(7).x, bbox.CornerPoint(7).y, bbox.CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);
}

void GameObject::RecalculateBBox()
{
	if (transform != nullptr)
	{

		if (children.size() > 0)
		{
			for (std::vector<GameObject*>::iterator it_c = children.begin(); it_c != children.end(); it_c++)
			{
				(*it_c)->RecalculateBBox();
				if ((*it_c)->bbox.IsFinite())
					bbox.Enclose((*it_c)->bbox);
			}
		}

		if (mesh != nullptr)
		{
			bbox.Enclose((float3*)mesh->vertices, mesh->numVertices);
		}

		if (children.size() <= 0)
		{
			bbox.TransformAsAABB(transform->globalMatrix);
		}
	}
}

void GameObject::ChangeName(std::string name)
{
	this->name = name;
}

void GameObject::SaveMesh(FILE* file)
{
	std::fputs("GameObject:\n", file);
	if (parent != nullptr)
		std::fprintf(file, "parentID: %i;\n", parent->UUID);
	else
		std::fprintf(file, "parentID: %i;\n", 0);

	std::fprintf(file, "ID: %i;\n", UUID);

	std::fprintf(file, "name: %s;\n", name.c_str());
	for (std::vector<Component*>::iterator comp = components.begin(); comp != components.end(); ++comp)
	{
		if ((*comp)->type == COMPONENT_TYPE::COMPONENT_MESH)
			dynamic_cast<Geometry*>(*comp)->Save(file);
	}
	
	for (std::vector<Component*>::iterator comp = components.begin(); comp != components.end(); ++comp)
	{
		if ((*comp)->type == COMPONENT_TYPE::COMPONENT_MATERIAL)
			dynamic_cast<Image*>(*comp)->Save(file);
	}
	std::fprintf(file, "//\n");

	if (children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
		{
			(*iter)->SaveMesh(file);
		}
	}
}

void GameObject::Select()
{
	isSelected = true;
}

bool GameObject::IsSelected()
{
	return isSelected;
}

void GameObject::LookForMeshCollision(LineSegment raySegment, MouseHit& hit)
{
	Transform* transform = dynamic_cast<Transform*>(GetComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	Geometry* mesh = dynamic_cast<Geometry*>(GetComponent(COMPONENT_TYPE::COMPONENT_MESH));

	if (mesh == nullptr) return;

	float* vertices = (float*)(mesh)->vertices;
	uint* indices = (uint*)(mesh)->indices;

	LineSegment segLocalized = raySegment;
	float4x4 invMatrix = transform->globalMatrix.Transposed().Inverted();
	segLocalized = invMatrix * segLocalized;

	for (int j = 0; j < (mesh)->numIndices; j+=3)
	{
		Triangle triangle;

		triangle.a.Set(&vertices[indices[j] * 3]);
		triangle.b.Set(&vertices[indices[j+1] * 3]);
		triangle.c.Set(&vertices[indices[j+2] * 3]);

		float tmpDistance;
		if (segLocalized.Intersects(triangle, &tmpDistance, nullptr))
		{
			hit.distance = tmpDistance;
		}
	}
}

GameObject* GameObject::FindChildByID(uint other_id) const
{
	GameObject* ret = nullptr;

	if (UUID == other_id)
	{
		ret = (GameObject*)this;
	}

	else for (int i = 0; i < children.size(); i++)
	{
		ret = children[i]->FindChildByID(other_id);
		if (ret != nullptr)
			break;
	}


	return ret;
}

void GameObject::SetParent(GameObject* new_parent)
{
	if (parent)
	{//Delete this object from the old parent childrens	
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); it++)
		{
			if ((*it) == this)
			{
				parent->children.erase(it);
				break;
			}
		}
	}

	parent = new_parent;
	parent->children.push_back(this);

}

void GameObject::ShowProperties(GameObject* object, uint& ntriangles, uint& nvertices)
{

	for (std::vector<Component*>::iterator it = object->components.begin(); it < object->components.end(); ++it)
	{
		if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
		{
			ntriangles += dynamic_cast<Geometry*>(*it)->numIndices / 3;
			nvertices += dynamic_cast<Geometry*>(*it)->numVertices;
		}
	}

	if (!object->children.empty())
	{
		for (std::vector<GameObject*>::iterator iter = object->children.begin(); iter < object->children.end(); ++iter)
		{
			ShowProperties(*iter, ntriangles, nvertices);
		}
	}
}
