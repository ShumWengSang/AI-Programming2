#pragma once
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <vector>
#include "Vector3.h"
#include "irrKlang.h"
#include "GameObject.h"
#include "freeglut.h"


#pragma comment(lib,"irrKlang.lib")

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
//The above will suppress the console application from opening

typedef struct												// Create A Structure
{
	GLubyte	*imageData;										// Image Data (Up To 32 Bits)
	GLuint	bpp;											// Image Color Depth In Bits Per Pixel.
	GLuint	width;											// Image Width
	GLuint	height;											// Image Height
	GLuint	texID;											// Texture ID Used To Select A Texture
	GLuint	frameNumbers;									// Number of frames in the texture
} TextureImage;												// Structure Name

struct WayPoint
{
	Vector3 thePoint;
	WayPoint * nextPoint;
};

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

	void GotoLocation(Vector3 theNewPos, GameObject * go, float speed);
	bool ReachedLocation(Vector3 thePosReached, GameObject * go);

	void DrawSquare(int length);
	void DrawLineCube(int x, int y, int width, int height);
	void DrawCubeTextured(int x, int y, int width, int height, Vector3 color, float alphaColor, int TexID);

	void DrawLegend();

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

	//Alarm color. Arlam dir controls which whether alarmcolor goes up or down. Bool soundplaying
	//is used for playing only one sound, or the program will play it multiple times.
	bool Alarm;
	float AlarmColor;
	int AlarmDir;
	bool SoundPlaying;

	int robberCount;

	GameObject* FetchGO();
	GameObject* exit;
	GameObject* money;
	//Vector3 WayPoints[5];
	WayPoint thePoints[5];

	std::vector<GameObject *> robbers;

	TextureImage textures[GameObject::GAMEOBJECT_TYPE::GO_TOTAL];
	bool LoadTGA(TextureImage *texture, char *filename);

	//Variables and things regarding to sound
	irrklang::ISoundEngine* theSoundEngine;
};