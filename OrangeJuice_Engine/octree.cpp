#include "Application.h"
#include "Octree.h"
#include "ModuleMesh.h"
#include "GameObject.h"
#include "MathGeoLib\Math\float3.h"


Octree::Octree(AABB limits, uint max_objects_in_LVL, uint maxLVLs, uint currentLVL)
	:aabb(limits), maxObjects(max_objects_in_LVL), maxLVLs(maxLVLs), currentLVL(currentLVL) {
}

Octree::~Octree()
{
	for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}
}

bool Octree::Clear()
{
	bool ret = true;
	for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		(*iter)->Clear();
		delete* iter;
		*iter = nullptr;
	}
	children.clear();
	staticObjects.clear();

	return ret;
}

void Octree::Insert(GameObject* object)
{
	if (!object->isStatic)
		return;

	if (!isDivided)
	{
		if (maxObjects < staticObjects.size())
		{
			Subdivide(); 
			isDivided = true;
		}
		else
			staticObjects.push_back(object);
	}
	else
	{
		for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			if ((*iter)->aabb.Contains(object->bbox))
				(*iter)->Insert(object);
		}
	}
}

void Octree::Remove(GameObject* object)
{
	bool is_removed = false;
	for (std::vector<GameObject*>::iterator iter = staticObjects.begin(); iter != staticObjects.end(); ++iter)
	{
		if (object == *iter)
		{
			staticObjects.erase(iter);
			is_removed = true;
			break;
		}
	}
	if (!is_removed)
	{
		for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
			(*iter)->Remove(object);
	}
}

void Octree::Intersect(GameObject*)
{
}

void Octree::Subdivide()
{
	//1 division left-down-front
	float3 AABBpoints[8];
	AABBpoints[0] = aabb.CornerPoint(0);
	AABBpoints[1] = (aabb.CornerPoint(0) + aabb.CornerPoint(1)) / 2;
	AABBpoints[2] = (aabb.CornerPoint(0) + aabb.CornerPoint(4)) / 2;
	AABBpoints[3] = (aabb.CornerPoint(0) + aabb.CornerPoint(2)) / 2;
	AABBpoints[4] = aabb.FaceCenterPoint(0);//-X
	AABBpoints[5] = aabb.FaceCenterPoint(2);//-Y
	AABBpoints[6] = aabb.FaceCenterPoint(4);//-Z
	AABBpoints[7] = aabb.CenterPoint();
	math::AABB newAABB;
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node0 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL+1);
	children.push_back(node0);

	//2 division left-up-front
	AABBpoints[0] = aabb.CornerPoint(2);
	AABBpoints[1] = (aabb.CornerPoint(2) + aabb.CornerPoint(3)) / 2;
	AABBpoints[2] = (aabb.CornerPoint(2) + aabb.CornerPoint(0)) / 2;
	AABBpoints[3] = (aabb.CornerPoint(2) + aabb.CornerPoint(6)) / 2;
	AABBpoints[4] = aabb.FaceCenterPoint(0);//-X
	AABBpoints[5] = aabb.FaceCenterPoint(3);//+Y
	AABBpoints[6] = aabb.FaceCenterPoint(4);//-Z
	AABBpoints[7] = aabb.CenterPoint();
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node1 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1);
	children.push_back(node1);

	//3 division right-down-front
	AABBpoints[0] = aabb.CornerPoint(4);
	AABBpoints[1] = (aabb.CornerPoint(4) + aabb.CornerPoint(5)) / 2;
	AABBpoints[2] = (aabb.CornerPoint(4) + aabb.CornerPoint(0)) / 2;
	AABBpoints[3] = (aabb.CornerPoint(4) + aabb.CornerPoint(6)) / 2;
	AABBpoints[4] = aabb.FaceCenterPoint(1);//+X
	AABBpoints[5] = aabb.FaceCenterPoint(2);//-Y
	AABBpoints[6] = aabb.FaceCenterPoint(4);//-Z
	AABBpoints[7] = aabb.CenterPoint();
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node2 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1);
	children.push_back(node2);

	//4 division right-down-front
	AABBpoints[0] = aabb.CornerPoint(6);
	AABBpoints[1] = (aabb.CornerPoint(6) + aabb.CornerPoint(7) / 2);
	AABBpoints[2] = (aabb.CornerPoint(6) + aabb.CornerPoint(4) / 2);
	AABBpoints[3] = (aabb.CornerPoint(6) + aabb.CornerPoint(2) / 2);
	AABBpoints[4] = aabb.FaceCenterPoint(1);//+X
	AABBpoints[5] = aabb.FaceCenterPoint(3);//+Y
	AABBpoints[6] = aabb.FaceCenterPoint(4);//-Z
	AABBpoints[7] = aabb.CenterPoint();
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node3 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1);
	children.push_back(node3);

	//5 division left-down-back
	AABBpoints[0] = aabb.CornerPoint(1);
	AABBpoints[1] = (aabb.CornerPoint(1) + aabb.CornerPoint(0)) / 2;
	AABBpoints[2] = (aabb.CornerPoint(1) + aabb.CornerPoint(3)) / 2;
	AABBpoints[3] = (aabb.CornerPoint(1) + aabb.CornerPoint(5)) / 2;
	AABBpoints[4] = aabb.FaceCenterPoint(0);//-X
	AABBpoints[5] = aabb.FaceCenterPoint(2);//-Y
	AABBpoints[6] = aabb.FaceCenterPoint(5);//+Z
	AABBpoints[7] = aabb.CenterPoint();
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node4 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1);
	children.push_back(node4);

	//6 division left-up-back
	AABBpoints[0] = aabb.CornerPoint(3);
	AABBpoints[1] = (aabb.CornerPoint(3) + aabb.CornerPoint(1)) / 2;
	AABBpoints[2] = (aabb.CornerPoint(3) + aabb.CornerPoint(2)) / 2;
	AABBpoints[3] = (aabb.CornerPoint(3) + aabb.CornerPoint(7)) / 2;
	AABBpoints[4] = aabb.FaceCenterPoint(0);//-X
	AABBpoints[5] = aabb.FaceCenterPoint(3);//+Y
	AABBpoints[6] = aabb.FaceCenterPoint(5);//+Z
	AABBpoints[7] = aabb.CenterPoint();
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node5 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1);
	children.push_back(node5);

	//7 division right-down-back
	AABBpoints[0] = aabb.CornerPoint(5);
	AABBpoints[1] = (aabb.CornerPoint(5) + aabb.CornerPoint(1)) / 2;
	AABBpoints[2] = (aabb.CornerPoint(5) + aabb.CornerPoint(4)) / 2;
	AABBpoints[3] = (aabb.CornerPoint(5) + aabb.CornerPoint(7)) / 2;
	AABBpoints[4] = aabb.FaceCenterPoint(1);//+X
	AABBpoints[5] = aabb.FaceCenterPoint(2);//-Y
	AABBpoints[6] = aabb.FaceCenterPoint(5);//+Z
	AABBpoints[7] = aabb.CenterPoint();
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node6 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1);
	children.push_back(node6);

	//8 division right-up-back
	AABBpoints[0] = aabb.CornerPoint(7);
	AABBpoints[1] = (aabb.CornerPoint(7) + aabb.CornerPoint(3)) / 2;
	AABBpoints[2] = (aabb.CornerPoint(7) + aabb.CornerPoint(5)) / 2;
	AABBpoints[3] = (aabb.CornerPoint(7) + aabb.CornerPoint(6)) / 2;
	AABBpoints[4] = aabb.FaceCenterPoint(1);//+X
	AABBpoints[5] = aabb.FaceCenterPoint(3);//+Y
	AABBpoints[6] = aabb.FaceCenterPoint(5);//+Z
	AABBpoints[7] = aabb.CenterPoint();
	newAABB.SetNegativeInfinity();
	newAABB.Enclose(&AABBpoints[0], 8);

	Octree* node7 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1);
	children.push_back(node7);

	uint i = 0;
	while (i < staticObjects.size())
	{
		if (staticObjects[i] != nullptr)
		{
			for (std::vector<Octree*>::iterator it = children.begin(); it != children.end(); ++it)
			{
				if ((*it)->aabb.Contains(staticObjects[i]->bbox))
				{
					(*it)->Insert(staticObjects[i]);
					break;
				}
			}
		}
	}
}

void Octree::Draw()
{
	GameObject go;
	go.DrawBBox(go.bbox);

	if (isDivided)
		for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
			(*iter)->Draw();
}