//#include "glutWindow.h"
#include "glutMainWindow.h"


#pragma once
int Create_glutSubWnd(char tittle[], GLVoxelModel* ptr_vmodel);

void Initial_Scene_Subwnd();
void Render_Scene_Subwnd(void);
void Reshape_Subwnd(int width, int high);