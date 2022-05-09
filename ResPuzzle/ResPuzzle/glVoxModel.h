//	THIS place should be include opengl, something wrong on my PC (:p)
#include "GL/freeglut.h"	
/////////////////////////////////////////////////////////////////////
#include < vector >

#pragma once
/// <summary>
/// glVoxModel.h描述:
/// 提供一個模型結構GLVoxelModel來記錄體素(voxel)的模型
///	可以運用glvmReadJSON()來讀取json檔案並且用glvmDraw()繪製GLVoxelModel的模型
///
/// 當然你也可以自訂出GLVoxelModel的資料然後匯出
///	要注意的是GLVoxelModel原則上可以記錄任意的模型結構(不一定要用voxel)
/// </summary>
struct GLVoxelModel
{
	std::vector<GLfloat> vertex_list;
	std::vector<GLubyte> color_list;
	std::vector<GLuint> index_list;
	std::vector<GLfloat> normal_list;
	int num_v = 0;//number of vertex

	GLfloat x_min, x_max;
	GLfloat y_min, y_max;
	GLfloat z_min, z_max;
	GLfloat maxBndLen;	// max bounding length;
	GLfloat center[3];	// object center : center of bounding box
};

struct RGBAcolor{
	GLubyte R; //0~255
	GLubyte G; //0~255
	GLubyte B; //0~255
	GLubyte A;	//0~255
};

struct HSVcolor{
	GLfloat H; //0~360
	GLfloat S; //0~1
	GLfloat V; //0~1
};

GLVoxelModel* glvmReadJSON(const char* filename);

GLvoid glvmDraw(GLVoxelModel* model);

GLvoid glvmDrawWithImmediateMode(GLVoxelModel* model);

GLfloat glvmCalculatemaxBndLen(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ);

RGBAcolor glvmHSVtoRGBA(HSVcolor hsv);

HSVcolor glvmRGBAtoHSV(RGBAcolor rgba);

//
//    v4----- v5
//   /|      /|
//  v6------v7|
//  | |     | |
//  | |v0---|-|v1
//  |/      |/
//  v2------v3
//	push cube into asign model

GLvoid glvmPushCube(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model);

GLvoid glvmPushCubeFront(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model);

GLvoid glvmPushCubeEnd(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model);

GLvoid glvmPushCubeLeft(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model);

GLvoid glvmPushCubeRight(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model);

GLvoid glvmPushCubeTop(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model);

GLvoid glvmPushCubeDown(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model);

//TODO:加入 glvmPushSphere()的方法，加入繪製Voxel邊框的方法

//	EXAMPLE
//	CASE1:assign your file to glvmReadJSON
/*
	//include lib
	#include "glVoxModel.h"

	//inilize a pointer on global(or any place you want)
	GLVoxelModel* pvmodel = nullptr; //pointer to voxel model

	//assign your file to glvmReadJSON
	pvmodel = glvmReadJSON("input/al-5.json");

	//draw the model
	glvmDraw(pvmodel);
	//or this
	glvmDrawWithImmediateMode(pvmodel);
*/

// CASE2:define your own model
/*
	//include lib
	#include "glVoxModel.h"

	//define your own model
	GLVoxelModel* cube = new GLVoxelModel;
	cube->vertex_list = {
	-50.0f, -50.0f, -50.0f,	//v0
	50.0f, -50.0f, -50.0f,	//v1
	-50.0f, 50.0f, -50.0f,	//v2
	50.0f, 50.0f, -50.0f,	//v3
	-50.0f, -50.0f, 50.0f,	//v4
	50.0f, -50.0f, 50.0f,	//v5
	-50.0f, 50.0f, 50.0f,	//v6
	50.0f, 50.0f, 50.0f,	//v7
	};

	cube->color_list = {
	0, 0, 0, 255,
	255, 0, 0, 255,
	0, 255, 0, 255,
	255, 255, 0, 255,
	0, 0, 255, 255,
	255, 0, 255, 255,
	0, 255, 255, 255,
	255, 255, 255, 255,
	};

	cube->index_list = {
		//fornt
		4, 5, 7,
		7, 6, 4,
		//end
		1, 0, 2,
		2, 3, 1,
		//left
		0, 4, 6,
		6, 2, 0,
		//right
		5, 1, 3,
		3, 7, 5,
		//top
		6, 7, 3,
		3, 2, 6,
		//down
		0, 1, 5,
		5, 4, 0,
	};

	cube->num_v = 8; //there is 8 vertex on vertexlist
	cube->x_min = -50.0f;
	cube->x_max = 50.0f;
	cube->y_min = -50.0f;
	cube->y_max = 50.0f;
	cube->z_min = -50.0f;
	cube->z_max = 50.0f;
	cube->maxBndLen = 100.0f;
	cube->center[0] = 0.0f;
	cube->center[1] = 0.0f;
	cube->center[2] = 0.0f;

	//draw the model
	glvmDraw(cube);
*/