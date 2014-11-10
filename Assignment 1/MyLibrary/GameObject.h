#pragma once

#include "Vector3.h"

struct GameObject
{
	enum STATES
	{
		PATROLLING = 0,
		CHASING,
		MOVING,
	};
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_POLICE,
		GO_ROBBER,
		GO_BULLET,
		GO_WALL,
		GO_TOTAL //must be last
	};

	STATES CurrentState;
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	bool active;
	float mass;
	Vector3 color;
	Vector3 normal;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_NONE);
	~GameObject();
};