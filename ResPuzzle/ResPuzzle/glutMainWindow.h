//#include "GL/freeglut.h"
//#include "trackball.h"
//#include "glVoxModel.h"
//#include "ResPuzzleModel.h"
#include "glutWindow.h"

#pragma once
int Create_glutMainWnd(char tittle[], GLVoxelModel* ptr_vmodel);

void Initial_Scene_mainwnd();
void Render_Scene_mainwnd(void);
void Reshape_mainwnd(int width, int high);

void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void mousewheel(int wheel, int direction, int x, int y);
void keyboard(unsigned char key, int x, int y);

void timf(int value);