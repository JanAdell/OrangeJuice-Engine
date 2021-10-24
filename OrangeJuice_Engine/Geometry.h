#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"

struct Geometry
{
	Geometry(float* ver, uint* ind, float* norm, uint num_vert, uint num_ind, uint num_norm);
	Geometry(Geometry* geo);
	Geometry();
	~Geometry();
	void Draw();

	uint idVertices = 0;
	uint numVertices = 0;
	float* vertices = nullptr;
	uint idIndices = 0;
	uint numIndices = 0;
	uint* indices = nullptr;
	uint idNormals = 2;
	uint numNormals = 0;
	float* normals = nullptr;
};

#endif //_GEOMETRY_H