#include "stdafx.h"
#include "glutSubWindow.h"
#include "globals.h"

int subWndSize_x = 800;
int	subWndSize_y = 450;
int subWndPosition_x = 100 + 800;
int subWndPosition_y = 100;

GLVoxelModel* subWndmodel = nullptr;

GLfloat subScene_center[3] = { 0.0f, 0.0f, 0.0f };
GLfloat subScene_maxBndLen = 0.0f;

GLdouble subCamera_eye[3] = { 0.0f, 0.0f, 0.0f };
GLdouble subCamera_center[3] = { 0.0f, 0.0f, 0.0f };
GLdouble subCamera_up[3] = { 0.0f, 1.0f, 0.0f };

int Create_glutSubWnd(char tittle[], GLVoxelModel* ptr_vmodel)
{
	if (ptr_vmodel == nullptr)
	{
		return 0;
	}

	///	Setup window size, return window id
	glutInitWindowSize(subWndSize_x, subWndSize_y);
	glutInitWindowPosition(subWndPosition_x, subWndPosition_y);
	subWndID = glutCreateWindow(tittle);

	/// Loading scene from GLVoxelmodel
	subWndmodel = ptr_vmodel;
	subScene_center[0] = subWndmodel->center[0];
	subScene_center[1] = subWndmodel->center[1];
	subScene_center[2] = subWndmodel->center[2];
	subScene_maxBndLen = subWndmodel->maxBndLen;
	subCamera_eye[0] = 0.0f;
	subCamera_eye[1] = 0.0f;
	subCamera_eye[2] = subScene_maxBndLen*2.4f;

	Initial_Scene_Subwnd();

	///	Register function call
	glutDisplayFunc(Render_Scene_Subwnd);
	glutReshapeFunc(Reshape_Subwnd);

	///	Tarckball controller
	glutTimerFunc(40, timf, 0);

	///	Start trackball controll 
	tbInit(GLUT_LEFT_BUTTON);
	tbAnimate(GL_TRUE);

	return subWndID;
}

void Initial_Scene_Subwnd(){
	/// background color
	glClearColor(0.75f, 0.75f, 0.75f, 0.75f);

	/// camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//gluLookAt(0.0f, 0.0f, maxBndLen*2.4f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//glTranslatef(-center[0], -center[1], -center[2]);
	gluLookAt(subCamera_eye[0], subCamera_eye[1], subCamera_eye[2], subCamera_center[0], subCamera_center[1], subCamera_center[2], subCamera_up[0], subCamera_up[1], subCamera_up[2]);
	glTranslatef(-subScene_center[0], -subScene_center[1], -subScene_center[2]);

	///	lighting and material
	//GLfloat sun_light_position[] = { subScene_center[0], subScene_center[1], subScene_center[2] + subScene_maxBndLen*0.7, 0.0f };
	GLfloat sun_light_position[] = { 0, 0, 1, 0.0f };
	GLfloat sun_light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat sun_light_specular[] = { 0, 0, 0, 0 };

	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

void Render_Scene_Subwnd(void){
	/// 1.Open depth_test,then clear color buffer and depth buffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();
	glPushMatrix();

	glTranslatef(subScene_center[0], subScene_center[1], subScene_center[2]);
	tbMatrix();
	glTranslatef(-subScene_center[0], -subScene_center[1], -subScene_center[2]);

	/// 3.Draw everything
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	Draw_axes(subScene_center[0], subScene_center[1], subScene_center[2], subScene_maxBndLen*1.3);
	glvmDraw(subWndmodel); //draw voxel model

	//	blending test
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4ub(255, 0, 0, 50);
	glutSolidCube(10.0);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glPopMatrix();

	// show text ==================> °µ¦¨function
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor3ub(255, 0, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, subWndSize_x, 0, subWndSize_y);

	// show Character
	glRasterPos2i(10, subWndSize_y - 23);
	char str[] = "Show Data";
	for (int i = 0; str[i]; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
	glRasterPos2i(10, subWndSize_y - 43);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	// 3.Swap buffers
	glutSwapBuffers();
}

void Reshape_Subwnd(int width, int high){
	tbReshape(width, high);

	glViewport(0, 0, width, high);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, float(width) / high, subScene_maxBndLen*0.1f, subScene_maxBndLen*10.0f);
}