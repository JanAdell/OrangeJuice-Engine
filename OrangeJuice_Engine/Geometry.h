#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"
#include "par_shapes.h"
#include "Image.h"
#include "Transform.h"

enum  class Primitives
{
	CUBE,
	SPHERE,
	CONE,
	CYILINDER,
	PLANE,
	UKNOWN,
};

class Geometry: public Component
{
public:

	Geometry(GameObject* parent = nullptr);
	~Geometry();
	void DebugDraw();

	void Disable() override;
	void CreatePrimitive(par_shapes_mesh* p_mesh, float col0 = 255, float col1 = 255, float col2 = 255, float col3 = 255);
	void DrawPrimitives();

	void Update() override;

	void LoadData(aiMesh* mesh);

	void ShowProperties();
	int GetTextureId() { return 0; };
	void UpdateBuffer();

private:
	void LoadBuffers();

public:

	uint idVertices = 0;
	uint numVertices = 0;
	float* vertices = nullptr;
	uint idIndices = 0;
	uint numIndices = 0;
	uint* indices = nullptr;
	uint idNormals = 2;
	uint numNormals = 0;
	float* normals = nullptr;
	int parIndices = 0;
	float r, g, b, a;
	uint numFaceNormals = 0;
	float* faceNormals = nullptr;
	
	Image* texture = nullptr;

	bool isPrimitive = false;
	//par_shapes_mesh* primitive_mesh = nullptr;

public:

	Transform* transform = nullptr;

};

#endif //_GEOMETRY_H