#include "VoxelSpace.h"
#pragma once

struct vec3f
{
	float x;
	float y;
	float z;
};

struct colorgroup
{
	unsigned short int id;
	std::vector<VoxelID> voxel_list;

	int sum_of_voxel;
	float sum_of_accessibility;

	colorgroup* ptr_nextsamecolor;
};

struct remainelement
{
	VoxelID id;
	int colorgroup_id;
	float accessibility;
	int distance;
};

struct remain_data
{
	int iterative;
	std::vector<remainelement> shell_voxels;
	std::vector<remainelement> inside_voxels;
	std::vector<colorgroup> colorgroup_list;

	//	to calculate distance transform, initialize be shell voxel
	std::vector<VoxelID> border_voxels;

	// we need neighbors of last puzzle, initialize be {}
	std::vector<VoxelID> last_neighbors;

	unsigned int resolution[3];
	std::vector<vector<vector<remainelement*>>> matrix;
};

// 兩個之間值相反代表反向,絕對值不同則垂直
enum MovingDirection { Xplus = 1, Xminus = -1, Yplus = 2, Yminus = -2, Zplus = 3, Zminus = -3 };

struct PuzzlePiece
{
	MovingDirection direction;
	std::vector<VoxelID> voxel_list;
	VoxelID seedvoxel_id;
	//record each direction's blocking voxel (could be same voxel)
	VoxelID blockXp;	//x+
	VoxelID blockXm;	//x-
	VoxelID blockYp;	//y+
	VoxelID blockYm;	//y-
	VoxelID blockZp;	//z+
	VoxelID blockZm;	//z-
};