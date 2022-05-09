#include "stdafx.h"
#include "glutWindow.h"
#include "globals.h"

void Draw_axes(float cent_x, float cent_y, float cent_z, float length){
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	// x-axis :red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(cent_x, cent_y, cent_z);
	glVertex3f(cent_x + length, cent_y, cent_z);
	// y-axis :green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(cent_x, cent_y, cent_z);
	glVertex3f(cent_x, cent_y + length, cent_z);
	// z-axis :blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(cent_x, cent_y, cent_z);
	glVertex3f(cent_x, cent_y, cent_z + length);
	glEnd();
}

GLVoxelModel* CreateModelByID(CVoxelSpace* pVSpace, vector<VoxelID>& Vid_list)
{
	//TODO:盢ㄧΑ虏て
	// Create a GLVoxModel & initilize
	GLVoxelModel* VModel;
	VModel = new GLVoxelModel;
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

	//Get the voxelsize and starting coordinate from VoxelSpace
	int resolution_x = pVSpace->resolution_x;
	int resolution_y = pVSpace->resolution_y;
	int resolution_z = pVSpace->resolution_z;

	float voxelsize_x = pVSpace->voxelsize[0];
	float voxelsize_y = pVSpace->voxelsize[1];
	float voxelsize_z = pVSpace->voxelsize[2];

	GLfloat startingfrom_x = pVSpace->startingfrom[0];
	GLfloat startingfrom_y = pVSpace->startingfrom[1];
	GLfloat startingfrom_z = pVSpace->startingfrom[2];

	//	Create a 3d array to varify if its on the vid_list
	vector < vector< vector< bool > > > IsThisVoxelBelongToVidList;
	//	Resize IsThisVoxelBelongToVidList[resolution_x][resolution_y][resolution_z]
	IsThisVoxelBelongToVidList.resize(resolution_x);
	for (int i = 0; i < resolution_x; ++i) {
		IsThisVoxelBelongToVidList[i].resize(resolution_y);
		for (int j = 0; j < resolution_y; ++j)
			IsThisVoxelBelongToVidList[i][j].resize(resolution_z);
	}
	// Initilize IsThisVoxelBelongToVidList[][][]
	for (size_t x = 0; x < resolution_x; x++)
	{
		for (size_t y = 0; y < resolution_y; y++)
		{
			for (size_t z = 0; z < resolution_z; z++)
			{
				IsThisVoxelBelongToVidList[x][y][z] = false;
			}
		}
	}
	// Assign value to IsThisVoxelBelongToVidList[][][]
	for (size_t i = 0; i < Vid_list.size(); i++)
	{
		if (pVSpace->IsThisVoxelIDSafe(Vid_list[i].x, Vid_list[i].y, Vid_list[i].z))
		{
			IsThisVoxelBelongToVidList[Vid_list[i].x][Vid_list[i].y][Vid_list[i].z] = true;
		}
	}

	//	Now, for each voxel in voxel_id_list,check each face if its outside
	for (int i = 0; i < Vid_list.size(); i++)
	{
		int vid_x = Vid_list[i].x;
		int vid_y = Vid_list[i].y;
		int vid_z = Vid_list[i].z;
		//check if id is legal
		if (pVSpace->IsThisVoxelIDSafe(vid_x, vid_y, vid_z))
		{
			//get the color and vertex from VSmatrix
			VoxelColor color = pVSpace->VSmatrix[vid_x][vid_y][vid_z]->getColor();
			float vox_size[3] = { voxelsize_x, voxelsize_y, voxelsize_z };

			//    v4----- v5
			//   /|      /|
			//  v6------v7|
			//  | |     | |
			//  | |v0---|-|v1
			//  |/      |/
			//  v2------v3
			GLfloat x_max, x_min, y_max, y_min, z_max, z_min;
			x_min = startingfrom_x + vid_x * vox_size[0];
			x_max = startingfrom_x + (vid_x + 1) * vox_size[0];
			y_min = startingfrom_y + vid_y * vox_size[1];
			y_max = startingfrom_y + (vid_y + 1) * vox_size[1];
			z_min = startingfrom_z + vid_z * vox_size[2];
			z_max = startingfrom_z + (vid_z + 1) * vox_size[2];

			// x+1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x + 1, vid_y, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeRight(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x + 1][vid_y][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeRight(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//x-1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x - 1, vid_y, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeLeft(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x - 1][vid_y][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeLeft(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//y+1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y + 1, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeTop(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y + 1][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeTop(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//y-1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y - 1, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeDown(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y - 1][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeDown(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//z+1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y, vid_z + 1))
			{
				//if it out of range, draw it
				glvmPushCubeFront(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y][vid_z + 1]){
				//if it not belong to vid_list, draw it
				glvmPushCubeFront(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//z-1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y, vid_z - 1))
			{
				//if it out of range, draw it
				glvmPushCubeEnd(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y][vid_z - 1]){
				//if it not belong to vid_list, draw it
				glvmPushCubeEnd(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
		}
	}

	//	compute bounding box
	//	眔 Vid_list 程程xyz だ秖―家程程
	int MAX_vid_x = Vid_list[0].x;
	int MIN_vid_x = Vid_list[0].x;
	int MAX_vid_y = Vid_list[0].y;
	int MIN_vid_y = Vid_list[0].y;
	int MAX_vid_z = Vid_list[0].z;
	int MIN_vid_z = Vid_list[0].z;
	for (int i = 0; i < Vid_list.size(); i++)
	{
		if (Vid_list[i].x > MAX_vid_x)
		{
			MAX_vid_x = Vid_list[i].x;
		}
		if (Vid_list[i].x < MIN_vid_x)
		{
			MIN_vid_x = Vid_list[i].x;
		}
		if (Vid_list[i].y > MAX_vid_y)
		{
			MAX_vid_y = Vid_list[i].y;
		}
		if (Vid_list[i].y < MIN_vid_y)
		{
			MIN_vid_y = Vid_list[i].y;
		}
		if (Vid_list[i].z > MAX_vid_z)
		{
			MAX_vid_z = Vid_list[i].z;
		}
		if (Vid_list[i].z < MIN_vid_z)
		{
			MIN_vid_z = Vid_list[i].z;
		}
	}

	VModel->x_min = startingfrom_x + MIN_vid_x * voxelsize_x;
	VModel->x_max = startingfrom_x + (MAX_vid_x + 1) * voxelsize_x;
	VModel->y_min = startingfrom_y + MIN_vid_y * voxelsize_y;
	VModel->y_max = startingfrom_y + (MAX_vid_y + 1) * voxelsize_y;
	VModel->z_min = startingfrom_z + MIN_vid_z * voxelsize_z;
	VModel->z_max = startingfrom_z + (MAX_vid_z + 1) * voxelsize_z;

	// compute the length for the longest edge of bounding box
	VModel->maxBndLen = glvmCalculatemaxBndLen(VModel->x_min, VModel->y_min, VModel->z_min, VModel->x_max, VModel->y_max, VModel->z_max);

	// object center : center of bounding box
	VModel->center[0] = (VModel->x_min + VModel->x_max) / 2.0;
	VModel->center[1] = (VModel->y_min + VModel->y_max) / 2.0;
	VModel->center[2] = (VModel->z_min + VModel->z_max) / 2.0;

	return VModel;
}

GLVoxelModel* CreateModelByID(CVoxelSpace* pVSpace, vector<VoxelID>& Vid_list, vector<RGBAcolor>& Vid_list_color)
{
	//TODO:盢ㄧΑ虏て
	// Create a GLVoxModel & initilize
	GLVoxelModel* VModel;
	VModel = new GLVoxelModel;
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

	//Get the voxelsize and starting coordinate from VoxelSpace
	int resolution_x = pVSpace->resolution_x;
	int resolution_y = pVSpace->resolution_y;
	int resolution_z = pVSpace->resolution_z;

	float voxelsize_x = pVSpace->voxelsize[0];
	float voxelsize_y = pVSpace->voxelsize[1];
	float voxelsize_z = pVSpace->voxelsize[2];

	GLfloat startingfrom_x = pVSpace->startingfrom[0];
	GLfloat startingfrom_y = pVSpace->startingfrom[1];
	GLfloat startingfrom_z = pVSpace->startingfrom[2];

	//	Create a 3d array to varify if its on the vid_list
	vector < vector< vector< bool > > > IsThisVoxelBelongToVidList;
	//	Resize IsThisVoxelBelongToVidList[resolution_x][resolution_y][resolution_z]
	IsThisVoxelBelongToVidList.resize(resolution_x);
	for (int i = 0; i < resolution_x; ++i) {
		IsThisVoxelBelongToVidList[i].resize(resolution_y);
		for (int j = 0; j < resolution_y; ++j)
			IsThisVoxelBelongToVidList[i][j].resize(resolution_z);
	}
	// Initilize IsThisVoxelBelongToVidList[][][]
	for (size_t x = 0; x < resolution_x; x++)
	{
		for (size_t y = 0; y < resolution_y; y++)
		{
			for (size_t z = 0; z < resolution_z; z++)
			{
				IsThisVoxelBelongToVidList[x][y][z] = false;
			}
		}
	}
	// Assign value to IsThisVoxelBelongToVidList[][][]
	for (size_t i = 0; i < Vid_list.size(); i++)
	{
		if (pVSpace->IsThisVoxelIDSafe(Vid_list[i].x, Vid_list[i].y, Vid_list[i].z))
		{
			IsThisVoxelBelongToVidList[Vid_list[i].x][Vid_list[i].y][Vid_list[i].z] = true;
		}
	}

	//	Now, for each voxel in voxel_id_list,check each face if its outside
	for (int i = 0; i < Vid_list.size(); i++)
	{
		int vid_x = Vid_list[i].x;
		int vid_y = Vid_list[i].y;
		int vid_z = Vid_list[i].z;
		//check if id is legal
		if (pVSpace->IsThisVoxelIDSafe(vid_x, vid_y, vid_z))
		{
			//get the color and vertex from VSmatrix
			VoxelColor color = { Vid_list_color[i].R, Vid_list_color[i].G, Vid_list_color[i].B, Vid_list_color[i].A };
			float vox_size[3] = { voxelsize_x, voxelsize_y, voxelsize_z };

			//    v4----- v5
			//   /|      /|
			//  v6------v7|
			//  | |     | |
			//  | |v0---|-|v1
			//  |/      |/
			//  v2------v3
			GLfloat x_max, x_min, y_max, y_min, z_max, z_min;
			x_min = startingfrom_x + vid_x * vox_size[0];
			x_max = startingfrom_x + (vid_x + 1) * vox_size[0];
			y_min = startingfrom_y + vid_y * vox_size[1];
			y_max = startingfrom_y + (vid_y + 1) * vox_size[1];
			z_min = startingfrom_z + vid_z * vox_size[2];
			z_max = startingfrom_z + (vid_z + 1) * vox_size[2];

			// x+1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x + 1, vid_y, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeRight(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x + 1][vid_y][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeRight(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//x-1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x - 1, vid_y, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeLeft(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x - 1][vid_y][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeLeft(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//y+1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y + 1, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeTop(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y + 1][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeTop(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//y-1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y - 1, vid_z))
			{
				//if it out of range, draw it
				glvmPushCubeDown(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y - 1][vid_z]){
				//if it not belong to vid_list, draw it
				glvmPushCubeDown(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//z+1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y, vid_z + 1))
			{
				//if it out of range, draw it
				glvmPushCubeFront(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y][vid_z + 1]){
				//if it not belong to vid_list, draw it
				glvmPushCubeFront(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			//z-1
			if (!pVSpace->IsThisVoxelIDSafe(vid_x, vid_y, vid_z - 1))
			{
				//if it out of range, draw it
				glvmPushCubeEnd(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
			else if (!IsThisVoxelBelongToVidList[vid_x][vid_y][vid_z - 1]){
				//if it not belong to vid_list, draw it
				glvmPushCubeEnd(x_min, y_min, z_min, x_max, y_max, z_max, color.R, color.G, color.B, color.A, VModel);
			}
		}
	}

	////	compute bounding box
	////	眔 Vid_list 程程xyz だ秖―家程程
	//int MAX_vid_x = Vid_list[0].x;
	//int MIN_vid_x = Vid_list[0].x;
	//int MAX_vid_y = Vid_list[0].y;
	//int MIN_vid_y = Vid_list[0].y;
	//int MAX_vid_z = Vid_list[0].z;
	//int MIN_vid_z = Vid_list[0].z;
	//for (int i = 0; i < Vid_list.size(); i++)
	//{
	//	if (Vid_list[i].x > MAX_vid_x)
	//	{
	//		MAX_vid_x = Vid_list[i].x;
	//	}
	//	if (Vid_list[i].x < MIN_vid_x)
	//	{
	//		MIN_vid_x = Vid_list[i].x;
	//	}
	//	if (Vid_list[i].y > MAX_vid_y)
	//	{
	//		MAX_vid_y = Vid_list[i].y;
	//	}
	//	if (Vid_list[i].y < MIN_vid_y)
	//	{
	//		MIN_vid_y = Vid_list[i].y;
	//	}
	//	if (Vid_list[i].z > MAX_vid_z)
	//	{
	//		MAX_vid_z = Vid_list[i].z;
	//	}
	//	if (Vid_list[i].z < MIN_vid_z)
	//	{
	//		MIN_vid_z = Vid_list[i].z;
	//	}
	//}

	//VModel->x_min = startingfrom_x + MIN_vid_x * voxelsize_x;
	//VModel->x_max = startingfrom_x + (MAX_vid_x + 1) * voxelsize_x;
	//VModel->y_min = startingfrom_y + MIN_vid_y * voxelsize_y;
	//VModel->y_max = startingfrom_y + (MAX_vid_y + 1) * voxelsize_y;
	//VModel->z_min = startingfrom_z + MIN_vid_z * voxelsize_z;
	//VModel->z_max = startingfrom_z + (MAX_vid_z + 1) * voxelsize_z;

	//	compute bounding box
	VModel->x_min = startingfrom_x;
	VModel->x_max = startingfrom_x + (resolution_x + 1) * voxelsize_x;
	VModel->y_min = startingfrom_y;
	VModel->y_max = startingfrom_y + (resolution_y + 1) * voxelsize_y;
	VModel->z_min = startingfrom_z;
	VModel->z_max = startingfrom_z + (resolution_z + 1) * voxelsize_z;

	// compute the length for the longest edge of bounding box
	VModel->maxBndLen = glvmCalculatemaxBndLen(VModel->x_min, VModel->y_min, VModel->z_min, VModel->x_max, VModel->y_max, VModel->z_max);

	// object center : center of bounding box
	VModel->center[0] = (VModel->x_min + VModel->x_max) / 2.0;
	VModel->center[1] = (VModel->y_min + VModel->y_max) / 2.0;
	VModel->center[2] = (VModel->z_min + VModel->z_max) / 2.0;

	return VModel;
}