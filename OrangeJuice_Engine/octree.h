#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "MathGeoLib/Geometry/AABB.h"
#include "GameObject.h"

class Octree
{
public:
	Octree(AABB limits, uint max_objects_in_LVL, uint maxLVL, uint currentLVL, Octree* p = nullptr);
	~Octree();

	bool Clear();

	void Insert(GameObject*);
	bool Remove(GameObject*);
	void Intersect(std::vector<GameObject*>&);

	bool Resize();
	void Subdivide();
	void Draw();
	void DeleteChildren();

private:
	std::vector<GameObject*> staticObjects;
	std::vector<Octree*> children;
	AABB aabb;
	uint maxObjects;
	uint maxLVLs;
	uint currentLVL;

	bool isDivided = false;

	Octree* parent = nullptr;
};

#endif 