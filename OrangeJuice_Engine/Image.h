#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Component.h"
#include "Globals.h"
#include "glew/include/GL/glew.h"
#include "par_shapes.h"

#include <gl/GL.h>
#include <string>

struct aiScene;
struct aiMesh;

class Image : public Component
{
public:
	Image(GameObject* parent);
	~Image();

	void Enable() override;
	void Update() override;
	void Disable() override;

	GLuint LoadImage(const char* p_tex);
	void LoadCoords(aiMesh* scene);
	void LoadCoords(par_shapes_mesh* p_mesh);

	void LoadMaterials(const aiScene* scene, std::string file_name);
	void LoadBuffers();
	void ShowProperties() {};
	std::string GetTexturePath();
	int GetTextureId();

public:
	uint idCoords = 0;
	uint numCoords = 0;
	float* uvCoord = nullptr;
	int textureId = 0;
	std::string pTex;
};

#endif
