#include "Geometry.h"

Geometry::Geometry(float* ver, uint* ind, float* norm, uint num_vert, uint num_ind, uint num_norm)
	: vertices(ver), indices(ind), normals(norm), numVertices(num_vert), numIndices(num_ind), numNormals(num_norm) 
{
	glGenBuffers(1, (uint*)&(idVertices));
	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, (uint*)&(idIndices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * numIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(idNormals));
	glBindBuffer(GL_ARRAY_BUFFER, idNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * numNormals, normals, GL_STATIC_DRAW);
}

Geometry::Geometry(Geometry* geo)
	: vertices(geo->vertices), indices(geo->indices), normals(geo->normals), numVertices(geo->numVertices), numIndices(geo->numIndices), numNormals(geo->numNormals)
{
	glGenBuffers(1, (uint*)&(idVertices));
	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(idIndices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * numIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(idNormals));
	glBindBuffer(GL_ARRAY_BUFFER, idNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numNormals * 3, normals, GL_STATIC_DRAW);
}

Geometry::Geometry()
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
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);


	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, idNormals);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, numNormals * 3);
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