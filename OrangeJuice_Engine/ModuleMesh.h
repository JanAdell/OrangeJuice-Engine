#ifndef _MODULEMESH_H_
#define _MODULEMESH_H_

#include "Globals.h"
#include "Geometry.h"
#include "Module.h"
#include "GameObject.h"
#include "Camera.h"

#include "glew/include/GL/glew.h"
#include <gl/GL.h>

#include <vector>
#include <string>
#include <queue>

struct aiNode;

class ModuleMesh : public Module
{
public:
	ModuleMesh(Application* app, bool start_enabled = true);
	~ModuleMesh();

	bool Init();
	update_status PostUpdate(float dt)override;
	bool CleanUp();

	bool LoadFile(const char* fileName);
	bool LoadFBXFile(const char* file_name);
	void LoadObjects(aiNode* node, const aiScene* scene, const char*& file_name, GameObject*& parent);
	void ChangeTex(GameObject* object, const char* file_name, Image* texture);
	std::string LoadData(aiMesh* mesh);
	void LoadMeshFromFormat(const char* file_name, GameObject* g_object);
		
	void LoadTemporalMaterials(const aiScene* scene);
	GLuint LoadImages(const char* p_tex, bool loading_scene = false);
	void LoadTextureFromFormat(const char*, GameObject* g_object);
	void LoadMaterials(const aiScene* scene, GameObject* g_object, const char* file_name, int last_mat_ind);
	void ImportTextureToDDSFile(const char* file_name) const;

	GLuint GetID();
	std::string RandomName(aiMesh* mesh);

	void DrawRay();

	bool IsCulling(Geometry* g);
	bool CheckInAABB(const AABB& bbox);

	std::string GenerateNameFromPath(std::string path);

	bool SaveCurrentScene(const char* s_name);
	uint SaveGameObjects(std::list<std::pair<char*, uint>>& buffer, GameObject* g_object);
	GameObject* LoadSceneFromFormat(const char* s_name);
	GameObject* LoadObjectFromFormat(char*& cursor);
		

private:
	float TriangleCenterAxis(const float& p1, const float& p2, const float& p3);
	
	std::string modelName;

public:
	std::queue<math::AABB*> bbox; 
	std::queue<math::Frustum*> cFrustums; 
	Camera* currentCamera = nullptr;
	std::vector<std::pair<aiMaterial*, int>> tmpMaterial;
	std::vector<std::pair <std::string, int>> tmpTextures; 
	std::vector<uint> tmpParentIds; 
	int randId = 0;
};

#endif //_MODULEMESH_H_ 