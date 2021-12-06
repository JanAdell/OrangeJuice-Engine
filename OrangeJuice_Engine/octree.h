#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "MathGeoLib/Geometry/AABB.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"
#include <vector>

class Octree
{
public:
	Octree(AABB limits, uint max_objects_in_LVL, uint maxLVL, uint currentLVL, Octree* p = nullptr);
	~Octree();

	bool Clear();

	void Insert(GameObject*);
	bool Remove(GameObject*);

	bool Resize();
	void Subdivide();
	void Draw();
	void DeleteChildren();

	template<typename TYPE>
	void CollectObjects(const TYPE& primitive, std::vector<MouseHit>& objects);

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

template<typename TYPE>
void Octree::CollectObjects(const TYPE& primitive, std::vector<MouseHit>& objects)
{
	for (std::vector<GameObject*>::iterator iter = staticObjects.begin(); iter != staticObjects.end(); ++iter)
	{
		if ((*iter)->bbox->aabb.Intersects(primitive))
		{
			MouseHit h;
			h.object = *iter;
			objects.push_back(h);
		}
	}
	for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		if ((*iter)->aabb.Intersects(primitive))
			(*iter)->CollectObjects(primitive, objects);
	}
}

#endif 