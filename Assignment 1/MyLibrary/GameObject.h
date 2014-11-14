#pragma once
#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_POLICE,
		GO_ROBBER,
		GO_WAYPOINTS,
		GO_WALL,
		GO_EXIT,
		GO_TOTAL //must be last
	};

	enum STATES
	{
		PATROLLING = 0,
		MOVING,
		CHASING,

		STEALING,
		RUNNING,
		CAUGHT
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

	int Target;
	bool TargetLocked;
	Vector3 TargetPos;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_NONE);
	~GameObject();
};