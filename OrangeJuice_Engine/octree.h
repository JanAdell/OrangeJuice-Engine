#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "MathGeoLib/Geometry/AABB.h"
#include "GameObject.h"

class Octree
{
public:
	Octree(AABB limits, uint max_objects_in_level);
	~Octree();

	bool Clear();
	void Insert(GameObject*);
	void Remove(GameObject*);
	void Intersect(GameObject*);

	void Subdivide();

private:
	std::vector<GameObject*> staticObjects;
	std::vector<Octree*> children;
	AABB aabb;
	uint maxObjects;

	bool isDivided = false;
};

#endif 