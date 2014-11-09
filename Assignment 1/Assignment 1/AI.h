#pragma once

#include <iostream>
#include <vector>
#include "Vector3.h"

#include "GameObject.h"
#include "freeglut.h"

class AI
{
public:

	static AI* GetInstance()
	{
		static AI s_instance;
		return &s_instance;
	}
	void Init();
	void Exit();

	void SetupRenderContext();
	void GlutReshape(GLsizei w, GLsizei h);
	void GlutMouseClick(int button, int state, int x, int y);
	void GlutMouseMove(int x, int y);
	void GlutKeyboard(unsigned char key, int x, int y);
	void GlutIdle();
	void GlutDisplay();
	void GlutSpecialKey(int key, int x, int y);
	void DrawObject(GameObject *go);
	void RenderStringOnScreen(float x, float y, const char* quote);



private:
	AI();
	~AI();

	std::vector<GameObject *> m_goList;
	float m_fps;
	float m_speed;
	float m_worldSizeX;
	float m_worldSizeY;
	Vector3 m_force;

	int m_lives;
	int m_score;

	bool Alarm;

	GameObject* FetchGO();
};