#include "stdafx.h"
#include "glutAnimateWindow.h"
#include "globals.h"

int AnimateWndSize_x = 800;
int	AnimateWndSize_y = 450;
int AnimateWndPosition_x = 100;
int AnimateWndPosition_y = 100;

Animate* ptr_wndanimate = nullptr;

GLfloat AnimateScene_center[3] = { 0.0f, 0.0f, 0.0f };
GLfloat AnimateScene_maxBndLen = 0.0f;

GLdouble AnimateCamera_eye[3] = { 0.0f, 0.0f, 0.0f };
GLdouble AnimateCamera_center[3] = { 0.0f, 0.0f, 0.0f };
GLdouble AnimateCamera_up[3] = { 0.0f, 1.0f, 0.0f };

int Create_glutAnimateWnd(char tittle[], Animate* ptr_animate)
{
	if (ptr_animate == nullptr)
	{
		return 0;
	}

	///	Setup window size, return window id
	glutInitWindowSize(AnimateWndSize_x, AnimateWndSize_y);
	glutInitWindowPosition(AnimateWndPosition_x, AnimateWndPosition_y);
	AnimateWndID = glutCreateWindow(tittle);

	/// Loading scene from GLVoxelmodel
	ptr_wndanimate = ptr_animate;

	AnimateScene_center[0] = 0;
	AnimateScene_center[1] = 0;
	AnimateScene_center[2] = 0;
	AnimateScene_maxBndLen = 25;
	AnimateCamera_eye[0] = 0.0f;
	AnimateCamera_eye[1] = 0.0f;
	AnimateCamera_eye[2] = AnimateScene_maxBndLen*2.4f;

	Initial_Scene_AnimateWnd();

	///	Register function call
	glutDisplayFunc(Render_Scene_AnimateWnd);
	glutReshapeFunc(Reshape_AnimateWnd);

	///	Tarckball controller
	glutMouseFunc(AnimateWnd_mouse);
	glutMotionFunc(AnimateWnd_motion);
	glutMouseWheelFunc(AnimateWnd_mousewheel);
	glutKeyboardFunc(AnimateWnd_keyboard);
	glutTimerFunc(40, AnimateWnd_timf, 0);

	///	Start trackball controll 
	tbInit(GLUT_LEFT_BUTTON);
	tbAnimate(GL_TRUE);

	return AnimateWndID;
}

void Initial_Scene_AnimateWnd()
{
	/// background color
	glClearColor(0.75f, 0.75f, 0.75f, 0.75f);

	/// camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//gluLookAt(0.0f, 0.0f, maxBndLen*2.4f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//glTranslatef(-center[0], -center[1], -center[2]);
	gluLookAt(AnimateCamera_eye[0], AnimateCamera_eye[1], AnimateCamera_eye[2], AnimateCamera_center[0], AnimateCamera_center[1], AnimateCamera_center[2], AnimateCamera_up[0], AnimateCamera_up[1], AnimateCamera_up[2]);
	glTranslatef(-AnimateScene_center[0], -AnimateScene_center[1], -AnimateScene_center[2]);

	///	lighting and material
	GLfloat sun_light_position[] = { AnimateScene_center[0], AnimateScene_center[1], AnimateScene_center[2] + AnimateScene_maxBndLen*0.7, 0.0f };
	GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

void Render_Scene_AnimateWnd(void)
{
	/// 1.Open depth_test,then clear color buffer and depth buffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();
	glPushMatrix();

	glTranslatef(AnimateScene_center[0], AnimateScene_center[1], AnimateScene_center[2]);
	tbMatrix();
	glTranslatef(-AnimateScene_center[0], -AnimateScene_center[1], -AnimateScene_center[2]);

	/// 3.Draw everything
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	Draw_axes(AnimateScene_center[0], AnimateScene_center[1], AnimateScene_center[2], AnimateScene_maxBndLen*1.3);

	for (int i = 0; i < ptr_wndanimate->trajectory.size(); i++)
	{
		glPushMatrix();
		glTranslatef(ptr_wndanimate->trajectory[i].movement[0], ptr_wndanimate->trajectory[i].movement[1], ptr_wndanimate->trajectory[i].movement[2]);
		glvmDraw(ptr_wndanimate->models[ptr_wndanimate->trajectory[i].to_model]);
		glPopMatrix();
	}

	glPopMatrix();

	// 3.Swap buffers
	glutSwapBuffers();
}

void Reshape_AnimateWnd(int width, int high)
{
	glutSetWindow(AnimateWndID);
	tbReshape(width, high);

	glViewport(0, 0, width, high);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, float(width) / high, AnimateScene_maxBndLen*0.1f, AnimateScene_maxBndLen*10.0f);
}

void AnimateWnd_mouse(int button, int state, int x, int y)
{
	tbMouse(button, state, x, y);
}

void AnimateWnd_motion(int x, int y)
{
	tbMotion(x, y);

	glutSetWindow(AnimateWndID);
	glutPostRedisplay();
}

void AnimateWnd_mousewheel(int wheel, int direction, int x, int y)
{
	AnimateWnd_mouse(GLUT_MIDDLE_BUTTON, GLUT_DOWN, x, y);
	AnimateWnd_motion(x + 3.0f*direction, y);
}

void AnimateWnd_keyboard(unsigned char key, int x, int y)
{
	int i;
	switch (key) {
		// press "space" key to let camera back to initial position
	case ' ':
		//if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
		glutSetWindow(AnimateWndID);
		tbInit(GLUT_LEFT_BUTTON);
		tbAnimate(GL_TRUE);
		glutPostRedisplay();
		//}
		break;
	}
}

void AnimateWnd_timf(int value)
{
	glutSetWindow(AnimateWndID);
	glutPostRedisplay();

	for (int i = 0; i < ptr_wndanimate->trajectory.size(); i++)
	{
		if (ptr_wndanimate->trajectory[i].movement != ptr_wndanimate->trajectory[i].end)
		{
			ptr_wndanimate->trajectory[i].movement[0] += ptr_wndanimate->trajectory[i].direction[0] * 0.1;
			ptr_wndanimate->trajectory[i].movement[1] += ptr_wndanimate->trajectory[i].direction[1] * 0.1;
			ptr_wndanimate->trajectory[i].movement[2] += ptr_wndanimate->trajectory[i].direction[2] * 0.1;
		}
	}

	glutTimerFunc(50, AnimateWnd_timf, 0.0);
}