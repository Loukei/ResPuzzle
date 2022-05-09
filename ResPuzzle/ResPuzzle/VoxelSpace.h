#include "Voxel.h"
#include <vector>

using std::vector;
#pragma once

/// <summary>
/// CVoxelSpace �y�z:
/// �\��:�����ôy�z������(voxel)���ҫ�(�H.json�ɮ׿�J)�A�åB���Ѥ@�Ӥ�����openglø�s���I(�������O��XGLVoxelModel���c)
///		�ô��Ѥ@�ǭp��accessibility����k...
/// </summary>
class CVoxelSpace
{
public:
	const char* pathname;

	//�@�ӤT�����а}�C�A�C�Ӱ}�C���������V������CVoxel���e
	std::vector < vector< vector< CVoxel* > > > VSmatrix;

	std::vector <CVoxel> shell_voxels;
	std::vector <CVoxel> inside_voxels;

	//VSmatrix�x�}���j�p
	int resolution_x; //x
	int resolution_y; //y
	int resolution_z; //z

	float startingfrom[3]; //�_�l�I��m�A�Ybounding box�̤p��
	float voxelsize[3]; //�C��voxel���j�p(���T�w�@��)

	//��h����P��������ƥ�
	int NumOFShellvoxel;
	int NumOFInsidevoxel;

	CVoxelSpace();
	~CVoxelSpace();
private:
	// Allocate and inilize VSmatrix
	bool AllocateAndInilizeVSmatrix(int resolution_x, int resolution_y, int resolution_z);
	
public:
	// loading voxel model data from .json file and store in voxelspace
	void ReadJSON(const char* filename);
	// �ˬd���w��ID�O�_�bVSmatrix�d��
	bool IsThisVoxelIDSafe(int vid_x, int vid_y, int vid_z);
	// calculate which voxel will be intersect on VoxelSpace
	std::vector<VoxelID> CVoxelSpace::WhichVoxelWillIntersect(float point_x, float point_y, float point_z);

	bool FillInInsideVoxel();
};

//�[�J�p��̵u���|�t��k�A�[�Jpoint��voxel���̵u�Z��

/// EXAMPLE:
/*
	#include "VoxelSpace.h"
	...
	CVoxelSpace vs;
	vs.ReadJSON("inputs/al.json");

	int x, y, z;
	x = 0;
	y = 10;
	z = 17;

	VoxelType type = vs.VSmatrix[x][y][z]->getType();
	VoxelColor color = vs.VSmatrix[x][y][z]->getColor();
	float accesibility = vs.VSmatrix[x][y][z]->getAccessibility();
	*/