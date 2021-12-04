#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Component.h"
#include "Globals.h"
#include "glew/include/GL/glew.h"
#include "par_shapes.h"

#include <gl/GL.h>
#include <string>
#include <vector>

struct aiScene;
struct aiMesh;
struct aiMaterial;

class Image : public Component
{
public:
	Image(GameObject* parent);
	~Image();

	void Enable() override;
	void Update() override;
	void Disable() override;

	void Save(FILE* file);

	GLuint LoadImage(const char* p_tex);
	void LoadCoords(aiMesh* scene);
	void LoadCoords(par_shapes_mesh* p_mesh);

	void LoadMaterials(const aiScene* scene, std::string file_name, std::vector<std::pair<aiMaterial*, int>>& tmp_mat, int last_mat_ind);
	void LoadBuffers();
	void LoadCheckerTexture();
	void ShowProperties() {};
	std::string GetTexturePath();
	int GetTextureId();
	int SetTextureId(int id);

public:
	uint idCoords = 0;
	uint numCoords = 0;
	float* uvCoord = nullptr;
	int textureId = 0;
	int tmpId = 0;
	uint checkId = 0;
	std::string pTex;
};

#endif
