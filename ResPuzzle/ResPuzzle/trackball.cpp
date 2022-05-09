/*
*  Simple trackball-like motion adapted (ripped off) from projtex.c
*  (written by David Yu and David Blythe).  See the SIGGRAPH '96
*  Advanced OpenGL course notes.
*
*
*  Usage:
*
*  o  call tbInit() in before any other tb call
*  o  call tbReshape() from the reshape callback
*  o  call tbMatrix() to get the trackball matrix rotation
*  o  call tbStartMotion() to begin trackball movememt
*  o  call tbStopMotion() to stop trackball movememt
*  o  call tbMotion() from the motion callback
*  o  call tbAnimate(GL_TRUE) if you want the trackball to continue
*     spinning after the mouse button has been released
*  o  call tbAnimate(GL_FALSE) if you want the trackball to stop
*     spinning after the mouse button has been released
*
*  Typical setup:
*
*
void
init(void)
{
tbInit(GLUT_MIDDLE_BUTTON);
tbAnimate(GL_TRUE);
. . .
}

void
reshape(int width, int height)
{
tbReshape(width, height);
. . .
}

void
display(void)
{
glPushMatrix();

tbMatrix();
. . . draw the scene . . .

glPopMatrix();
}

void
mouse(int button, int state, int x, int y)
{
tbMouse(button, state, x, y);
. . .
}

void
motion(int x, int y)
{
tbMotion(x, y);
. . .
}

int
main(int argc, char** argv)
{
. . .
init();
glutReshapeFunc(reshape);
glutDisplayFunc(display);
glutMouseFunc(mouse);
glutMotionFunc(motion);
. . .
}
*
* */


#include "stdafx.h"

/* includes */
#include <math.h>
#include <assert.h>
#include <GL/glut.h>
#include "trackball.h"


/* globals */
static GLuint    tb_lasttime;
static GLfloat   tb_lastposition[3];

static GLfloat   tb_angle = 0.0;
static GLfloat   tb_axis[3];
static GLfloat   tb_transform[4][4];

static GLuint    tb_width;
static GLuint    tb_height;

static GLint     tb_button = -1;
static GLboolean tb_tracking = GL_FALSE;
static GLboolean tb_animate = GL_TRUE;


// phlin
static GLint	tb_mouse_button;
static GLint	tb_oldPos[2];	// key down mouse position
static GLdouble	tb_modelview[16];
static GLdouble tb_project[16];
static GLint	tb_viewport[4];
static GLdouble tb_pan_x, tb_pan_y;

static GLdouble	tb_zoom;
static GLdouble	tb_zoom_inc;	// zoom increment
// end phlin

/* functions */
static void _tbPointToVector(int x, int y, int width, int height, float v[3])
{
	float d, a;

	/* project x, y onto a hemi-sphere centered within width, height. */
	v[0] = (2.0 * x - width) / width;
	v[1] = (height - 2.0 * y) / height;
	d = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[2] = cos((3.14159265 / 2.0) * ((d < 1.0) ? d : 1.0));
	//  a = 1.0 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	// phlin
	a = 0.25 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	// end phlin
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

static void _tbAnimate(void)
{
	glutPostRedisplay();
}

void _tbStartMotion(int x, int y, int button, int time)
{
	assert(tb_button != -1);

	tb_tracking = GL_TRUE;
	tb_lasttime = time;
	_tbPointToVector(x, y, tb_width, tb_height, tb_lastposition);
}

void _tbStopMotion(int button, unsigned time)
{
	assert(tb_button != -1);

	tb_tracking = GL_FALSE;

	// phlin
	//  if (time == tb_lasttime && tb_animate) {
	float thisTime, lastTime;
	thisTime = time;
	lastTime = tb_lasttime;
	if (fabs(thisTime - lastTime)<3 && tb_animate) {
		// end phlin  
		glutIdleFunc(_tbAnimate);
	}
	else {
		tb_angle = 0.0;
		if (tb_animate)
			glutIdleFunc(0);
	}
}

void tbAnimate(GLboolean animate)
{
	tb_animate = animate;
}

void tbInit(GLuint button)
{
	tb_button = button;
	tb_angle = 0.0;

	// phlin
	tb_pan_x = 0.0;
	tb_pan_y = 0.0;

	tb_zoom = 1.0;
	tb_zoom_inc = 0.005;
	// end phlin


	/* put the identity in the trackball transform */
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)tb_transform);
	glPopMatrix();
}

void tbMatrix()
{
	assert(tb_button != -1);

	glPushMatrix();
	glLoadIdentity();
	glRotatef(tb_angle, tb_axis[0], tb_axis[1], tb_axis[2]);
	glMultMatrixf((GLfloat *)tb_transform);
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)tb_transform);
	glPopMatrix();

	// phlin
	glTranslatef(tb_pan_x, tb_pan_y, 0.0);
	glScaled(tb_zoom, tb_zoom, tb_zoom);
	// end phlin

	glMultMatrixf((GLfloat *)tb_transform);

	// phlin
	// 	tb_angle = 0.0;
	//	tb_axis[0] = 1.0; tb_axis[1] = 0.0; tb_axis[2] = 0.0;
	// end phlin
}

void tbReshape(int width, int height)
{
	assert(tb_button != -1);

	tb_width = width;
	tb_height = height;
}

void tbMouse(int button, int state, int x, int y)
{
	assert(tb_button != -1);

	if (state == GLUT_DOWN && button == tb_button)
		_tbStartMotion(x, y, button, glutGet(GLUT_ELAPSED_TIME));
	else if (state == GLUT_UP && button == tb_button)
		_tbStopMotion(button, glutGet(GLUT_ELAPSED_TIME));

	// phlin
	if (state == GLUT_DOWN) {
		tb_mouse_button = button;
		if (button == GLUT_RIGHT_BUTTON || button == GLUT_MIDDLE_BUTTON) {
			tb_oldPos[0] = x;
			tb_oldPos[1] = y;
		}
	}

	// end phlin
}

void tbMotion(int x, int y)
{
	GLfloat current_position[3], dx, dy, dz;

	// phlin
	GLdouble winx, winy, winz, tmp, old_pan_x, old_pan_y, new_pan_x, new_pan_y;
	// end phlin

	// phlin
	// rotating : mouse left button
	if (tb_mouse_button == tb_button) {
		// end phlin

		assert(tb_button != -1);

		if (tb_tracking == GL_FALSE)
			return;

		_tbPointToVector(x, y, tb_width, tb_height, current_position);

		/* calculate the angle to rotate by (directly proportional to the
		length of the mouse movement */
		dx = current_position[0] - tb_lastposition[0];
		dy = current_position[1] - tb_lastposition[1];
		dz = current_position[2] - tb_lastposition[2];
		tb_angle =180.0 * sqrt(dx * dx + dy * dy + dz * dz);

		/* calculate the axis of rotation (cross product) */
		tb_axis[0] = tb_lastposition[1] * current_position[2] -
			tb_lastposition[2] * current_position[1];
		tb_axis[1] = tb_lastposition[2] * current_position[0] -
			tb_lastposition[0] * current_position[2];
		tb_axis[2] = tb_lastposition[0] * current_position[1] -
			tb_lastposition[1] * current_position[0];

		/* reset for next time */
		tb_lasttime = glutGet(GLUT_ELAPSED_TIME);
		tb_lastposition[0] = current_position[0];
		tb_lastposition[1] = current_position[1];
		tb_lastposition[2] = current_position[2];
	}

	// phlin
	// panning : mouse right button
	else if (tb_mouse_button == GLUT_RIGHT_BUTTON)
	{
		glGetDoublev(GL_MODELVIEW_MATRIX, tb_modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, tb_project);
		glGetIntegerv(GL_VIEWPORT, tb_viewport);

		gluProject(0.0, 0.0, 0.0, tb_modelview, tb_project, tb_viewport, &winx, &winy, &winz);
		// 這也許會有問題, 因為不確定 0.0, 0.0, 0.0 一定會落在 view frustum 內
		// 但實際操作好像沒問題

		gluUnProject(x, tb_height - y, winz, tb_modelview, tb_project, tb_viewport,
			&new_pan_x, &new_pan_y, &tmp);

		gluUnProject(tb_oldPos[0], tb_height - tb_oldPos[1], winz, tb_modelview, tb_project, tb_viewport,
			&old_pan_x, &old_pan_y, &tmp);

		tb_pan_x += (new_pan_x - old_pan_x);
		tb_pan_y += (new_pan_y - old_pan_y);

		tb_oldPos[0] = x;
		tb_oldPos[1] = y;
	}

	// zooming : mouse middle button
	else if (tb_mouse_button == GLUT_MIDDLE_BUTTON) {
		tb_zoom = tb_zoom + ((x - tb_oldPos[0]) - (y - tb_oldPos[1])) * tb_zoom_inc;
		if (tb_zoom <= 0.0)
			tb_zoom = tb_zoom_inc;
		tb_oldPos[0] = x;
		tb_oldPos[1] = y;
	}

	// end phlin

	/* remember to draw new position */
	glutPostRedisplay();
}

// phlin
GLdouble tb_getScale(void)
{
	return tb_zoom;
}