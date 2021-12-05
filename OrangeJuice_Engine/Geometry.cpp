#include "Geometry.h"
#include <vector>
#include "ImGui/imgui.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleFile.h"
#include "ModuleInput.h"
#include <fstream>

//Primitives constructor

Geometry::Geometry(GameObject* parent) :Component(parent, COMPONENT_TYPE::COMPONENT_MESH)
{
	r = g = b = a = 255;
	isEnable = true;
}
Geometry::~Geometry()
{
}

void Geometry::Disable()
{
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;
	delete[] normals;
	normals = nullptr;
}

void Geometry::CreatePrimitive(par_shapes_mesh* p_mesh, float col0, float col1, float col2, float col3)
{
	numVertices = p_mesh->npoints;
	parIndices = p_mesh->ntriangles;
	numIndices = p_mesh->ntriangles * 3;

	vertices = new float[numVertices * 3];
	indices = new uint[numIndices];
	normals = new float[numVertices * 3];
	uvCoords = new float[numCoords];
	memcpy(vertices, p_mesh->points, sizeof(float) * numVertices * 3);
	memcpy(indices, p_mesh->triangles, sizeof(uint) * numIndices);

	if (p_mesh->normals != NULL)
	{
		memcpy(normals, p_mesh->normals, sizeof(float) * numVertices * 3);
		numNormals = numVertices * 3;
	}
	if (p_mesh->tcoords != nullptr)
	{
		memcpy(uvCoords, p_mesh->tcoords, sizeof(float) * numCoords);
	}

	r = col0;
	g = col1;
	b = col2;
	a = col3;

	transform = dynamic_cast<Transform*>(parent->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	texture = dynamic_cast<Image*>(parent->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
	
	CalculateParentBBox(parent);

	LoadBuffers();

	LOG("Primitive created");
}


//DebugDraw for all geometries
void Geometry::DebugDraw()
{
	if (parent->showVertexNormals && numNormals != 0)
	{
		glEnableClientState(GL_VERTEX_ARRAY);

		for (uint i = 0; i < numVertices * 3; i += 3)
		{
			glColor3f(3.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
			glVertex3f(vertices[i] + normals[i] * 2, vertices[i + 1] + normals[i + 1] * 2, vertices[i + 2] + normals[i + 2] * 2);
			glEnd();
			glColor3f(1.0f, 1.0f, 1.0f);
		}

		if (parent->showNormals && numNormals != 0)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			for (uint i = 0; i < numFaceNormals; i += 6)
			{
				glColor3f(3.0f, 0.0f, 1.0f);
				glBegin(GL_LINES);
				glVertex3f(faceNormals[i], faceNormals[i + 1], faceNormals[i + 2]);
				glVertex3f(faceNormals[i + 3], faceNormals[i + 4], faceNormals[i + 5]);
				glEnd();
				glColor3f(1.0f, 1.0f, 1.0f);
			}

			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}

void Geometry::Update()
{

}

void Geometry::DrawMesh()
{
	glPushMatrix();
	if (transform != nullptr)
		glMultMatrixf((GLfloat*)&transform->globalMatrix.Transposed());
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(r, g, b, a);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	if (texture != nullptr)
	{
		if (texture->textureId != 0 && texture->show)
		{
			//Bind textures
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, texture->textureId);
			glBindBuffer(GL_ARRAY_BUFFER, idCoords);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
	}
	else
		glColor4f(r, g, b, a);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopAttrib();
	glPopMatrix();
	DebugDraw();
}

void Geometry::ShowProperties()
{
	
	if (transform->LoadTransformation(this))
		UpdateBuffer();
}

void Geometry::LoadBuffers()
{
	glGenBuffers(1, (uint*)&(idVertices));
	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(idIndices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * numIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(idCoords));
	glBindBuffer(GL_ARRAY_BUFFER, idCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numCoords, uvCoords, GL_STATIC_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG("Error Storing Indices! %s\n", gluErrorString(error));

}

void Geometry::UpdateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);
}

void Geometry::Save(FILE* file)
{
	fputs("vertices: ", file);
	for (int i = 0; i < numVertices * 3; ++i)
	{
		fprintf(file, "vertex: %f ", vertices[i]);

	}
	fputs(";\n", file);
	fputs("indices: ", file);
	for (int i = 0; i < numIndices; ++i)
	{
		fprintf(file, "indice: %u ", indices[i]);
	}
	fputs(";\n", file);
	fputs("normals: ", file);
	for (int i = 0; i < numNormals; ++i)
	{
		fprintf(file, "normal: %f ", normals[i]);
	}
	fputs(";\n", file);
	fputs("face_normals: ", file);
	for (int i = 0; i < numFaceNormals; ++i)
	{
		fprintf(file, "face_normal: %f ", faceNormals[i]);
	}
	fputs(";\n", file);

	fputs("uv_coords: ", file);
	for (int i = 0; i < numCoords; ++i)
	{
		fprintf(file, "coord: %f ", uvCoords[i]);
	}
	fputs(";\n", file);
}


void Geometry::CalculateParentBBox(GameObject* object)
{
	std::vector <float3> vertex_array;

	if (vertices == nullptr)
		return;

	for (int i = 0; i < numVertices * 3; i += 3)
		vertex_array.push_back(float3(vertices[i], vertices[i + 1], vertices[i + 2]));

	object->bbox->aabb.Enclose(&vertex_array[0], (int)numVertices);

	if (object->parent != nullptr)
	{
		CalculateParentBBox(object->parent);
	}
}
