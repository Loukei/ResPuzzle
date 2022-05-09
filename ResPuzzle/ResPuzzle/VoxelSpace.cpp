#include "stdafx.h"
#include "VoxelSpace.h"
#include "jsoncons/json.hpp"
#include <math.h>
#include <time.h>
#include <queue>

using jsoncons::json;
using namespace std;

CVoxelSpace::CVoxelSpace()
{
	// Allocate everything
	this->pathname = nullptr;

	this->resolution_x = 1;	//x
	this->resolution_y = 1;	//y
	this->resolution_z = 1;	//z

	this->startingfrom[0] = 0.0f;
	this->startingfrom[1] = 0.0f;
	this->startingfrom[2] = 0.0f;

	this->voxelsize[0] = 0.0f;
	this->voxelsize[1] = 0.0f;
	this->voxelsize[2] = 0.0f;

	this->NumOFShellvoxel = 0;
	this->NumOFInsidevoxel = 0;

	AllocateAndInilizeVSmatrix(this->resolution_x, this->resolution_y, this->resolution_z);
}

CVoxelSpace::~CVoxelSpace()
{
}

// Allocate and inilize VSmatrix
bool CVoxelSpace::AllocateAndInilizeVSmatrix(int resolution_x, int resolution_y, int resolution_z)
{
	// Only accept for unsigned resolution
	if ((resolution_x < 0) || (resolution_y < 0) || (resolution_z < 0)){
		return false;
	}
	// Set up sizes VSmatrix[x][y][z]
	this->VSmatrix.resize(resolution_x);
	for (int i = 0; i < resolution_x; ++i) {
		this->VSmatrix[i].resize(resolution_y);

		for (int j = 0; j < resolution_y; ++j)
			this->VSmatrix[i][j].resize(resolution_z);
	}

	//	Initialize VSmatrix[x][y][z]
	for (int i = 0; i < resolution_x; i++)
	{
		for (int j = 0; j < resolution_y; j++)
		{
			for (int k = 0; k < resolution_z; k++)
			{
				this->VSmatrix[i][j][k] = nullptr;
			}
		}
	}
	return true;
}

bool CVoxelSpace::FillInInsideVoxel()
{
	bool isSuccess;
	std::queue<VoxelID> q;

	//	a 3d array define if voxel is visited
	std::vector<vector<vector<bool>>> visit;
	visit.resize(this->resolution_x);
	for (int x = 0; x < this->resolution_x; x++)
	{
		visit[x].resize(this->resolution_y);
		for (int y = 0; y < this->resolution_y; y++)
		{
			visit[x][y].resize(this->resolution_x, false);
		}
	}

	//	randomly push inside voxel as start
	srand(time(NULL));
	int vi = rand() % (this->inside_voxels.size());

	VoxelID start = this->inside_voxels[vi].getID();
	visit[start.x][start.y][start.z] = true;
	q.push(start);

	while (!q.empty())
	{
		// push 一個voxel
		VoxelID from = q.front();
		q.pop();

		//	將六鄰域中不是shell voxel的加入queue並設為拜訪過
		if (this->IsThisVoxelIDSafe(from.x + 1, from.y, from.z)
			&& !visit[from.x + 1][from.y][from.z]
			&& this->VSmatrix[from.x + 1][from.y][from.z] != nullptr
			&& this->VSmatrix[from.x + 1][from.y][from.z]->getType() != SHELL)
		{
			visit[from.x + 1][from.y][from.z] = true;
			q.push({ from.x + 1, from.y, from.z });
		}
		if (this->IsThisVoxelIDSafe(from.x - 1, from.y, from.z)
			&& !visit[from.x - 1][from.y][from.z]
			&& this->VSmatrix[from.x - 1][from.y][from.z] != nullptr
			&& this->VSmatrix[from.x - 1][from.y][from.z]->getType() != SHELL)
		{
			visit[from.x - 1][from.y][from.z] = true;
			q.push({ from.x - 1, from.y, from.z });
		}
		if (this->IsThisVoxelIDSafe(from.x, from.y + 1, from.z)
			&& !visit[from.x][from.y + 1][from.z]
			&& this->VSmatrix[from.x][from.y + 1][from.z] != nullptr
			&& this->VSmatrix[from.x][from.y + 1][from.z]->getType() != SHELL)
		{
			visit[from.x][from.y + 1][from.z] = true;
			q.push({ from.x, from.y + 1, from.z });
		}
		if (this->IsThisVoxelIDSafe(from.x, from.y - 1, from.z)
			&& !visit[from.x][from.y - 1][from.z]
			&& this->VSmatrix[from.x][from.y - 1][from.z] != nullptr
			&& this->VSmatrix[from.x][from.y - 1][from.z]->getType() != SHELL)
		{
			visit[from.x][from.y - 1][from.z] = true;
			q.push({ from.x, from.y - 1, from.z });
		}
		if (this->IsThisVoxelIDSafe(from.x, from.y, from.z + 1)
			&& !visit[from.x][from.y][from.z + 1]
			&& this->VSmatrix[from.x][from.y][from.z + 1] != nullptr
			&& this->VSmatrix[from.x][from.y][from.z + 1]->getType() != SHELL)
		{
			visit[from.x][from.y][from.z + 1] = true;
			q.push({ from.x, from.y, from.z + 1 });
		}
		if (this->IsThisVoxelIDSafe(from.x, from.y, from.z - 1)
			&& !visit[from.x][from.y][from.z - 1]
			&& this->VSmatrix[from.x][from.y][from.z - 1] != nullptr
			&& this->VSmatrix[from.x][from.y][from.z - 1]->getType() != SHELL)
		{
			visit[from.x][from.y][from.z - 1] = true;
			q.push({ from.x, from.y, from.z - 1 });
		}
	}

	// 檢查更新過後inside voxel的數目，如果 outside voxel數目為0表示失敗
	int sum_of_all_voxel = (this->resolution_x)*(this->resolution_y)*(this->resolution_z);
	int sum_of_inside_voxel = 0;
	for (int x = 0; x < this->resolution_x; x++)
	{
		for (int y = 0; y < this->resolution_y; y++)
		{
			for (int z = 0; z < this->resolution_z; z++)
			{
				if (visit[x][y][z]) sum_of_inside_voxel++;
			}
		}
	}
	if (sum_of_all_voxel - (sum_of_inside_voxel + this->shell_voxels.size()) == 0)
	{
		std::cout << "[ERROR]: outside voxel is" << sum_of_all_voxel - (sum_of_inside_voxel + this->shell_voxels.size()) << std::endl;
		return false;
	}

	//	確認數目沒錯才會更新
	this->inside_voxels.clear();	//清空inside voxel
	for (int x = 0; x < this->resolution_x; x++)
	{
		for (int y = 0; y < this->resolution_y; y++)
		{
			for (int z = 0; z < this->resolution_z; z++)
			{
				if (visit[x][y][z])
				{
					VoxelColor color = {
						0,	//R
						0,	//G
						0,	//B
						0,	//A
					};
					this->inside_voxels.push_back(CVoxel({ x, y, z }, INSIDE, color));
				}
			}
		}
	}
	for (int i = 0; i < this->inside_voxels.size(); i++)
	{
		VoxelID tmp_id = inside_voxels[i].getID();
		VSmatrix[tmp_id.x][tmp_id.y][tmp_id.z] = &inside_voxels[i];
	}

	return true;
}

// loading voxel model data from .json file and store in voxelspace
void CVoxelSpace::ReadJSON(const char* filename)
{
	json obj = json::parse_file(filename);
	std::cout << "[MESSAGE]Try to load file:" << filename << "..." << endl;

	this->pathname = filename;

	this->resolution_x = obj["ResolutionXYZ"].as<std::vector<int>>()[0];
	this->resolution_y = obj["ResolutionXYZ"].as<std::vector<int>>()[1];
	this->resolution_z = obj["ResolutionXYZ"].as<std::vector<int>>()[2];

	this->startingfrom[0] = obj["StartingFrom"].as<std::vector<double>>()[0];
	this->startingfrom[1] = obj["StartingFrom"].as<std::vector<double>>()[1];
	this->startingfrom[2] = obj["StartingFrom"].as<std::vector<double>>()[2];

	this->voxelsize[0] = obj["VoxelSIZE"].as<std::vector<double>>()[0];
	this->voxelsize[1] = obj["VoxelSIZE"].as<std::vector<double>>()[1];
	this->voxelsize[2] = obj["VoxelSIZE"].as<std::vector<double>>()[2];

	AllocateAndInilizeVSmatrix(this->resolution_x, this->resolution_y, this->resolution_z);

	this->NumOFShellvoxel = obj["ShellVoxel"].size();
	this->NumOFInsidevoxel = obj["InsideVoxel"].size();

	//	read for shall voxel
	for (size_t i = 0; i < obj["ShellVoxel"].size(); i++)
	{
		json& voxel = obj["ShellVoxel"][i];

		int x = voxel["Position"].as<std::vector<int>>()[0];
		int y = voxel["Position"].as<std::vector<int>>()[1];
		int z = voxel["Position"].as<std::vector<int>>()[2];

		VoxelColor color = {
			voxel["RGBAcolor"].as<std::vector<int>>()[0],	//R
			voxel["RGBAcolor"].as<std::vector<int>>()[1],	//G
			voxel["RGBAcolor"].as<std::vector<int>>()[2],	//B
			voxel["RGBAcolor"].as<std::vector<int>>()[3],	//A
		};

		this->shell_voxels.push_back(CVoxel({ x, y, z }, SHELL, color));
	}

	//	read for inside voxel
	for (size_t i = 0; i < obj["InsideVoxel"].size(); i++)
	{
		json& voxel = obj["InsideVoxel"][i];

		int x = voxel["Position"].as<std::vector<int>>()[0];
		int y = voxel["Position"].as<std::vector<int>>()[1];
		int z = voxel["Position"].as<std::vector<int>>()[2];

		VoxelColor color = {
			0,	//R
			0,	//G
			0,	//B
			0,	//A
		};

		this->inside_voxels.push_back(CVoxel({ x, y, z }, INSIDE, color));
	}

	// assign OUTSIDE voxel (all outside voxel will assign to same voxel address for space efficiency )
	CVoxel* ptrOutsideVoxel = new CVoxel({ -1. - 1. - 1 }, OUTSIDE, { 255, 255, 255, 0 });
	for (size_t x = 0; x < this->resolution_x; x++)
	{
		for (size_t y = 0; y < this->resolution_y; y++)
		{
			for (size_t z = 0; z < this->resolution_z; z++)
			{
				VSmatrix[x][y][z] = ptrOutsideVoxel;
			}
		}
	}

	for (size_t i = 0; i < this->shell_voxels.size(); i++)
	{
		VoxelID tmp_id = shell_voxels[i].getID();
		VSmatrix[tmp_id.x][tmp_id.y][tmp_id.z] = &shell_voxels[i];
	}

	for (size_t i = 0; i < this->inside_voxels.size(); i++)
	{
		VoxelID tmp_id = inside_voxels[i].getID();
		VSmatrix[tmp_id.x][tmp_id.y][tmp_id.z] = &inside_voxels[i];
	}
	std::cout << "[MESSAGE]Loading complete:" << endl;
	std::cout << "==============================" << endl
		<< "RESOLUTION:" << "[" << this->resolution_x << "," << this->resolution_y << "," << this->resolution_z << "]" << endl
		<< "SIZE:" << "[" << this->voxelsize[0] << "," << this->voxelsize[1] << "," << this->voxelsize[2] << "]" << endl
		<< "NUM OF INSIDE VOXEL:" << this->inside_voxels.size() << endl
		<< "NUM OF SHELL VOXEL:" << this->shell_voxels.size() << endl;
}

// 檢查指定的ID是否在VSmatrix範圍內
bool CVoxelSpace::IsThisVoxelIDSafe(int vid_x, int vid_y, int vid_z)
{
	int Xmax = this->resolution_x - 1;
	int Ymax = this->resolution_y - 1;
	int Zmax = this->resolution_z - 1;

	if ((vid_x >= 0) && (vid_x <= Xmax) && (vid_y >= 0) && (vid_y <= Ymax) && (vid_z >= 0) && (vid_z <= Zmax))
	{
		return true;
	}
	else
	{
		return false;
	}
}


// calculate which voxel will be intersect on VoxelSpace
//std::vector<VoxelID> CVoxelSpace::WhichVoxelWillIntersect(float point_x, float point_y, float point_z)
//{
//	vector<VoxelID> intersectVoxelID;
//
//	int vidMIN_x, vidMAX_x;
//	int vidMIN_y, vidMAX_y;
//	int vidMIN_z, vidMAX_z;
//
//	/*x_min = startingfrom_x + vid_x * vox_size[0];
//	x_max = startingfrom_x + (vid_x + 1) * vox_size[0];
//	y_min = startingfrom_y + vid_y * vox_size[1];
//	y_max = startingfrom_y + (vid_y + 1) * vox_size[1];
//	z_min = startingfrom_z + vid_z * vox_size[2];
//	z_max = startingfrom_z + (vid_z + 1) * vox_size[2];*/
//
//	float startingfrom_x = this->startingfrom[0];
//	float startingfrom_y = this->startingfrom[1];
//	float startingfrom_z = this->startingfrom[2];
//
//	float vox_size_x = this->voxelsize[0];
//	float vox_size_y = this->voxelsize[1];
//	float vox_size_z = this->voxelsize[2];
//
//	for (size_t i = 0; i < resolution_x; i++)
//	{
//		float MIN_x = ;
//		float MAX_x = ;
//	}
//
//	for (size_t i = 0; i < resolution_y; i++)
//	{
//		float MIN_y = ;
//		float MAX_y = ;
//	}
//
//	for (size_t i = 0; i < resolution_z; i++)
//	{
//		float MIN_z = ;
//		float MAX_z = ;
//	}
//
//	return intersectVoxelID;
//}
