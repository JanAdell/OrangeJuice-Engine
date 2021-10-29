#include "Geometry.h"
#include <vector>
#include "ImGui/imgui.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"

//Primitives constructor

Geometry::Geometry(GameObject* parent) :Component(parent, COMPONENT_TYPE::COMPONENT_MESH)
{
	r = g = b = a = 255;
	isEnable = true;
}
Geometry::~Geometry()
{
}

void Geometry::CreatePrimitive(par_shapes_mesh* p_mesh, float col0, float col1, float col2, float col3)
{
	numVertices = p_mesh->npoints;
	parIndices = p_mesh->ntriangles;
	numIndices = p_mesh->ntriangles * 3;

	vertices = p_mesh->points;
	indices = p_mesh->triangles;
	normals = p_mesh->normals;

	r = col0;
	g = col1;
	b = col2;
	a = col3;

	transform = new Transform(parent);
	LoadBuffers();

	LOG("Primitive created");
}


//DebugDraw for all geometries
void Geometry::DebugDraw()
{//TODO: Fix normals in the primitive geometries
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	for (uint i = 0; i < numVertices * 3; i += 3)
	{
		glColor3f(3.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
		glVertex3f(vertices[i] + normals[i] * 10, vertices[i + 1] + normals[i + 1] * 10, vertices[i + 2] + normals[i + 2] * 10);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

//Draw primitives geometries
void Geometry::DrawPrimitives()
{


	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, parIndices * 3, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);

}

void Geometry::Update()
{
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(r, g, b, a);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	if (texture != nullptr)
	{
		if (texture->textureId != 0)
		{
			//Bind textures
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, texture->textureId);
			glBindBuffer(GL_ARRAY_BUFFER, texture->idCoords);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
	}
	else
		glColor4f(r, g, b, a);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopAttrib();
}

void Geometry::LoadData(aiMesh* mesh)
{
	//Load vertex
	numVertices = mesh->mNumVertices;
	vertices = new float[numVertices * 3];
	memcpy(vertices, mesh->mVertices, sizeof(float) * numVertices * 3);
	LOG("New mesh with %d vertices", vertices);

	//load index
	if (mesh->HasFaces())
	{
		numIndices = mesh->mNumFaces * 3;
		indices = new uint[numIndices * 3];
		for (uint j = 0; j < mesh->mNumFaces; ++j)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else
				memcpy(&indices[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint));
		}
		//load normals
		if (mesh->HasNormals())
		{
			normals = new float[mesh->mNumVertices * 3];
			memcpy(normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);
		}
	}
	LoadBuffers();
}

void Geometry::ShowProperties()
{
	static int scale[3] = { 1,1,1 };
	static int translation[3] = { 0,0,0 };
	static int rad = 0;
	static float axis[3] = { 0,0,0 };
	if (ImGui::CollapsingHeader("Transformation"))
	{
		ImGui::SliderInt3("Scale", scale, 1, 10);
		ImGui::SliderInt3("Translation", translation, 0, 100);
		ImGui::TextWrapped("Rotation");
		ImGui::Separator();
		ImGui::SliderInt("Radiant", &rad, 0, 360);

		static int item_current = 0;
		const char* items[] = { "X", "Y", "Z" };
		ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
		switch (item_current)
		{
		case 0:
			axis[0] = 1;
			axis[1] = 0;
			axis[2] = 0;
			break;
		case 1:
			axis[0] = 0;
			axis[1] = 1;
			axis[2] = 0;
			break;
		case 2:
			axis[0] = 0;
			axis[1] = 0;
			axis[2] = 1;
			break;
		}
		if (ImGui::Button("Transform"))
		{
			transform->LoadTransformation(this, translation, scale, rad, axis);

			glBindBuffer(GL_ARRAY_BUFFER, idVertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * numIndices, indices, GL_STATIC_DRAW);
		}
	}
}

void Geometry::LoadBuffers()
{
	glGenBuffers(1, (uint*)&(idVertices));
	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(idIndices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * numIndices, indices, GL_STATIC_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG("Error Storing Indices! %s\n", gluErrorString(error));

}