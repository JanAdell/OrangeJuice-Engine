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
	bool LoadTextureFile(const char* file_name);
	void ChangeTex(GameObject* object, const char* file_name);
	bool IsCulling(Geometry* g);

	bool CheckInAABB(const AABB& bbox);

	std::string GenerateNameFromPath(std::string path);
		

private:
	float TriangleCenterAxis(const float& p1, const float& p2, const float& p3);
	
	std::string modelName;

public:
	std::queue<math::AABB*> bbox; 
	std::queue<math::Frustum*> cFrustums; 
	Camera* currentCamera = nullptr;
};

#endif //_MODULEMESH_H_ 