#include "glutWindow.h"

#pragma once
int Create_glutAnimateWnd(char tittle[], Animate* animate);

void Initial_Scene_AnimateWnd();
void Render_Scene_AnimateWnd(void);
void Reshape_AnimateWnd(int width, int high);

void AnimateWnd_mouse(int button, int state, int x, int y);
void AnimateWnd_motion(int x, int y);
void AnimateWnd_mousewheel(int wheel, int direction, int x, int y);
void AnimateWnd_keyboard(unsigned char key, int x, int y);

void AnimateWnd_timf(int value);