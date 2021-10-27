#include "Geometry.h"

Geometry::Geometry(float* ver, uint* ind, float* norm, uint numVert, uint numInd, uint numNorm, GameObject* parent):Component(parent, COMPONENT_TYPE::COMPONENT_MESH),
	vertices(ver), indices(ind), normals(norm), numVertices(numVert), numIndices(numInd), numNormals(numNorm) 
{
	glGenBuffers(1, (uint*)&(idVertices));
	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, (uint*)&(idIndices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * numIndices, indices, GL_STATIC_DRAW);

}

Geometry::Geometry(Geometry* geo, GameObject* parent) : Component(parent, COMPONENT_TYPE::COMPONENT_MESH),
	vertices(geo->vertices), indices(geo->indices), normals(geo->normals), numVertices(geo->numVertices), numIndices(geo->numIndices), numNormals(geo->numNormals), texture(geo->texture)
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

	if (texture != nullptr)
	{
		glGenBuffers(1, (uint*)&(texture->idCoords));
		glBindBuffer(GL_ARRAY_BUFFER, texture->idCoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 2, texture->uvCoord, GL_STATIC_DRAW);
		error = glGetError();
		if (error != GL_NO_ERROR)
			LOG("Error Storing textures! %s\n", gluErrorString(error));

		error = glGetError();
		if (error != GL_NO_ERROR)
			LOG("Error Storing textures! %s\n", gluErrorString(error));

	}
}

Geometry::Geometry(GameObject* parent) :Component(parent, COMPONENT_TYPE::COMPONENT_MESH)
{
}

Geometry::~Geometry()
{
	delete[] vertices;
	delete[] indices;
}

void Geometry::Draw()
{
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
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
			glBindTexture(GL_TEXTURE_2D, texture->textureId);
			glBindBuffer(GL_ARRAY_BUFFER, texture->idCoords);
		}
		
	}

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

}


void Geometry::DebugDraw()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	for (uint i = 0; i < numVertices * 3; i += 3)
	{
		glColor3f(3.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
		glVertex3f(vertices[i] + normals[i] * 2, vertices[i + 1] + normals[i + 1] * 2, vertices[i + 2] + normals[i + 2] * 2);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void Geometry::Enable()
{
}

void Geometry::Update()
{
}

void Geometry::Disable()
{
}