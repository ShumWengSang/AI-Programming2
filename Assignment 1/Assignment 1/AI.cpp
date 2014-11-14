#include "AI.h"

inline const char * const BooltoString(bool b)
{
	if (b)
		return "true";
	return "false";
}

void AI::Init()
{
	srand(time(NULL));
	GameObject * go;
	Math::InitRNG();


	WayPoints[0] = Vector3(80, 70, 0);
	WayPoints[1] = Vector3(100, 70, 0);
	WayPoints[2] = Vector3(100, 40, 0);
	WayPoints[3] = Vector3(80, 40, 0);
	WayPoints[4] = Vector3(50, 50, 0);
	


	for (int i = 0; i < 5; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WAYPOINTS;
		go->pos.Set(WayPoints[i]);
		go->color.Set(0.5, 0.5, 1);
		//go->scale.Set(5, 5, 5);
	}

	Alarm = false;
	


	go = FetchGO();
	go->type = GameObject::GO_ROBBER;
	go->pos.Set(70, 50, 0);
	go->CurrentState = GameObject::STATES::STEALING;
	
	go = FetchGO();
	go->type = GameObject::GO_ROBBER;
	go->pos.Set(70, 20, 0);
	go->CurrentState = GameObject::STATES::STEALING;

	go = FetchGO();
	go->type = GameObject::GO_ROBBER;
	go->pos.Set(70, 40, 0);
	go->CurrentState = GameObject::STATES::STEALING;

	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(80, 70, 0);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->TargetLocked = false;


	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(80, 60, 0);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->TargetLocked = false;

	exit = FetchGO();
	exit->type = GameObject::GO_EXIT;
	exit->pos.Set(20, 60, 0);

	for (std::vector<GameObject *>::iterator iter2 = m_goList.begin(); iter2 != m_goList.end(); ++iter2)
	{
		if ((*iter2)->type == GameObject::GO_ROBBER)
			robbers.push_back((*iter2));
	}
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
	case ' ':
		Alarm = true;
		break;
	}
}

void AI::GlutSpecialKey(int key, int x, int y)
{
	//Exercise 5: update m_force with the following keys
	switch (key)
	{
	case GLUT_KEY_LEFT:
		exit->pos.x -= 5;
		break;
	case GLUT_KEY_UP:
		exit->pos.y += 5;
		break;
	case GLUT_KEY_RIGHT:
		exit->pos.x += 5;
		break;
	case GLUT_KEY_DOWN:
		exit->pos.y -= 5;
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
			case GameObject::GO_ROBBER:
				if (go->mass < 100) 
					go->CurrentState = GameObject::STATES::STEALING;
				else 
					go->CurrentState = GameObject::STATES::RUNNING;

				switch (go->CurrentState) 
				{
				case GameObject::STATES::STEALING:
					go->mass += 1;
					break;
				case GameObject::STATES::RUNNING:
					//go->vel.Set();
					//Vector3 temp = (exit->pos - go->pos).Normalized;
					go->vel = (exit->pos - go->pos).Normalized() * 15;
					break;
				}
				break;
			case GameObject::GO_POLICE:

				switch (go->CurrentState)
				{
				case GameObject::STATES::PATROLLING:

					if (ReachedLocation(WayPoints[0], go))
					{
						GotoLocation(WayPoints[1], go, 15);
					}
					if (ReachedLocation(WayPoints[1], go))
					{
						GotoLocation(WayPoints[2], go, 15);
					}
					if (ReachedLocation(WayPoints[2], go))
					{
						GotoLocation(WayPoints[3], go, 15);
					}
					if (ReachedLocation(WayPoints[3], go))
					{
						GotoLocation(WayPoints[0], go, 15);
					}

					if (Alarm)
						go->CurrentState = GameObject::STATES::MOVING;
					break;
				case GameObject::STATES::MOVING:
					GotoLocation(WayPoints[4], go, 15);
					go->color.Set(1, 0, 0);
					if (ReachedLocation(WayPoints[4], go))
					{
						go->vel.Set(0, 0, 0);
						go->CurrentState = GameObject::STATES::CHASING;
					}
					break;
				case GameObject::STATES::CHASING:
					
					if (go->TargetLocked)
					{
						//if (!(robbers[go->Target]->pos == go->TargetPos))
						if (!(robbers[go->Target]->active))
						{

							go->TargetLocked = false;
							std::cout << "Target change" << std::endl;
						}
					}

					if (!go->TargetLocked)
					{
						go->Target = rand() % robbers.size();
						go->TargetLocked = true;
					}
					GotoLocation(robbers[go->Target]->pos, go, 15);
					go->TargetPos = robbers[go->Target]->pos/* + robbers[go->Target]->vel*/;

					break;
				}
				break;
			}
		}
	}
}

void AI::GlutDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1, 1, 1);

	DrawLineCube(10, 10, 120, 80);
	

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			DrawObject(go);
		}
	}

	//glColor3f(1.0, 1.0, 1.0);
	//char temp[64];
	//sprintf_s(temp, "FPS: %.2f", m_fps);
	//RenderStringOnScreen(0, 94, temp);
	//sprintf_s(temp, "Simulation Speed: %.1f", m_speed);
	//RenderStringOnScreen(0, 90, temp);
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

void AI::GotoLocation(Vector3 theNewPos, GameObject * go, float speed)
{
	Vector3 TheDirection(theNewPos - go->pos);
	TheDirection.Normalize();
	TheDirection *= speed;
	go->vel = TheDirection;
}

bool AI::ReachedLocation(Vector3 thePosReached, GameObject * go)
{
	return 1 > (thePosReached - go->pos).Length();
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
			
			glTranslatef(0, 3, 0); //Bar Outline
			glColor3f(0, 1, 0);
			glBegin(GL_LINES);
				glVertex3f(-3, -1, 0);
				glVertex3f(3, -1, 0);

				glVertex3f(3, 1, 0);
				glVertex3f(-3, 1, 0);

				glVertex3f(3, -1, 0);
				glVertex3f(3, 1, 0);

				glVertex3f(-3, -1, 0);
				glVertex3f(-3, 1, 0);
			glEnd();

			glBegin(GL_QUADS);
				glVertex3f(-3, 1, 0);
				glVertex3f(-3, -1, 0);
				//glVertex3f(2, -1, 0);
				//glVertex3f(2, 1, 0);
				glVertex3f(-3 + 0.06 * go->mass, -1, 0);
				glVertex3f(-3 + 0.06 * go->mass, 1, 0);
			glEnd();
		glPopMatrix();
		break;
	case GameObject::GO_POLICE:
		glPushMatrix();
			glColor3f(0, 0, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();
		break;
	case GameObject::GO_WAYPOINTS:
		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();
		break;
	case GameObject::GO_EXIT:
		glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(go->pos.x, go->pos.y, go->pos.z);
		glScalef(go->scale.x, go->scale.y, go->scale.z);
		glutSolidCube(2);
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

void AI::DrawLineCube(int x, int y, int width, int height)
{
	glLineWidth(5);
	glBegin(GL_LINE_STRIP);
	{
	
		glColor3f(1, 1, 1);
		glVertex2f(x, y);
		glVertex2f(x, y + height);
		glVertex2f(x + width, y + height);
		glVertex2f(x + width, y);
		glVertex2f(x, y);
		//glLineWidth(1);
	}
	glEnd();
	glLineWidth(1);
}

void AI::DrawCubeTextured(int x, int y, int size)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	//glBindTexture();
	glColor3f(0, 0, 1);
	glTranslatef(x, y, 0);
	glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + size, y);
		glVertex2f(x + size, y + size);
		glVertex2f(x, y + size);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}