#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "MathGeoLib/Geometry/AABB.h"
#include "GameObject.h"

class Octree
{
public:
	Octree(AABB limits, uint max_objects_in_LVL, uint maxLVLs, uint currentLVL);
	~Octree();

	bool Clear();
	void Insert(GameObject*);
	void Remove(GameObject*);
	void Intersect(GameObject*);

	void Subdivide();
	void Draw();

private:
	std::vector<GameObject*> staticObjects;
	std::vector<Octree*> children;
	AABB aabb;
	uint maxObjects;
	uint maxLVLs;
	uint currentLVL;

	bool isDivided = false;
};

#endif 