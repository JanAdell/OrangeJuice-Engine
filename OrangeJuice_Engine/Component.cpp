#include "Component.h"
#include "GameObject.h"


Component::Component(GameObject* parent, COMPONENT_TYPE type) : parent(parent), type(type)
{
	componentUUID = parent->CreateUUID();
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

COMPONENT_TYPE Component::GetComponentType()
{
	return COMPONENT_TYPE::NO_COMPONENT;
}
