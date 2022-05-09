#include "stdafx.h"
#include "glutMainWindow.h"
#include "globals.h"

int mainwndSize_x = 800;
int	mainwndSize_y = 450;
int mainwndPosition_x = 100;
int mainwndPosition_y = 100;

GLVoxelModel* Wndmodel = nullptr;

GLfloat Scene_center[3] = { 0.0f, 0.0f, 0.0f };
GLfloat Scene_maxBndLen = 0.0f;

GLdouble Camera_eye[3] = { 0.0f, 0.0f, 0.0f };
GLdouble Camera_center[3] = { 0.0f, 0.0f, 0.0f };
GLdouble Camera_up[3] = { 0.0f, 1.0f, 0.0f };

int Create_glutMainWnd(char tittle[], GLVoxelModel* ptr_vmodel)
{
	if (ptr_vmodel == nullptr)
	{ 
		return 0; 
	}

	///	Setup window size, return window id
	glutInitWindowSize(mainwndSize_x, mainwndSize_y);
	glutInitWindowPosition(mainwndPosition_x, mainwndPosition_y);
	mainWndID = glutCreateWindow(tittle);

	/// Loading scene from GLVoxelmodel
	Wndmodel = ptr_vmodel;
	Scene_center[0] = Wndmodel->center[0];
	Scene_center[1] = Wndmodel->center[1];
	Scene_center[2] = Wndmodel->center[2];
	Scene_maxBndLen = Wndmodel->maxBndLen;
	Camera_eye[0] = 0.0f;
	Camera_eye[1] = 0.0f;
	Camera_eye[2] = Scene_maxBndLen*2.4f;

	Initial_Scene_mainwnd();

	///	Register function call
	glutDisplayFunc(Render_Scene_mainwnd);
	glutReshapeFunc(Reshape_mainwnd);

	///	Tarckball controller
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(mousewheel);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(40, timf, 0);

	///	Start trackball controll 
	tbInit(GLUT_LEFT_BUTTON);
	tbAnimate(GL_TRUE);

	return mainWndID;
}

void Initial_Scene_mainwnd(){
	/// background color
	glClearColor(0.75f, 0.75f, 0.75f, 0.75f);

	/// camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//gluLookAt(0.0f, 0.0f, maxBndLen*2.4f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//glTranslatef(-center[0], -center[1], -center[2]);
	gluLookAt(Camera_eye[0], Camera_eye[1], Camera_eye[2], Camera_center[0], Camera_center[1], Camera_center[2], Camera_up[0], Camera_up[1], Camera_up[2]);
	glTranslatef(-Scene_center[0], -Scene_center[1], -Scene_center[2]);

	///	lighting and material
	GLfloat sun_light_position[] = { 0,0,1,0 };
	GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glEnable(GL_NORMALIZE);
}

void Render_Scene_mainwnd(void){
	/// 1.Open depth_test,then clear color buffer and depth buffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();
	glPushMatrix();

	glTranslatef(Scene_center[0], Scene_center[1], Scene_center[2]);
	tbMatrix();
	glTranslatef(-Scene_center[0], -Scene_center[1], -Scene_center[2]);

	/// 3.Draw everything
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	Draw_axes(Scene_center[0], Scene_center[1], Scene_center[2], Scene_maxBndLen*1.3);
	glvmDraw(Wndmodel); //draw voxel model
	//glvmDrawWithImmediateMode(Wndmodel);

	//glPushMatrix();
	//	glTranslatef(teapot_movement_x, teapot_movement_y, teapot_movement_z); //teapot movement
	//	glTranslatef(center[0], center[1] + maxBndLen*0.7, center[2]);//set teapot initial position
	//	glutSolidTeapot(1.5);
	//glPopMatrix();
	//glvmDrawWithImmediateMode(pvmodel); //draw with opengl ImmediateMode(more time cost)
	//perspectiveDrawCoordinateReference(); //劃出世界座標

	glPopMatrix();

	// 3.Swap buffers
	glutSwapBuffers();
}

void Reshape_mainwnd(int width, int high){
	tbReshape(width, high);

	glViewport(0, 0, width, high);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, float(width) / high, Scene_maxBndLen*0.1f, Scene_maxBndLen*10.0f);
}


void mouse(int button, int state, int x, int y)
{
	tbMouse(button, state, x, y);
}

void motion(int x, int y)
{
	tbMotion(x, y);

	glutSetWindow(subWndID);
	glutPostRedisplay();
	glutSetWindow(mainWndID);
	glutPostRedisplay();
}

void mousewheel(int wheel, int direction, int x, int y)
{
	mouse(GLUT_MIDDLE_BUTTON, GLUT_DOWN, x, y);
	motion(x + 3.0f*direction, y);
}

void keyboard(unsigned char key, int x, int y)
{
	int i;
	switch (key) {
		// press "space" key to let camera back to initial position
	case ' ':
		//if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
		glutSetWindow(mainWndID);
		tbInit(GLUT_LEFT_BUTTON);
		tbAnimate(GL_TRUE);
		glutPostRedisplay();
		//}
		break;
	}
}

void timf(int value)
{
	glutSetWindow(subWndID);
	glutPostRedisplay();

	glutSetWindow(mainWndID);
	glutPostRedisplay();

	glutTimerFunc(50, timf, 0.0);
}