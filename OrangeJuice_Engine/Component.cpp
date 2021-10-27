#include "Component.h"
#include "GameObject.h"


Component::Component(GameObject* parent, COMPONENT_TYPE type) : parent(parent), type(type)
{
}

Component::~Component()
{
}

void Component::Enable()
{
	isEnable = true;
}

void Component::Disable()
{
	isEnable = false;
}