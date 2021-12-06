#pragma once
#include "Resource.h"
#include "Globals.h"
#include "par_shapes.h"
#include "GameObject.h"
#include "Transform.h"
#include "Image.h"
#include "Geometry.h"

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint uid);
	~ResourceMesh();

	float* GetVertices();
	uint GetNumVertices();
	uint* GetIndices();
	uint GetNumIndices();

	void DrawMesh();
	void DebugDraw(bool vert = true, bool face = true);

	void SetData(float* ver, uint* ind, uint num_vert, uint num_ind, float* normals, float* texture_coords, uint num_coords, uint num_face_normals, uint id_coords);
	void Disable();
	void ActualitzateBuffer();

	void LoadBuffer();
	void CalculateParentBBox(GameObject* object);
	void CreatePrimitive(par_shapes_mesh* p_mesh, Geometry* geo, float col0 = 255, float col1 = 255, float col2 = 255, float col3 = 255);

public:

	Transform* transform;
	Image* material;

	int id_mesh;

	float* vertices;
	uint num_vertices;
	uint id_vertices = 0;

	uint* indices;
	uint num_indices;
	uint id_indices = 0;

	int par_num_indices = 0;

	uint num_normals = 0;
	float* normals;
	uint id_normals = 2;

	uint num_face_normals;
	float* face_normals = nullptr;

	uint num_coords;
	float* uv_coord;
	uint id_coords;

	bool isPrimitive = false;

	float a, r, g, b;
};

