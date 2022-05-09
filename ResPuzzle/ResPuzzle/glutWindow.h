#include "GL/freeglut.h"
#include "trackball.h"
#include "glVoxModel.h"
#include "ResPuzzleModel.h"

#pragma once

typedef GLfloat vec3 [3] ;

struct Animate_trajectory
{
	short unsigned int to_model;
	vec3 direction;
	vec3 end;
	vec3 movement;
};

struct Animate
{
	std::vector<GLVoxelModel*> models;
	std::vector<Animate_trajectory> trajectory;
};

void Draw_axes(float cent_x, float cent_y, float cent_z, float length);

GLVoxelModel* CreateModelByID(CVoxelSpace* pVSpace, vector<VoxelID>& Vid_list);

GLVoxelModel* CreateModelByID(CVoxelSpace* pVSpace, vector<VoxelID>& Vid_list, vector<RGBAcolor>& Vid_list_color);
