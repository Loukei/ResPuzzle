#include "stdafx.h"
#include "glVoxModel.h"
#include "jsoncons/json.hpp"
#include < vector >

using jsoncons::json;
using namespace std;

/// <summary>
/// GLVMs the read json.
/// </summary>
/// <param name="filename">resource .json FILE path.</param>
/// <returns>The pointer to voxel model</returns>
GLVoxelModel* glvmReadJSON(const char* filename)
{
	GLVoxelModel* VModel;
	VModel = new GLVoxelModel;
	// allocate a new model
	// C++ <victor> initilize will be empty element (vertex_list / vectorcolor_list / index_list;)
	VModel->num_v = 0;			//number of vertex
	VModel->x_min = 0.0f;
	VModel->x_max = 0.0f;
	VModel->y_min = 0.0f;
	VModel->y_max = 0.0f;
	VModel->z_min = 0.0f;
	VModel->z_max = 0.0f;
	VModel->maxBndLen = 0.0f;	// max bounding length;
	VModel->center[0] = 0.0f;	// object center : center of bounding box
	VModel->center[1] = 0.0f;
	VModel->center[2] = 0.0f;

	json obj = json::parse_file(filename);

	vector<double> vox_size = obj["VoxelSIZE"].as<std::vector<double>>();
	vector<int> model_scale = obj["ResolutionXYZ"].as<std::vector<int>>();
	vector<double> startingfrom = obj["StartingFrom"].as<std::vector<double>>();

	//	compute bounding box
	VModel->x_min = startingfrom[0];
	VModel->x_max = startingfrom[0] + (model_scale[0] + 1) * vox_size[0];
	VModel->y_min = startingfrom[1];
	VModel->y_max = startingfrom[1] + (model_scale[1] + 1) * vox_size[1];
	VModel->z_min = startingfrom[2];
	VModel->z_max = startingfrom[2] + (model_scale[2] + 1) * vox_size[2];

	// compute the length for the longest edge of bounding box
	VModel->maxBndLen = glvmCalculatemaxBndLen(VModel->x_min, VModel->y_min, VModel->z_min, VModel->x_max, VModel->y_max, VModel->z_max);

	// object center : center of bounding box
	VModel->center[0] = (VModel->x_min + VModel->x_max) / 2.0;
	VModel->center[1] = (VModel->y_min + VModel->y_max) / 2.0;
	VModel->center[2] = (VModel->z_min + VModel->z_max) / 2.0;

	//	read for each SHELLVoxel
	for (size_t i = 0; i < obj["ShellVoxel"].size(); i++)
	{
		json& voxel = obj["ShellVoxel"][i];

		vector<int> voxel_color = voxel["RGBAcolor"].as<std::vector<int>>();
		vector<int> voxel_id = voxel["Position"].as<std::vector<int>>();

		GLfloat x_max, x_min, y_max, y_min, z_max, z_min;
		x_min = startingfrom[0] + voxel_id[0] * vox_size[0];
		x_max = startingfrom[0] + (voxel_id[0] + 1) * vox_size[0];
		y_min = startingfrom[1] + voxel_id[1] * vox_size[1];
		y_max = startingfrom[1] + (voxel_id[1] + 1) * vox_size[1];
		z_min = startingfrom[2] + voxel_id[2] * vox_size[2];
		z_max = startingfrom[2] + (voxel_id[2] + 1) * vox_size[2];

		glvmPushCube(x_min, y_min, z_min, x_max, y_max, z_max, voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3], VModel);
	}

	return VModel;
}

/// <summary>
/// GLVMs the draw GLVoxModel.
///	NOTICE that this function only draw each face with GL_QUADS
/// </summary>
/// <param name="model">The model pointer.</param>
/// <returns></returns>
GLvoid glvmDraw(GLVoxelModel* model)
{
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);

	////glEnable(GL_BACK);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);

	//glVertexPointer(3, GL_FLOAT, 0, &(model->vertex_list[0]));
	//glColorPointer(4, GL_UNSIGNED_BYTE, 0, &(model->color_list[0]));
	//glNormalPointer(GL_FLOAT, 0, &(model->normal_list[0]));
	//
	//glDisable(GL_LIGHTING);
	//glDisable(GL_COLOR_MATERIAL);
	//glDisableClientState(GL_COLOR_ARRAY);
	//glColor3ub(255,0,0);
	//glDrawElements(GL_LINES, model->index_list.size(), GL_UNSIGNED_INT, &(model->index_list[0]));

	//glEnable(GL_LIGHTING);
	//glEnable(GL_COLOR_MATERIAL);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 1.0);
	//glDrawElements(GL_TRIANGLES, model->index_list.size(), GL_UNSIGNED_INT, &(model->index_list[0]));
	//glDisable(GL_POLYGON_OFFSET_FILL);

	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisable(GL_BACK);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	//glEnable(GL_BACK);

	//draw line
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, &(model->vertex_list[0]));
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, &(model->color_list[0]));
	glNormalPointer(GL_FLOAT, 0, &(model->normal_list[0]));

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisableClientState(GL_COLOR_ARRAY);
	glColor3ub(255, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_QUADS, model->index_list.size(), GL_UNSIGNED_INT, &(model->index_list[0]));

	// draw face
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnableClientState(GL_COLOR_ARRAY);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	glDrawElements(GL_QUADS, model->index_list.size(), GL_UNSIGNED_INT, &(model->index_list[0]));
	glDisable(GL_POLYGON_OFFSET_FILL);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_BACK);
}

GLvoid glvmDrawWithImmediateMode(GLVoxelModel* model)
{
	glFrontFace(GL_CCW);

	glEnable(GL_BACK);
	glBegin(GL_TRIANGLES);
	//draw each triangle
	for (size_t i = 0; i < model->index_list.size(); i += 3)
	{
		int tri[3] = { model->index_list[i], model->index_list[i + 1], model->index_list[i + 2] };
		GLfloat v1[3] = { model->vertex_list[tri[0] * 3], model->vertex_list[tri[0] * 3 + 1], model->vertex_list[tri[0] * 3 + 2] };
		GLfloat v2[3] = { model->vertex_list[tri[1] * 3], model->vertex_list[tri[1] * 3 + 1], model->vertex_list[tri[1] * 3 + 2] };
		GLfloat v3[3] = { model->vertex_list[tri[2] * 3], model->vertex_list[tri[2] * 3 + 1], model->vertex_list[tri[2] * 3 + 2] };
		GLubyte c1[4] = { model->color_list[tri[0] * 4], model->color_list[tri[0] * 4 + 1], model->color_list[tri[0] * 4 + 2], model->color_list[tri[0] * 4 + 3] };
		GLubyte c2[4] = { model->color_list[tri[1] * 4], model->color_list[tri[1] * 4 + 1], model->color_list[tri[1] * 4 + 2], model->color_list[tri[1] * 4 + 3] };
		GLubyte c3[4] = { model->color_list[tri[2] * 4], model->color_list[tri[2] * 4 + 1], model->color_list[tri[2] * 4 + 2], model->color_list[tri[2] * 4 + 3] };
		GLfloat n1[3] = { model->normal_list[tri[0] * 3], model->normal_list[tri[0] * 3 + 1], model->normal_list[tri[0] * 3 + 2] };
		GLfloat n2[3] = { model->normal_list[tri[1] * 3], model->normal_list[tri[1] * 3 + 1], model->normal_list[tri[1] * 3 + 2] };
		GLfloat n3[3] = { model->normal_list[tri[2] * 3], model->normal_list[tri[2] * 3 + 1], model->normal_list[tri[2] * 3 + 2] };
		glNormal3fv(n1);	glColor4ubv(c1);	glVertex3fv(v1);
		glNormal3fv(n2);	glColor4ubv(c2);	glVertex3fv(v2);
		glNormal3fv(n3);	glColor4ubv(c3);	glVertex3fv(v3);
	}
	glEnd();
	glDisable(GL_BACK);
}

GLfloat glvmCalculatemaxBndLen(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ)
{
	GLfloat maxBndLen = 0.0f;
	GLfloat x_len, y_len, z_len;

	maxBndLen = x_len = maxX - minX;
	y_len = maxY - minY;
	z_len = maxZ - minZ;
	if (maxBndLen < y_len)
	{
		maxBndLen = y_len;
	}
	if (maxBndLen < z_len)
	{
		maxBndLen = z_len;
	}
	return maxBndLen;
}

//http://www.cs.rit.edu/~ncs/color/t_convert.html
RGBAcolor glvmHSVtoRGBA(HSVcolor hsv)
{
	RGBAcolor rgba;

	int i;
	float f, p, q, t;
	float r, g, b; //use range 0~1

	if (hsv.S == 0) {
		// achromatic (grey)
		r = g = b = hsv.V;
		rgba = { (int)(r * 255), (int)(g * 255), (int)(b * 255), 255 };
		return rgba;
	}

	hsv.H /= 60;			// sector 0 to 5
	i = floor(hsv.H);
	f = hsv.H - i;			// factorial part of h
	p = hsv.V * (1 - hsv.S);
	q = hsv.V * (1 - hsv.S * f);
	t = hsv.V * (1 - hsv.S * (1 - f));

	switch (i) {
	case 0:
		r = hsv.V;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = hsv.V;
		b = p;
		break;
	case 2:
		r = p;
		g = hsv.V;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = hsv.V;
		break;
	case 4:
		r = t;
		g = p;
		b = hsv.V;
		break;
	default:		// case 5:
		r = hsv.V;
		g = p;
		b = q;
		break;
	}

	rgba = { (int)(r * 255), (int)(g * 255), (int)(b * 255), 1 };
	return rgba;
}

HSVcolor glvmRGBAtoHSV(RGBAcolor rgba)
{
	HSVcolor hsv;

	float r = (float)rgba.R / 255;
	float g = (float)rgba.G / 255;
	float b = (float)rgba.B / 255;
	float min, max, delta;

	//min = MIN(r, g, b);
	min = r;
	if (g < min){ min = g; }
	if (b < min){ min = b; }
	//max = MAX(r, g, b);
	max = r;
	if (g > max){ max = g; }
	if (b > max){ max = b; }

	hsv.V = max;				// v
	delta = max - min;

	if (max != 0)
		hsv.S = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		hsv.S = 0;
		hsv.H = -1;
		return hsv;
	}

	if (r == max)
		hsv.H = (g - b) / delta;		// between yellow & magenta
	else if (g == max)
		hsv.H = 2 + (b - r) / delta;	// between cyan & yellow
	else
		hsv.H = 4 + (r - g) / delta;	// between magenta & cyan

	hsv.H *= 60;				// degrees
	if (hsv.H < 0)
		hsv.H += 360;

	return hsv;
}

//    v4----- v5
//   /|      /|
//  v6------v7|
//  | |     | |
//  | |v0---|-|v1
//  |/      |/
//  v2------v3
//push cube into asign model

GLvoid glvmPushCube(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model)
{
	//just push each face (in cube) into model
	glvmPushCubeFront(minX, minY, minZ, maxX, maxY, maxZ, colorR, colorG, colorB, colorA, model);
	glvmPushCubeEnd(minX, minY, minZ, maxX, maxY, maxZ, colorR, colorG, colorB, colorA, model);
	glvmPushCubeLeft(minX, minY, minZ, maxX, maxY, maxZ, colorR, colorG, colorB, colorA, model);
	glvmPushCubeRight(minX, minY, minZ, maxX, maxY, maxZ, colorR, colorG, colorB, colorA, model);
	glvmPushCubeTop(minX, minY, minZ, maxX, maxY, maxZ, colorR, colorG, colorB, colorA, model);
	glvmPushCubeDown(minX, minY, minZ, maxX, maxY, maxZ, colorR, colorG, colorB, colorA, model);
}

GLvoid glvmPushCubeFront(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model)
{
	GLfloat x_min = minX;
	GLfloat y_min = minY;
	GLfloat z_min = minZ;
	GLfloat x_max = maxX;
	GLfloat y_max = maxY;
	GLfloat z_max = maxZ;

	GLfloat voxel_vertex_list[12] = {
		x_min, y_min, z_max,	//v4 0
		x_max, y_min, z_max,	//v5 1
		x_min, y_max, z_max,	//v6 2
		x_max, y_max, z_max,	//v7 3
	};

	GLfloat voxel_normal_list[12] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	GLubyte voxel_color_list[16] = {
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
	};

	//// triangle way
	//GLuint voxel_index_list[6] = {
	//	//fornt
	//	model->num_v + 0, model->num_v + 1, model->num_v + 3,
	//	model->num_v + 3, model->num_v + 2, model->num_v + 0,
	//};

	//model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	//model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	//model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	//model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 6);
	////model->num_v = model->num_v + 4;
	//model->num_v = (int)(model->vertex_list.size()) / 3;

	// quad way
	GLuint voxel_index_list[4] = {
		//fornt
		model->num_v + 0, model->num_v + 1, model->num_v + 3, model->num_v + 2
	};

	model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 4);
	//model->num_v = model->num_v + 4;
	model->num_v = (int)(model->vertex_list.size()) / 3;
}

GLvoid glvmPushCubeEnd(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model)
{
	GLfloat x_min = minX;
	GLfloat y_min = minY;
	GLfloat z_min = minZ;
	GLfloat x_max = maxX;
	GLfloat y_max = maxY;
	GLfloat z_max = maxZ;

	GLfloat voxel_vertex_list[12] = {
		x_min, y_min, z_min,	//v0
		x_max, y_min, z_min,	//v1
		x_min, y_max, z_min,	//v2
		x_max, y_max, z_min,	//v3
	};

	GLfloat voxel_normal_list[12] = {
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
	};

	GLubyte voxel_color_list[16] = {
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
	};
	//// triangle way
	//GLuint voxel_index_list[6] = {
	//	//end
	//	model->num_v + 1, model->num_v + 0, model->num_v + 2,
	//	model->num_v + 2, model->num_v + 3, model->num_v + 1,
	//};
	//model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	//model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	//model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	//model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 6);
	////model->num_v = model->num_v + 4;
	//model->num_v = (int)(model->vertex_list.size()) / 3;

	// quad way
	GLuint voxel_index_list[4] = {
		//end
		model->num_v + 1, model->num_v + 0, model->num_v + 2, model->num_v + 3
	};

	model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 4);
	//model->num_v = model->num_v + 4;
	model->num_v = (int)(model->vertex_list.size()) / 3;
}

GLvoid glvmPushCubeLeft(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model)
{
	GLfloat x_min = minX;
	GLfloat y_min = minY;
	GLfloat z_min = minZ;
	GLfloat x_max = maxX;
	GLfloat y_max = maxY;
	GLfloat z_max = maxZ;

	GLfloat voxel_vertex_list[12] = {
		x_min, y_min, z_min,	//v0 0
		x_min, y_max, z_min,	//v2 1
		x_min, y_min, z_max,	//v4 2
		x_min, y_max, z_max,	//v6 3
	};

	GLfloat voxel_normal_list[12] = {
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
	};

	GLubyte voxel_color_list[16] = {
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
	};
	//// triangle way
	//GLuint voxel_index_list[6] = {
	//	//left
	//	model->num_v + 0, model->num_v + 2, model->num_v + 3,
	//	model->num_v + 3, model->num_v + 1, model->num_v + 0,
	//};
	//model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	//model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	//model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	//model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 6);
	////model->num_v = model->num_v + 4;
	//model->num_v = (int)(model->vertex_list.size()) / 3;

	// quad way
	GLuint voxel_index_list[4] = {
		//left
		model->num_v + 0, model->num_v + 2, model->num_v + 3, model->num_v +1
	};
	model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 4);
	//model->num_v = model->num_v + 4;
	model->num_v = (int)(model->vertex_list.size()) / 3;
}

GLvoid glvmPushCubeRight(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model)
{
	GLfloat x_min = minX;
	GLfloat y_min = minY;
	GLfloat z_min = minZ;
	GLfloat x_max = maxX;
	GLfloat y_max = maxY;
	GLfloat z_max = maxZ;

	GLfloat voxel_vertex_list[12] = {
		x_max, y_min, z_min,	//v1 0
		x_max, y_max, z_min,	//v3 1
		x_max, y_min, z_max,	//v5 2
		x_max, y_max, z_max,	//v7 3
	};

	GLfloat voxel_normal_list[12] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	GLubyte voxel_color_list[16] = {
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
	};
	//// triangle way
	//GLuint voxel_index_list[6] = {
	//	//right
	//	model->num_v + 2, model->num_v + 0, model->num_v + 1,
	//	model->num_v + 1, model->num_v + 3, model->num_v + 2,
	//};
	//model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	//model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	//model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	//model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 6);
	////model->num_v = model->num_v + 4;
	//model->num_v = (int)(model->vertex_list.size()) / 3;

	// triangle way
	GLuint voxel_index_list[4] = {
		//right
		model->num_v + 2, model->num_v + 0, model->num_v + 1, model->num_v + 3
	};
	model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 4);
	//model->num_v = model->num_v + 4;
	model->num_v = (int)(model->vertex_list.size()) / 3;
}

GLvoid glvmPushCubeTop(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model)
{
	GLfloat x_min = minX;
	GLfloat y_min = minY;
	GLfloat z_min = minZ;
	GLfloat x_max = maxX;
	GLfloat y_max = maxY;
	GLfloat z_max = maxZ;

	GLfloat voxel_vertex_list[12] = {
		x_min, y_max, z_min,	//v2 0
		x_max, y_max, z_min,	//v3 1
		x_min, y_max, z_max,	//v6 2
		x_max, y_max, z_max,	//v7 3
	};

	GLfloat voxel_normal_list[12] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	GLubyte voxel_color_list[16] = {
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
	};
	//// triangle way
	//GLuint voxel_index_list[6] = {
	//	//top
	//	model->num_v + 2, model->num_v + 3, model->num_v + 1,
	//	model->num_v + 1, model->num_v + 0, model->num_v + 2,
	//};
	//model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	//model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	//model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	//model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 6);
	////model->num_v = model->num_v + 4;
	//model->num_v = (int)(model->vertex_list.size()) / 3;

	// quad way
	GLuint voxel_index_list[4] = {
		//top
		model->num_v + 2, model->num_v + 3, model->num_v + 1, model->num_v + 0
	};
	model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 4);
	//model->num_v = model->num_v + 4;
	model->num_v = (int)(model->vertex_list.size()) / 3;
}

GLvoid glvmPushCubeDown(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLubyte colorR, GLubyte colorG, GLubyte colorB, GLubyte colorA, GLVoxelModel* model)
{
	GLfloat x_min = minX;
	GLfloat y_min = minY;
	GLfloat z_min = minZ;
	GLfloat x_max = maxX;
	GLfloat y_max = maxY;
	GLfloat z_max = maxZ;

	GLfloat voxel_vertex_list[12] = {
		x_min, y_min, z_min,	//v0 0
		x_max, y_min, z_min,	//v1 1
		x_min, y_min, z_max,	//v4 2
		x_max, y_min, z_max,	//v5 3
	};

	GLfloat voxel_normal_list[12] = {
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
	};

	GLubyte voxel_color_list[16] = {
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
		colorR, colorG, colorB, colorA,
	};
	//// triangle way
	//GLuint voxel_index_list[6] = {
	//	//down
	//	model->num_v + 0, model->num_v + 1, model->num_v + 3,
	//	model->num_v + 3, model->num_v + 2, model->num_v + 0,
	//};
	//model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	//model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	//model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	//model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 6);
	////model->num_v = model->num_v + 4;
	//model->num_v = (int)(model->vertex_list.size()) / 3;

	// quad way
	GLuint voxel_index_list[4] = {
		//down
		model->num_v + 0, model->num_v + 1, model->num_v + 3, model->num_v + 2
	};
	model->vertex_list.insert(model->vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 12);
	model->normal_list.insert(model->normal_list.end(), voxel_normal_list, voxel_normal_list + 12);
	model->color_list.insert(model->color_list.end(), voxel_color_list, voxel_color_list + 16);
	model->index_list.insert(model->index_list.end(), voxel_index_list, voxel_index_list + 4);
	//model->num_v = model->num_v + 4;
	model->num_v = (int)(model->vertex_list.size()) / 3;
}