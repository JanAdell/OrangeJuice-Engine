#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(uint uid) : Resource(uid, ResourceType::MESH)
{
	vertices = 0;
	num_vertices = 0;
	indices = 0;
	num_indices = 0;
}

ResourceMesh::~ResourceMesh()
{
	delete vertices;
	vertices = nullptr;
	num_vertices = 0;

	delete indices;
	indices = nullptr;
	num_indices = 0;
}

float* ResourceMesh::GetVertices()
{
	return vertices;
}

uint ResourceMesh::GetNumVertices()
{
	return num_vertices;
}

uint* ResourceMesh::GetIndices()
{
	return indices;
}

uint ResourceMesh::GetNumIndices()
{
	return num_indices;
}
