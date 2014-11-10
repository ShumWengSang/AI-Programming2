#include "AI.h"

inline const char * const BooltoString(bool b)
{
	if (b)
		return "true";
	return "false";
}

void AI::Init()
{
	Alarm = false;
	Math::InitRNG();

	GameObject * go;
	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(80, 50, 0);
	go->scale.Set(1, 1, 1);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->color.Set(0, 0, 1);
}

void AI::Exit()
{
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
}

GameObject* AI::FetchGO()
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = *it;
		if (!go->active)
		{
			go->active = true;
			return go;
		}
	}
	GameObject *go = new GameObject(GameObject::GO_NONE);
	go->active = true;
	m_goList.push_back(go);
	return go;
}

AI::AI()
	: m_fps(30)
	, m_speed(1)
	, m_worldSizeY(100)
	, m_worldSizeX(m_worldSizeY)
{
}

AI::~AI()
{
	Exit();
}

void AI::SetupRenderContext()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);	// set background colour to black
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

void AI::GlutReshape(GLsizei w, GLsizei h)
{
	if (h == 0)
		h = 1;

	float ratio = (float)w / h;
	m_worldSizeX = m_worldSizeY * ratio;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, m_worldSizeX, 0, m_worldSizeY);
	glMatrixMode(GL_MODELVIEW);
}

void AI::GlutMouseClick(int button, int state, int x, int y)
{
}

void AI::GlutMouseMove(int x, int y)
{
}

void AI::GlutKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	}
}

void AI::GlutSpecialKey(int key, int x, int y)
{
	//Exercise 5: update m_force with the following keys
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Alarm = true;
		break;
	case GLUT_KEY_UP:
		break;
	case GLUT_KEY_RIGHT:
		break;
	case GLUT_KEY_DOWN:
		break;
	}
}

void AI::GlutIdle()
{
	static int frame = 0;
	static int lastTime = glutGet(GLUT_ELAPSED_TIME);
	++frame;
	int time = glutGet(GLUT_ELAPSED_TIME);
	float dt = (time - lastTime) / 1000.f;
	lastTime = time;

	static int lastFPSTime = glutGet(GLUT_ELAPSED_TIME);
	if (time - lastFPSTime > 1000)
	{
		m_fps = frame * 1000.f / (time - lastFPSTime);
		lastFPSTime = time;
		frame = 0;
	}

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			go->pos += go->vel * dt * m_speed;

			GameObject *nearest = NULL;
			for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
			{
				GameObject *other = (GameObject *)*it2;
				if (other->active)
				{

				}
			}

			//Code to handle out of bound game objects

			switch (go->type)
			{
			case GameObject::GO_BULLET:
				break;
			case GameObject::GO_ROBBER:
				break;
			case GameObject::GO_POLICE:

				switch (go->CurrentState)
				{
				case GameObject::STATES::PATROLLING:
					if (go->pos.y >= 70)
					{
						go->vel = Vector3(15, 0, 0);
					}
					if (go->pos.x >= 100)
					{
						go->vel = Vector3(0, -15, 0);
					}
					if (go->pos.y <= 40)
					{
						go->vel = Vector3(-15, 0, 0);
					}
					if (go->pos.x <= 79)
					{
						go->vel = Vector3(0, 15, 0);
					}
					if (Alarm)
						go->CurrentState = GameObject::STATES::MOVING;
					break;
				case GameObject::STATES::MOVING:
					GotoLocation(Vector3(50,50,0),go,15);
					go->color.Set(1, 0, 0);
					break;
				}
				
				break;
			}
		}
	}
}

void AI::GotoLocation(Vector3 theNewPos, GameObject * go, float speed)
{
	Vector3 TheDirection( theNewPos - go->pos);
	TheDirection.Normalize();
	TheDirection *= speed;
	go->vel = TheDirection;
}

void AI::GlutDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1, 1, 1);

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			DrawObject(go);
		}
	}

	//glColor3f(1.0, 1.0, 1.0);
	char temp[64];
	sprintf_s(temp, "Police Position: %.2f %.2f %.2f", m_goList.at(0)->pos.x, m_goList.at(0)->pos.y, m_goList.at(0)->pos.z);
	//Testing for position

	RenderStringOnScreen(0, 94, temp);
	//sprintf_s(temp, "Alarm : %.1b", Alarm);

	RenderStringOnScreen(0, 90, BooltoString(Alarm));
	//sprintf_s(temp, "Mass: %.1f", m_ship->mass);
	//RenderStringOnScreen(0, 86, temp);

	////Exercise 4: Render m_lives and m_score
	//sprintf_s(temp, "Lives: %d", m_lives);
	//RenderStringOnScreen(100, 94, temp);
	//sprintf_s(temp, "Score: %d", m_score);
	//RenderStringOnScreen(100, 90, temp);

	glutSwapBuffers();
	glutPostRedisplay();
}

void AI::DrawObject(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_ROBBER:
		glPushMatrix();
			glColor3f(1, 0, 0);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();
		break;
	case GameObject::GO_POLICE:
		glPushMatrix();
			//glColor3f(0, 0, 1);
			glColor3f(go->color.x, go->color.y, go->color.z);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();
		break;
	case GameObject::GO_BULLET:
		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(0.2, 10, 10);
		glPopMatrix();
		break;
	}
}

void AI::RenderStringOnScreen(float x, float y, const char* quote)
{
	int length = strlen(quote);
	glRasterPos2f(x, y);

	for (int i = 0; i < length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, quote[i]);
	}
}

bool AI::ReachedLocation(Vector3 thePosReached, GameObject * go)
{
	return 1 > (thePosReached - go->pos).Length();
}