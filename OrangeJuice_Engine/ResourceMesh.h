#pragma once
#include "Resource.h"

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint uid);
	~ResourceMesh();

	float* GetVertices();
	uint GetNumVertices();
	uint* GetIndices();
	uint GetNumIndices();

public:

	float* vertices;
	uint num_vertices;

	uint* indices;
	uint num_indices;
};

