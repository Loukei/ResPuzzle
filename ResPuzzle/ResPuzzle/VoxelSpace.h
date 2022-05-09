#include "Voxel.h"
#include <vector>

using std::vector;
#pragma once

/// <summary>
/// CVoxelSpace 描述:
/// 功能:紀錄並描述整個體素(voxel)的模型(以.json檔案輸入)，並且提供一個介面讓opengl繪製頂點(此部分是輸出GLVoxelModel結構)
///		並提供一些計算accessibility的方法...
/// </summary>
class CVoxelSpace
{
public:
	const char* pathname;

	//一個三維指標陣列，每個陣列元素都指向對應的CVoxel內容
	std::vector < vector< vector< CVoxel* > > > VSmatrix;

	std::vector <CVoxel> shell_voxels;
	std::vector <CVoxel> inside_voxels;

	//VSmatrix矩陣的大小
	int resolution_x; //x
	int resolution_y; //y
	int resolution_z; //z

	float startingfrom[3]; //起始點位置，即bounding box最小值
	float voxelsize[3]; //每個voxel的大小(都固定一樣)

	//表層體素與內部體素數目
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
	// 檢查指定的ID是否在VSmatrix範圍內
	bool IsThisVoxelIDSafe(int vid_x, int vid_y, int vid_z);
	// calculate which voxel will be intersect on VoxelSpace
	std::vector<VoxelID> CVoxelSpace::WhichVoxelWillIntersect(float point_x, float point_y, float point_z);

	bool FillInInsideVoxel();
};

//加入計算最短路徑演算法，加入point到voxel的最短距離

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