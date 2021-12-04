#include "Application.h"
#include "Octree.h"
#include "ModuleMesh.h"
#include "ModuleScene.h"
#include "Transform.h"
#include "GameObject.h"
#include "MathGeoLib\Math\float3.h"


Octree::Octree(AABB limits, uint max_objects_in_LVL, uint maxLVL, uint currentLVL, Octree* p)
	:aabb(limits), maxObjects(max_objects_in_LVL), maxLVLs(maxLVL), currentLVL(currentLVL), parent(p)
{
}

Octree::~Octree()
{
	Clear();
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

	if (!isDivided)
	{
		staticObjects.push_back(object);
		if (maxObjects < staticObjects.size() && currentLVL <= maxLVLs)
		{
			Subdivide(); 
			isDivided = true;
		}
	}
	else
	{
		for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			if ((*iter)->aabb.Contains(object->bbox->aabb))
				(*iter)->Insert(object);
		}
	}
}

bool Octree::Remove(GameObject* object)
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
		{
			if ((*iter)->Remove(object))
			{
				DeleteChildren();
				if (Resize())
				{
					return true;
				}
				break;
			}
		}
	}
	else
	{
		if (Resize())
		{
			return true;
		}
	}
	return false;
}

bool Octree::Resize()
{
	if (isDivided)
		return false;

	if (parent != nullptr)
	{
		float numObjects = staticObjects.size();

		for (std::vector<Octree*>::iterator iter = parent->children.begin(); iter != parent->children.end(); ++iter)
		{
			numObjects += parent->staticObjects.size();
			for (std::vector<GameObject*>::iterator it = (*iter)->staticObjects.begin(); it != (*iter)->staticObjects.end(); ++it)
			{
				if (*iter != this)
					staticObjects.push_back(*it);

			}
		}
		if (numObjects <= maxObjects)
		{
			for (std::vector<GameObject*>::iterator it = staticObjects.begin(); it != staticObjects.end(); ++it)
			{
				parent->staticObjects.push_back(*it);
			}
			return true;
		}
	}
	return false;
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

	Octree* node0 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL+1, this);
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

	Octree* node1 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1, this);
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

	Octree* node2 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1, this);
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

	Octree* node3 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1, this);
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

	Octree* node4 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1, this);
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

	Octree* node5 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1, this);
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

	Octree* node6 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1, this);
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

	Octree* node7 = new Octree(newAABB, maxObjects, maxLVLs, currentLVL + 1, this);
	children.push_back(node7);

	std::vector<GameObject*>::iterator iter = staticObjects.begin();
	while (iter != staticObjects.end())
	{
		if (*iter != nullptr)
		{
			bool inserted = false;
			for (std::vector<Octree*>::iterator it = children.begin(); it != children.end(); ++it)
			{
				if ((*it)->aabb.Contains((*iter)->bbox->aabb))
				{
					(*it)->Insert((*iter));
					iter = staticObjects.erase(iter);
					inserted = true;
					break;
				}
			}
			if (!inserted) ++iter;
		}
	}
}

void Octree::Draw()
{
	//GameObject go;
	//go.DrawBBox(go.bbox->aabb);

	for (int i = 0; i < 4; i++)
	{
		glVertex3f(aabb.CornerPoint(i + 4).x, aabb.CornerPoint(i + 4).y, aabb.CornerPoint(i + 4).z);
		glVertex3f(aabb.CornerPoint(i).x, aabb.CornerPoint(i).y, aabb.CornerPoint(i).z);
	}

	//create the vertices that define the faces that form the mesh box
	for (int i = 0; i <= 4; i += 4)
	{
		glVertex3f(aabb.CornerPoint(i).x, aabb.CornerPoint(i).y, aabb.CornerPoint(i).z);
		glVertex3f(aabb.CornerPoint(i + 1).x, aabb.CornerPoint(i + 1).y, aabb.CornerPoint(i + 1).z);

		glVertex3f(aabb.CornerPoint(i + 2).x, aabb.CornerPoint(i + 2).y, aabb.CornerPoint(i + 2).z);
		glVertex3f(aabb.CornerPoint(i + 3).x, aabb.CornerPoint(i + 3).y, aabb.CornerPoint(i + 3).z);

		glVertex3f(aabb.CornerPoint(i).x, aabb.CornerPoint(i).y, aabb.CornerPoint(i).z);
		glVertex3f(aabb.CornerPoint(i + 2).x, aabb.CornerPoint(i + 2).y, aabb.CornerPoint(i + 2).z);

		glVertex3f(aabb.CornerPoint(i + 1).x, aabb.CornerPoint(i + 1).y, aabb.CornerPoint(i + 1).z);
		glVertex3f(aabb.CornerPoint(i + 3).x, aabb.CornerPoint(i + 3).y, aabb.CornerPoint(i + 3).z);
	}


	if (isDivided)
	{
		LOG("is divided");
		for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			(*iter)->Draw();
			LOG("Drawing octree");
		}
	}

	
}

void Octree::DeleteChildren()
{
	for (std::vector<Octree*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}

	isDivided = false;
	children.clear();
}