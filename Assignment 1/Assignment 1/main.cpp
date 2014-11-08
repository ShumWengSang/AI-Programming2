#include "AI.h"

void onReshape(GLsizei w, GLsizei h)
{
	AI::GetInstance()->GlutReshape(w, h);
}

void onMouseClick(int button, int state, int x, int y)
{
	AI::GetInstance()->GlutMouseClick(button, state, x, y);
}

void onMouseMove(int x, int y)
{
	AI::GetInstance()->GlutMouseMove(x, y);
}

void onKeyboard(unsigned char key, int x, int y)
{
	AI::GetInstance()->GlutKeyboard(key, x, y);
}

void onSpecialKey(int key, int x, int y)
{
	AI::GetInstance()->GlutSpecialKey(key, x, y);
}

void idle()
{
	AI::GetInstance()->GlutIdle();
}

void display()
{
	AI::GetInstance()->GlutDisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("AI game");
	glutReshapeFunc(onReshape);
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseMove);
	glutKeyboardFunc(onKeyboard);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutSpecialFunc(onSpecialKey);

	AI::GetInstance()->SetupRenderContext();
	AI::GetInstance()->Init();
	glutMainLoop();
	AI::GetInstance()->Exit();
	return 0;
}