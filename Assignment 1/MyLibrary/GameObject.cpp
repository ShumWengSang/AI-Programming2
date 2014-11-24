#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue) 
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	money(0.f),
	color(1,1,1)
{
	Target = 0;
}

GameObject::~GameObject()
{
}