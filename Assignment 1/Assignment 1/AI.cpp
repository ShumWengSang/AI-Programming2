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

	SoundPlaying = false;
	AlarmColor = 1;
	AlarmDir = -1;

	theSoundEngine = irrklang::createIrrKlangDevice();
	if (!theSoundEngine)
		return;		// Error starting up the sound engine

	LoadTGA(&textures[GameObject::GAMEOBJECT_TYPE::GO_ROBBER], "images/robber.tga");
	LoadTGA(&textures[GameObject::GAMEOBJECT_TYPE::GO_POLICE], "images/police.tga");
	LoadTGA(&textures[GameObject::GAMEOBJECT_TYPE::GO_MONEY], "images/money.tga");
	LoadTGA(&textures[GameObject::GAMEOBJECT_TYPE::GO_EXIT], "images/exit.tga");

	//WayPoints[0] = Vector3(90, 60, 0);
	//WayPoints[1] = Vector3(110, 60, 0);
	//WayPoints[2] = Vector3(110, 40, 0);
	//WayPoints[3] = Vector3(90, 40, 0);
	//WayPoints[4] = Vector3(50, 50, 0);
	
	thePoints[0].thePoint = Vector3(90, 80, 0);
	thePoints[1].thePoint = Vector3(110, 80, 0);
	thePoints[2].thePoint = Vector3(110, 20, 0);
	thePoints[3].thePoint = Vector3(90, 20, 0);
	thePoints[4].thePoint = Vector3(65, 50, 0);

	thePoints[0].nextPoint = &thePoints[1];
	thePoints[1].nextPoint = &thePoints[2];
	thePoints[2].nextPoint = &thePoints[3];
	thePoints[3].nextPoint = &thePoints[0];
	thePoints[4].nextPoint = &thePoints[0];

	for (int i = 0; i < 5; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WAYPOINTS;
		go->pos.Set(thePoints[i].thePoint);
		go->color.Set(0.5, 0.5, 1);
		//go->scale.Set(5, 5, 5);
	}

	Alarm = false;
	
	moneypile = FetchGO();
	moneypile->type = GameObject::GO_MONEY;
	moneypile->pos.Set(40, 30, 0);
	moneypile->scale.Set(1, 1, 1);

	moneypile2 = FetchGO();
	moneypile2->type = GameObject::GO_MONEY;
	moneypile2->pos.Set(40, 60, 0);
	moneypile2->scale.Set(1, 1, 1);

	exit = FetchGO();
	exit->type = GameObject::GO_EXIT;
	exit->pos.Set(6, 50, 0);


	go = FetchGO();
	go->type = GameObject::GO_ROBBER;
	go->pos.Set(30, 60, 0);
	go->Target = rand() % 2;
	go->CurrentState = GameObject::STATES::STEALING;
	
	go = FetchGO();
	go->type = GameObject::GO_ROBBER;
	go->pos.Set(30, 40, 0);	
	go->Target = rand() % 2;
	go->CurrentState = GameObject::STATES::STEALING;

	go = FetchGO();
	go->type = GameObject::GO_ROBBER;
	go->pos.Set(30, 20, 0);
	go->Target = rand() % 2;
	go->CurrentState = GameObject::STATES::STEALING;

	go = FetchGO();
	go->type = GameObject::GO_ROBBER;
	go->pos.Set(40, 20, 0);
	go->Target = rand() % 2;
	go->CurrentState = GameObject::STATES::STEALING;

	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(90, 40, 0);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->TargetLocked = false;

	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(90, 50, 0);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->TargetLocked = false;

	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(90, 60, 0);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->TargetLocked = false;


	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(110, 60, 0);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->TargetLocked = false;

	go = FetchGO();
	go->type = GameObject::GO_POLICE;
	go->pos.Set(110, 50, 0);
	go->vel.Set(0, 15, 0);
	go->CurrentState = GameObject::STATES::PATROLLING;
	go->TargetLocked = false;

	robberCount = 0;

	for (std::vector<GameObject *>::iterator iter2 = m_goList.begin(); iter2 != m_goList.end(); ++iter2)
	{
		if ((*iter2)->type == GameObject::GO_ROBBER) {
			robbers.push_back((*iter2));
			robberCount++;
		}
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
	if (theSoundEngine != NULL)
		theSoundEngine->drop();
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
	, theSoundEngine(NULL)
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
	case 'm':
		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (go->active)
			{
				if (go->type == GameObject::GO_ROBBER)
					go->money = 100;
			}
		}
		break;
	case ' ':
		Alarm = true;
		break;
	case 'r':
		while (m_goList.size() > 0)
		{
			GameObject *go = m_goList.back();
			delete go;
			m_goList.pop_back();
		}
		Init();
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
	if (Alarm)
	{
		if (!SoundPlaying)
		{
			SoundPlaying = true;
			theSoundEngine->play2D("Alarm.wav");
		}
		//The below controls the color of the alarm
		AlarmColor += 0.8 * AlarmDir * dt;
		if (AlarmColor > 1)
		{
			AlarmColor = 1;
			AlarmDir = -AlarmDir;
		}
		if (AlarmColor < 0)
		{
			AlarmColor = 0;
			AlarmDir = -AlarmDir;
		}

		//std::cout << "Alarm is blaring." << std::endl;
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
				if (go->CurrentState != GameObject::STATES::CAUGHT)
				{
					if (go->money < 100) {
						go->CurrentState = GameObject::STATES::STEALING;
						if (go->TargetLocked == false) {
							go->Target = rand() % 2;
							std::cout << go->Target;
							go->TargetLocked = true;
						}
					}
					else {
						go->CurrentState = GameObject::STATES::RUNNING;
						go->TargetLocked = false;
					}
				}

				switch (go->CurrentState) 
				{
				case GameObject::STATES::STEALING:
					//go->money += 1;
					if (go->Target == 0)
						GotoLocation(moneypile->pos, go, 15);
					else
						GotoLocation(moneypile2->pos, go, 15);
					if (ReachedLocation(moneypile->pos, go) || ReachedLocation(moneypile2->pos, go))
						go->money++;

					break;

				case GameObject::STATES::RUNNING:
					//go->vel.Set();
					//Vector3 temp = (exit->pos - go->pos).Normalized;
					GotoLocation(exit->pos, go, 15.f - 5);
					if ((go->pos - exit->pos).Length() < 5) {
						if (Alarm) {
							go->active = false;
							robberCount--;
						}
						else {
							go->money = 0;

						}
					}
					break;
				case GameObject::STATES::CAUGHT:
					go->vel.SetZero();
					break;
				}
				break;

			case GameObject::GO_POLICE:

				switch (go->CurrentState)
				{
				case GameObject::STATES::PATROLLING:

					for (int i = 0; i < 4; i++)
					{
						if (ReachedLocation(thePoints[i].thePoint, go))
						{
							GotoLocation(thePoints[i].nextPoint->thePoint, go, 15);
						}
					}

					if (Alarm)
						go->CurrentState = GameObject::STATES::MOVING;
					break;
				case GameObject::STATES::MOVING:
					GotoLocation(thePoints[4].thePoint, go, 15);
					go->color.Set(1, 0, 0);
					if (ReachedLocation(thePoints[4].thePoint, go))
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
					if (!go->TargetLocked && robberCount > 0)
					{
						go->Target = rand() % robbers.size();
						go->TargetLocked = true;
					}

					if (robberCount == 0) {
						GotoLocation(exit->pos, go, 15);
						go->TargetLocked = false;
					}
					else
					{
						GotoLocation(robbers[go->Target]->pos, go, 15);
						go->TargetPos = robbers[go->Target]->pos/* + robbers[go->Target]->vel*/;

						if ((go->pos - robbers[go->Target]->pos).Length() < 5) {
							if (robbers[go->Target]->active) {
								robbers[go->Target]->CurrentState = GameObject::STATES::CAUGHT;
								go->TargetLocked = false;
								robberCount--;
							}
						}
					}

					if (ReachedLocation(exit->pos, go))
						go->active = false;

					
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

	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//DrawLegend();
	if (Alarm)
	{
		//if (theSoundEngine->isCurrentlyPlaying(*theSoundEngine))
		//{
		//	theSoundEngine->drop();
		//}
		DrawCubeTextured(0, 0, m_worldSizeX, m_worldSizeY, Vector3(AlarmColor, 0, 0), 1, 0);
	}
	DrawCubeTextured(30, 5, 5, 80, Vector3(1, 1, 1), 1, 0);
	if (Alarm)
	{

		DrawCubeTextured(30, 20, 5, 20, Vector3 (0,0,0) ,1,0);
	}

	DrawLineCube(6, 10, 120, 80);
	DrawLegend();

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			DrawObject(go);
		}
	}



	glColor3f(1.0, 1.0, 1.0);
	char temp[64];
	sprintf_s(temp, "Robbers: %d", robberCount);
	RenderStringOnScreen(0, 94, temp);

	sprintf_s(temp, "Robbers: %d", robberCount);
	RenderStringOnScreen(0, 94, temp);

	sprintf_s(temp, "Police");
	RenderStringOnScreen(32, 94, temp);

	sprintf_s(temp, "Robbers");
	RenderStringOnScreen(52, 94, temp);

	sprintf_s(temp, "Money");
	RenderStringOnScreen(72, 94, temp);

	//sprintf_s(temp, "Simulation Speed: %.1f", m_speed);
	//RenderStringOnScreen(0, 90, temp);
	//sprintf_s(temp, "money: %.1f", m_ship->money);
	//RenderStringOnScreen(0, 86, temp);

	////Exercise 4: Render m_lives and m_score
	//sprintf_s(temp, "Lives: %d", m_lives);
	//RenderStringOnScreen(100, 94, temp);
	//sprintf_s(temp, "Score: %d", m_score);
	//RenderStringOnScreen(100, 90, temp);

	glutSwapBuffers();
	glutPostRedisplay();

	//glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();
			/*glColor3f(1, 0, 0);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(1, 10, 10);*/

			glColor3f(1, 1, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glBindTexture(GL_TEXTURE_2D, textures[GameObject::GAMEOBJECT_TYPE::GO_ROBBER].texID);
			DrawSquare(4);

			glDisable(GL_BLEND);

			if (go->CurrentState != GameObject::STATES::CAUGHT)
			{
				glTranslatef(0, 4, 0); //Bar Outline
				glColor3f(0, 1, 0);
				glBegin(GL_LINES);
					glVertex3f(-3, -1, 0);	glVertex3f(3, -1, 0);
					glVertex3f(3, 1, 0);	glVertex3f(-3, 1, 0);
					glVertex3f(3, -1, 0);	glVertex3f(3, 1, 0);
					glVertex3f(-3, -1, 0);	glVertex3f(-3, 1, 0);
				glEnd();
			

				glBegin(GL_QUADS);
					glVertex3f(-3, 1, 0);
					glVertex3f(-3, -1, 0);
					glVertex3f(-3 + 0.06 * go->money, -1, 0);
					glVertex3f(-3 + 0.06 * go->money, 1, 0);
				glEnd();
			}
		glPopMatrix();
		
		break;
	case GameObject::GO_POLICE:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/*glPushMatrix();
			glColor3f(0, 0, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();*/

		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glBindTexture(GL_TEXTURE_2D, textures[GameObject::GAMEOBJECT_TYPE::GO_POLICE].texID);
			DrawSquare(4);
		glPopMatrix();

		glDisable(GL_BLEND);
		break;
	case GameObject::GO_WAYPOINTS:
		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();
		break;
	case GameObject::GO_MONEY:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glBindTexture(GL_TEXTURE_2D, textures[GameObject::GAMEOBJECT_TYPE::GO_MONEY].texID);
			DrawSquare(10);
		glPopMatrix();

		glDisable(GL_BLEND);
		break;
	case GameObject::GO_EXIT:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(go->pos.x, go->pos.y, go->pos.z);
			glScalef(go->scale.x, go->scale.y, go->scale.z);
			glRotatef(90, 0, 0, 1);
			glBindTexture(GL_TEXTURE_2D, textures[GameObject::GAMEOBJECT_TYPE::GO_EXIT].texID);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0);		glVertex2f(-9, -3);
				glTexCoord2f(1, 0);		glVertex2f(9, -3);
				glTexCoord2f(1, 1);		glVertex2f(9, 3);
				glTexCoord2f(0, 1);		glVertex2f(-9, 3);
			glEnd();
		//glutSolidCube(2);
		glPopMatrix();

		glDisable(GL_BLEND);
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

void AI::DrawSquare(int length)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex2f(-length * 0.5, -length * 0.5);
		glTexCoord2f(1, 0);		glVertex2f(length * 0.5, -length * 0.5);
		glTexCoord2f(1, 1);		glVertex2f(length * 0.5, length * 0.5);
		glTexCoord2f(0, 1);		glVertex2f(-length * 0.5, length * 0.5);
	glEnd();
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

void AI::DrawCubeTextured(int x, int y, int width, int height, Vector3 color, float alphaColor, int TexID)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	//glBindTexture();
	glColor4f(color.x, color.y, color.z, alphaColor);
	glTranslatef(x, y, 0);
	glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}

bool AI::LoadTGA(TextureImage *texture, char *filename)			// Loads A TGA File Into Memory
{
	GLubyte		TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;										// Temporary Variable
	GLuint		type = GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)

	FILE *file = fopen(filename, "rb");						// Open The TGA File

	if (file == NULL ||										// Does File Even Exist?
		fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||	// Does The Header Match What We Want?
		fread(header, 1, sizeof(header), file) != sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL)									// Did The File Even Exist? *Added Jim Strong*
			return false;									// Return False
		else
		{
			fclose(file);									// If Anything Failed, Close The File
			return false;									// Return False
		}
	}

	texture->width = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)

	if (texture->width <= 0 ||								// Is The Width Less Than Or Equal To Zero
		texture->height <= 0 ||								// Is The Height Less Than Or Equal To Zero
		(header[4] != 24 && header[4] != 32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

	texture->bpp = header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel = texture->bpp / 8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize = texture->width*texture->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture->imageData = (GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if (texture->imageData == NULL ||							// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file) != imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if (texture->imageData != NULL)						// Was Image Data Loaded
			free(texture->imageData);						// If So, Release The Image Data

		fclose(file);										// Close The File
		return false;										// Return False
	}

	for (GLuint i = 0; i<int(imageSize); i += bytesPerPixel)		// Loop Through The Image Data
	{														// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp = texture->imageData[i];							// Temporarily Store The Value At Image Data 'i'
		texture->imageData[i] = texture->imageData[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
		texture->imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose(file);											// Close The File

	// Build A Texture From The Data
	glGenTextures(1, &texture[0].texID);					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture[0].texID);			// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered

	if (texture[0].bpp == 24)									// Was The TGA 24 Bits
	{
		type = GL_RGB;										// If So Set The 'type' To GL_RGB
	}

	texture->frameNumbers = texture->width / texture->height;

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture[0].imageData);

	return true;											// Texture Building Went Ok, Return True
}

void AI::DrawLegend()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_TEXTURE_2D);
	//Start Drawing of Police
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(30,95,0);
	glBindTexture(GL_TEXTURE_2D, textures[GameObject::GAMEOBJECT_TYPE::GO_POLICE].texID);
	DrawSquare(4);
	glPopMatrix();
	//Stop drawing of Police

	////Start drawing of robbers
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(50, 95, 0);
	glBindTexture(GL_TEXTURE_2D, textures[GameObject::GAMEOBJECT_TYPE::GO_ROBBER].texID);
	DrawSquare(4);
	glPopMatrix();
	////End drawing of robbers

	////Start draw money
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(70,95,0);
	glBindTexture(GL_TEXTURE_2D, textures[GameObject::GAMEOBJECT_TYPE::GO_MONEY].texID);
	DrawSquare(4);
	glPopMatrix();
	//End drawing of money

	//All strings are drawn last at glutdisplay();
	glDisable(GL_BLEND);
	//End draw money
}