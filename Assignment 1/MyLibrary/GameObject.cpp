#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue) 
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	mass(0.f),
	color(1,1,1)
{
}

GameObject::~GameObject()
{
}