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
	texture->LoadCoords(p_mesh);	

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
	
	if (App->gui->frustumCulling)
	{
		glPushMatrix();
		if (transform->globalMatrix.IsInvertible())
			glMultMatrixf((GLfloat*)&transform->globalMatrix.Transposed());
		glPushAttrib(GL_CURRENT_BIT);
		glColor4f(r, g, b, a);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, idVertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		if (texture != nullptr)
		{
			if (App->mesh->IsCulling(this))
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

				DebugDraw();
			}
		}
	}
	else
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


	DebugDraw();
}

/*void Geometry::LoadData(aiMesh* mesh)
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
			faceNormals[j + 1] = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;
			faceNormals[j + 2] = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;
			faceNormals[j + 3] = faceNormals[j] + (u[1] * v[2] - u[2] * v[1]);
			faceNormals[j + 4] = faceNormals[j + 1] + (u[2] * v[0] - u[0] * v[2]);
			faceNormals[j + 5] = faceNormals[j + 2] + (u[0] * v[1] - u[1] * v[0]);
			j += 6;
		}

		if (mesh->HasTextureCoords(0))
		{
			numCoords = mesh->mNumVertices * 2;
			uvCoords = new float[numCoords];
			for (uint i = 0; i < mesh->GetNumUVChannels(); ++i)
			{
				for (uint k = 0; k < mesh->mNumVertices; ++k) {
					uvCoords[k * 2] = mesh->mTextureCoords[i][k].x;
					uvCoords[k * 2 + 1] = mesh->mTextureCoords[i][k].y;
					
				}
			}
		}
	}
	LoadBuffers();

	parent->CreateBBOX();
}*/

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

	for (int i = 0; i < numIndices * 3; i += 3)
		vertex_array.push_back(float3(vertices[i], vertices[i + 1], vertices[i + 2]));

	object->bbox->aabb.Enclose(&vertex_array[0], (int)numVertices);

	if (object->parent != nullptr)
	{
		CalculateParentBBox(object->parent);
	}
}
/*
void Geometry::ImportNewMesh(char*& cursor)
{
	//vertices	
	std::string data = App->file->DataValue(cursor, "vertices:", 100000, ";");
	std::vector<float> elements;
	while (1)
	{
		char* value = App->file->DataValue(data, "vertex:", 10, "vertex:");
		if (value == ";")
			break;
		std::stringstream convertor(value);
		float el;
		convertor >> el;
		elements.push_back(el);
	}

	numVertices = elements.size() / 3;
	vertices = new float[numVertices * 3];

	int j = 0;
	for (std::vector<float>::iterator i = elements.begin(); i != elements.end(); ++i)
	{
		vertices[j] = (*i);
		++j;
	}

	//indices	
	data = App->file->DataValue(cursor, "indices:", 100000, ";");
	std::vector<uint>elements2;
	while (1)
	{
		char* value = App->file->DataValue(data, "indice:", 2, "indice:");
		if (value == ";")
			break;
		std::stringstream convertor(value);
		uint el;
		convertor >> el;
		elements2.push_back(el);
	}

	numIndices = elements2.size();
	indices = new uint[numIndices];
	j = 0;
	for (std::vector<uint>::iterator i = elements2.begin(); i != elements2.end(); ++i)
	{
		indices[j] = *i;
		++j;
	}
	elements2.clear();

	//normals
	data = App->file->DataValue(cursor, "normals:", 100000, ";");
	elements.clear();

	while (1)
	{
		char* value = App->file->DataValue(data, "normal:", 10, "normal:");
		if (value == ";")
			break;
		std::stringstream convertor(value);
		float el;
		convertor >> el;
		elements.push_back(el);
	}

	numNormals = elements.size() / 3;
	normals = new float[numNormals * 3];

	j = 0;
	for (std::vector<float>::iterator i = elements.begin(); i != elements.end(); ++i)
	{
		normals[j] = (*i);
		++j;
	}

	//face normals
	data = App->file->DataValue(cursor, "face_normals:", 100000, ";");
	elements.clear();
	while (1)
	{
		char* value = App->file->DataValue(data, "face_normal:", 10, "face_normal:");
		if (value == ";")
			break;
		std::stringstream convertor(value);
		float el;
		convertor >> el;
		elements.push_back(el);
	}
	numFaceNormals = elements.size();
	faceNormals = new float[numFaceNormals];

	j = 0;
	for (std::vector<float>::iterator i = elements.begin(); i != elements.end(); ++i)
	{
		faceNormals[j] = (*i);
		++j;
	}

	//uvs
	data = App->file->DataValue(cursor, "uv_coords:", 100000, ";");
	elements.clear();
	while (1)
	{
		char* value = App->file->DataValue(data, "coord:", 10, "coord:");
		if (value == ";")
			break;
		std::stringstream convertor(value);
		float el;
		convertor >> el;
		elements.push_back(el);
	}
	numCoords = elements.size();
	uvCoords = new float[numCoords];

	j = 0;
	for (std::vector<float>::iterator i = elements.begin(); i != elements.end(); ++i)
	{
		uvCoords[j] = (*i);
		++j;
	}
	elements.clear();
	LoadBuffers();
	
	CalculateParentBBox(parent);
}

void Geometry::ImportNewMaterial(char*& cursor)
{
	std::string data = App->file->DataValue(cursor, "texture_id:", 100000, ";");
	std::stringstream convertor(data);
	int el;
	convertor >> el;
	texture->textureId = el;
}*/