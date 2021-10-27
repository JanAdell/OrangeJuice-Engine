#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"

#include "Image.h"

class Geometry: public Component
{
public:
	Geometry(float* ver, uint* ind, float* norm, uint numVert, uint numInd, uint numNorm, GameObject* parent = nullptr);
	Geometry(Geometry* geo, GameObject* parent);
	Geometry(GameObject* parent);
	~Geometry();
	void Draw();
	void DebugDraw();

	void Enable() override;
	void Update() override;
	void Disable() override;

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
	
	Image* texture = nullptr;

public:

	GameObject* parent;

};

#endif //_GEOMETRY_H