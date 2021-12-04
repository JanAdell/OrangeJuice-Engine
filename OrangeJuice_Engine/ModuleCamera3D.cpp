#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Geometry.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	camera = new Camera(nullptr);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

bool ModuleCamera3D::Save(nlohmann::json & j) const
{
	return true;
}

bool ModuleCamera3D::Load(nlohmann::json & j)
{
	return true;
}

update_status ModuleCamera3D::PreUpdate(float dt)
{
	bool ret = true;
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !write)
		MousePicking();

	return ret ? UPDATE_CONTINUE : UPDATE_STOP;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	camFrustum = &camera->frustum;

	vec3 newPos(0,0,0);
	float speed = 6.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 15.0f * dt;

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) newPos.y += speed;
	if(App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) newPos.y -= speed;

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	if (App->input->GetMouseZ() > 0)
	{
		newPos -= Z * speed * 10;
	}
	if (App->input->GetMouseZ() < 0)
	{
		newPos += Z * speed * 10;
	}
	
	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();
		float Sensitivity = 0.25f;
		Position -= Reference;
		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;
			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}
		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;
			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);
			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	}

	
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

void ModuleCamera3D::BeInCamera(const AABB& bbox)
{
	vec central = bbox.CenterPoint();
	vec diagonal = bbox.Diagonal();

	Position.x = central.x;
	Position.y = central.y;
	Position.z = central.z + diagonal.Length();

	//Position.y = camera->frustum.pos.y = center.y;
	//Position.x = camera->frustum.pos.x = center.x;
	LookAt({ central.x, central.y, central.z });
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

void ModuleCamera3D::GoAroundGeometry(GameObject* obj)
{
	if (obj == nullptr)
		return;

	std::vector<float3> vertices;
	if (obj->children.empty())
	{

		for (std::vector < Component*>::iterator iter2 = obj->components.begin(); iter2 != obj->components.end(); ++iter2)
		{
			COMPONENT_TYPE type = (*iter2)->type;
			if (type == COMPONENT_TYPE::COMPONENT_MESH)
			{
				//Generate AABBS for each geometry on scene
				math::AABB newAABB(float3(0, 0, 0), float3(0, 0, 0));
				std::vector <float3> vertex_array;

				Geometry* g = dynamic_cast<Geometry*>(*iter2);
				for (int j = 0; j < g->numVertices * 3; j += 3)
				{
					vertex_array.push_back(float3(g->vertices[j], g->vertices[j + 1], g->vertices[j + 2]));
				}

				newAABB.Enclose(&vertex_array[0], g->numVertices);

				//Stores the 8 vertices of the box in a general array
				for (int j = 0; j < 8; j++)
				{
					vertices.push_back(newAABB.CornerPoint(j));
				}
			}

		}
	}
	else
	{
		for (std::vector<GameObject*>::iterator iter = obj->children.begin(); iter < obj->children.end(); ++iter)
		{
			for (std::vector < Component*>::iterator iter2 = (*iter)->components.begin(); iter2 != (*iter)->components.end(); ++iter2)
			{
				COMPONENT_TYPE type = (*iter2)->type;
				if (type == COMPONENT_TYPE::COMPONENT_MESH)
				{
					//Generate AABBS for each geom in scene
					math::AABB newAABB(float3(0, 0, 0), float3(0, 0, 0));
					std::vector <float3> vertex_array;

					Geometry* g = dynamic_cast<Geometry*>(*iter2);
					for (int j = 0; j < g->numVertices * 3; j += 3)
					{
						vertex_array.push_back(float3(g->vertices[j], g->vertices[j + 1], g->vertices[j + 2]));
					}

					newAABB.Enclose(&vertex_array[0], g->numVertices);

					//Stores the 8 vertices of the box in a general array
					for (int j = 0; j < 8; j++)
					{
						vertices.push_back(newAABB.CornerPoint(j));
					}
				}
			}
		}
	}
	//Creates a general AABB 
	math::AABB general(float3(0, 0, 0), float3(0, 0, 0));
	general.Enclose(&vertices[0], vertices.size());

	Position.x = general.maxPoint.x * 1.5;
	Position.y = general.maxPoint.y * 1.5;
	Position.z = general.maxPoint.z * 1.5;

	Reference.x = general.CenterPoint().x;
	Reference.y = general.CenterPoint().y;
	Reference.z = general.CenterPoint().z;

	LookAt(Reference);
}

std::vector<float3> ModuleCamera3D::AABBVertex(GameObject* obj, std::vector<float3> vertices)
{
	for (std::vector < Component*>::iterator iter2 = obj->components.begin(); iter2 != obj->components.end(); ++iter2)
	{
		COMPONENT_TYPE type = (*iter2)->type;
		if (type == COMPONENT_TYPE::COMPONENT_MESH)
		{
			
			math::AABB newAABB(float3(0, 0, 0), float3(0, 0, 0));
			std::vector <float3> vertexArray;

			Geometry* g = dynamic_cast<Geometry*>(*iter2);
			for (int j = 0; j < g->numVertices * 3; j += 3)
			{
				vertexArray.push_back(float3(g->vertices[j], g->vertices[j + 1], g->vertices[j + 2]));
			}

			newAABB.Enclose(&vertexArray[0], g->numVertices);

			for (int j = 0; j < 8; j++)
			{
				vertices.push_back(newAABB.CornerPoint(j));
			}
		}
	}
	return vertices;
}

void ModuleCamera3D::MousePicking()
{
	//normalize mouse pos
	int width;
	int height;
	SDL_GetWindowSize(App->window->window, &width, &height);
	float2 mouseNormal;
	mouseNormal.x = -1.0 + 2.0 * App->input->GetMouseX() / width;
	mouseNormal.y = 1.0 - 2.0 * App->input->GetMouseY() / height;

	// Create the list of distances and objects that we will use 
	// to select the closest gameobject that was hit
	std::vector<MouseHit> hit;

	ray = camFrustum->UnProjectLineSegment(mouseNormal.x, mouseNormal.y);

	float rayDir = ray.Length();
	GameObject* pickedObj = nullptr;

	std::vector<GameObject*>::iterator iter = App->scene->gameObjects.begin();
	for (; iter != App->scene->gameObjects.end(); ++iter)
	{
		(*iter)->LookForRayCollision(pickedObj, ray, rayDir, hit);
	}

	if (!hit.empty())
	{
		std::sort(hit.begin(), hit.end(), LesThanKey());
		std::vector<MouseHit>::iterator it = hit.begin();
		pickedObj = (*it).object;
		if (pickedObj)
		{
			App->scene->gameObjectSelect = pickedObj;
			App->scene->gameObjectSelect->showInspectorWindow = true;
		}
	}

}