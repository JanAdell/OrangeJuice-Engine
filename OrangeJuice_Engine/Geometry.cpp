#include "Geometry.h"
#include <vector>
#include "ImGui/imgui.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"

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

	memcpy(vertices, p_mesh->points, sizeof(float) * numVertices * 3);
	memcpy(indices, p_mesh->triangles, sizeof(uint) * numIndices);
	memcpy(normals, p_mesh->normals, sizeof(float) * numVertices * 3);

	if (p_mesh->normals != NULL)
	{
		memcpy(normals, p_mesh->normals, sizeof(float) * numVertices * 3);
		numNormals = numVertices * 3;
	}

	r = col0;
	g = col1;
	b = col2;
	a = col3;

	transform = dynamic_cast<Transform*>(parent->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	texture = dynamic_cast<Image*>(parent->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
	texture->LoadCoords(p_mesh);	
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
			glBindBuffer(GL_ARRAY_BUFFER, texture->idCoords);
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

	DebugDraw();
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
			numNormals = mesh->mNumVertices * 3;
			normals = new float[mesh->mNumVertices * 3];
			memcpy(normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);
		}

		numFaceNormals = numVertices * 2;
		faceNormals = new float[numFaceNormals];
		uint j = 0;
		for (uint i = 0; i < numVertices; i += 9)
		{
			float u[3] = { (vertices[i + 3] - vertices[i + 0]),(vertices[i + 4] - vertices[i + 1]),(vertices[i + 5] - vertices[i + 2]) };
			float v[3] = { (vertices[i + 6] - vertices[i + 3]),(vertices[i + 7] - vertices[i + 4]),(vertices[i + 8] - vertices[i + 5]) };
			faceNormals[j] = (vertices[i] + vertices[i + 3] + vertices[i + 6]) / 3;
			faceNormals[j + 1] = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;;
			faceNormals[j + 2] = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;;
			faceNormals[j + 3] = faceNormals[j] + (u[1] * v[2] - u[2] * v[1]);
			faceNormals[j + 4] = faceNormals[j + 1] + (u[2] * v[0] - u[0] * v[2]);
			faceNormals[j + 5] = faceNormals[j + 2] + (u[0] * v[1] - u[1] * v[0]);
			j += 6;
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

	/*if (ImGui::CollapsingHeader("Information"))
	{
		ImGui::Text("triangles: %u", numIndices / 3);
		ImGui::Text("vertices: %u", numVertices);
	}*/

	if (ImGui::CollapsingHeader("Transformation"))
	{
		ImGui::SliderInt3("Scale", scale, 1, 10);
		ImGui::SliderInt3("Translation", translation, -100, 100);
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

void Geometry::UpdateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);
}
