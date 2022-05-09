#include "stdafx.h"
#include "ResPuzzleController.h"
#include "jsoncons/json.hpp"
#include <math.h> //pow
//#include <queue> //std::queue
#include <time.h> //time
#include <algorithm> //sort

void InitializeRemainData(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	// set iterative to 0
	ptr_rdata->iterative = 0;
	// load shell voxel
	ptr_rdata->shell_voxels.resize(ptr_VSpace->shell_voxels.size());
	for (int i = 0; i < ptr_VSpace->shell_voxels.size(); i++)
	{
		VoxelID tmp_id = ptr_VSpace->shell_voxels[i].getID();
		int tmp_colorgroup_id = -1;
		float tmp_accessibility = 0.0f;
		int distance = -1;
		ptr_rdata->shell_voxels[i] = { tmp_id, tmp_colorgroup_id, tmp_accessibility, distance };
	}
	// load inside voxel
	ptr_rdata->inside_voxels.resize(ptr_VSpace->inside_voxels.size());
	for (int i = 0; i < ptr_VSpace->inside_voxels.size(); i++)
	{
		VoxelID tmp_id = ptr_VSpace->inside_voxels[i].getID();
		int tmp_colorgroup_id = -1;
		float tmp_accessibility = 0.0f;
		int distance = -1;
		ptr_rdata->inside_voxels[i] = { tmp_id, tmp_colorgroup_id, tmp_accessibility, distance };
	}
	//	push voxels pointer into matrix
	ptr_rdata->resolution[0] = ptr_VSpace->resolution_x;
	ptr_rdata->resolution[1] = ptr_VSpace->resolution_y;
	ptr_rdata->resolution[2] = ptr_VSpace->resolution_z;
	//
	ptr_rdata->matrix.resize(ptr_rdata->resolution[0]);
	for (int x = 0; x < ptr_rdata->resolution[0]; x++)
	{
		ptr_rdata->matrix[x].resize(ptr_rdata->resolution[1]);
		for (int y = 0; y < ptr_rdata->resolution[1]; y++)
		{
			ptr_rdata->matrix[x][y].resize(ptr_rdata->resolution[2]);
		}
	}

	for (int x = 0; x < ptr_rdata->resolution[0]; x++)
	{
		for (int y = 0; y < ptr_rdata->resolution[1]; y++)
		{
			for (int z = 0; z < ptr_rdata->resolution[2]; z++)
			{
				ptr_rdata->matrix[x][y][z] = nullptr;
			}
		}
	}

	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		int x = ptr_rdata->shell_voxels[i].id.x;
		int y = ptr_rdata->shell_voxels[i].id.y;
		int z = ptr_rdata->shell_voxels[i].id.z;
		ptr_rdata->matrix[x][y][z] = &ptr_rdata->shell_voxels[i];
	}

	for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	{
		int x = ptr_rdata->inside_voxels[i].id.x;
		int y = ptr_rdata->inside_voxels[i].id.y;
		int z = ptr_rdata->inside_voxels[i].id.z;
		ptr_rdata->matrix[x][y][z] = &ptr_rdata->inside_voxels[i];
	}

	// initilize border_voxels be shell voxels
	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		int x = ptr_rdata->shell_voxels[i].id.x;
		int y = ptr_rdata->shell_voxels[i].id.y;
		int z = ptr_rdata->shell_voxels[i].id.z;
		ptr_rdata->border_voxels.push_back({ x, y, z });
	}
}

void CalculateAccessibility(int recursion, float weight, CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	// clear data
	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		ptr_rdata->shell_voxels[i].accessibility = 0.0f;
	}
	for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	{
		ptr_rdata->inside_voxels[i].accessibility = 0.0f;
	}

	// j = 0,accessibility = number of neighbors
	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		int x = ptr_rdata->shell_voxels[i].id.x;
		int y = ptr_rdata->shell_voxels[i].id.y;
		int z = ptr_rdata->shell_voxels[i].id.z;
		//if neighbors is safe id and in rest data
		if (ptr_VSpace->IsThisVoxelIDSafe(x + 1, y, z) && ptr_rdata->matrix[x + 1][y][z] != nullptr && (ptr_VSpace->VSmatrix[x + 1][y][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x + 1][y][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z) && ptr_rdata->matrix[x - 1][y][z] != nullptr && (ptr_VSpace->VSmatrix[x - 1][y][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x - 1][y][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z) && ptr_rdata->matrix[x][y + 1][z] != nullptr && (ptr_VSpace->VSmatrix[x][y + 1][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y + 1][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z) && ptr_rdata->matrix[x][y - 1][z] != nullptr && (ptr_VSpace->VSmatrix[x][y - 1][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y - 1][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1) && ptr_rdata->matrix[x][y][z + 1] != nullptr && (ptr_VSpace->VSmatrix[x][y][z + 1]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y][z + 1]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z - 1) && ptr_rdata->matrix[x][y][z - 1] != nullptr && (ptr_VSpace->VSmatrix[x][y][z - 1]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y][z - 1]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
	}
	for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	{
		int x = ptr_rdata->inside_voxels[i].id.x;
		int y = ptr_rdata->inside_voxels[i].id.y;
		int z = ptr_rdata->inside_voxels[i].id.z;
		//if neighbors is safe id and in rest data
		if (ptr_VSpace->IsThisVoxelIDSafe(x + 1, y, z) && ptr_rdata->matrix[x + 1][y][z] != nullptr && ptr_VSpace->VSmatrix[x + 1][y][z]->getType() == INSIDE)
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z) && ptr_rdata->matrix[x - 1][y][z] != nullptr && ptr_VSpace->VSmatrix[x - 1][y][z]->getType() == INSIDE)
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z) && ptr_rdata->matrix[x][y + 1][z] != nullptr && ptr_VSpace->VSmatrix[x][y + 1][z]->getType() == INSIDE)
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z) && ptr_rdata->matrix[x][y - 1][z] != nullptr && ptr_VSpace->VSmatrix[x][y - 1][z]->getType() == INSIDE)
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1) && ptr_rdata->matrix[x][y][z + 1] != nullptr && ptr_VSpace->VSmatrix[x][y][z + 1]->getType() == INSIDE)
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z - 1) && ptr_rdata->matrix[x][y][z - 1] != nullptr && ptr_VSpace->VSmatrix[x][y][z - 1]->getType() == INSIDE)
		{
			ptr_rdata->matrix[x][y][z]->accessibility += 1.0f;
		}
	}

	// j>0
	for (int j = 1; j < recursion; j++)
	{
		for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
		{
			int x = ptr_rdata->shell_voxels[i].id.x;
			int y = ptr_rdata->shell_voxels[i].id.y;
			int z = ptr_rdata->shell_voxels[i].id.z;
			if (ptr_VSpace->IsThisVoxelIDSafe(x + 1, y, z) && ptr_rdata->matrix[x + 1][y][z] != nullptr && (ptr_VSpace->VSmatrix[x + 1][y][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x + 1][y][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x + 1][y][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z) && ptr_rdata->matrix[x - 1][y][z] != nullptr && (ptr_VSpace->VSmatrix[x - 1][y][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x - 1][y][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x - 1][y][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z) && ptr_rdata->matrix[x][y + 1][z] != nullptr && (ptr_VSpace->VSmatrix[x][y + 1][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y + 1][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y + 1][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z) && ptr_rdata->matrix[x][y - 1][z] != nullptr && (ptr_VSpace->VSmatrix[x][y - 1][z]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y - 1][z]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y - 1][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1) && ptr_rdata->matrix[x][y][z + 1] != nullptr && (ptr_VSpace->VSmatrix[x][y][z + 1]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y][z + 1]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y][z + 1]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z - 1) && ptr_rdata->matrix[x][y][z - 1] != nullptr && (ptr_VSpace->VSmatrix[x][y][z - 1]->getType() == INSIDE || (ptr_VSpace->VSmatrix[x][y][z - 1]->getColor() == ptr_VSpace->VSmatrix[x][y][z]->getColor())))
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y][z - 1]->accessibility;
			}
		}
		for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
		{
			int x = ptr_rdata->inside_voxels[i].id.x;
			int y = ptr_rdata->inside_voxels[i].id.y;
			int z = ptr_rdata->inside_voxels[i].id.z;
			if (ptr_VSpace->IsThisVoxelIDSafe(x + 1, y, z) && ptr_rdata->matrix[x + 1][y][z] != nullptr && ptr_VSpace->VSmatrix[x + 1][y][z]->getType() == INSIDE)
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x + 1][y][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z) && ptr_rdata->matrix[x - 1][y][z] != nullptr && ptr_VSpace->VSmatrix[x - 1][y][z]->getType() == INSIDE)
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x - 1][y][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z) && ptr_rdata->matrix[x][y + 1][z] != nullptr && ptr_VSpace->VSmatrix[x][y + 1][z]->getType() == INSIDE)
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y + 1][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z) && ptr_rdata->matrix[x][y - 1][z] != nullptr && ptr_VSpace->VSmatrix[x][y - 1][z]->getType() == INSIDE)
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y - 1][z]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1) && ptr_rdata->matrix[x][y][z + 1] != nullptr && ptr_VSpace->VSmatrix[x][y][z + 1]->getType() == INSIDE)
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y][z + 1]->accessibility;
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z - 1) && ptr_rdata->matrix[x][y][z - 1] != nullptr && ptr_VSpace->VSmatrix[x][y][z - 1]->getType() == INSIDE)
			{
				ptr_rdata->matrix[x][y][z]->accessibility += pow((float)weight, j) * ptr_rdata->matrix[x][y][z - 1]->accessibility;
			}
		}
	}
}

void SeperateColorGroup(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	const int size = ptr_rdata->shell_voxels.size();

	std::vector<bool> visit;
	visit.resize(size, false);

	// adjacency list
	std::vector<vector<int>> adj_list;
	// equal to adj[size]
	adj_list.resize(size);
	//	calculate adjacency list
	for (size_t i = 0; i < size; i++)
	{
		for (size_t j = 0; j < size; j++)
		{
			int i_x = ptr_rdata->shell_voxels[i].id.x;
			int i_y = ptr_rdata->shell_voxels[i].id.y;
			int i_z = ptr_rdata->shell_voxels[i].id.z;
			int j_x = ptr_rdata->shell_voxels[j].id.x;
			int j_y = ptr_rdata->shell_voxels[j].id.y;
			int j_z = ptr_rdata->shell_voxels[j].id.z;

			int diff_x = j_x - i_x;
			int diff_y = j_y - i_y;
			int diff_z = j_z - i_z;

			//int length = floor(diff_x) + floor(diff_y) + floor(diff_z);
			float length = pow(diff_x, 2) + pow(diff_y, 2) + pow(diff_z, 2);
			bool IsTheseSamecolor = ISThose2VoxelSameColor(ptr_VSpace->VSmatrix[i_x][i_y][i_z], ptr_VSpace->VSmatrix[j_x][j_y][j_z]);
			if (IsTheseSamecolor && length <= 2 && length != 0)
			{
				adj_list[i].push_back(j);
			}
		}
	}

	///	use BFS algo to get group
	std::queue<int> q;
	int groupid = 0;
	for (size_t root = 0; root < visit.size(); root++)
	{
		if (!visit[root])
		{
			// 一、把起點放入queue。
			q.push(root);
			visit[root] = true;
			colorgroup group;

			// 二、重複下述兩點，直到queue裡面沒有東西為止：
			while (!q.empty())
			{
				//甲、從queue當中取出一點。
				int from = q.front();
				group.voxel_list.push_back(ptr_rdata->shell_voxels[from].id);
				q.pop();

				// 乙、找出跟此點相鄰的點，並且尚未遍歷的點，
				//     依照編號順序通通放入queue。
				for (size_t edge = 0; edge < adj_list[from].size(); edge++)
				{
					int end = adj_list[from][edge];
					if (!visit[end])
					{
						q.push(end);
						visit[end] = true;
					}
				}

			}
			//running out the queue,which means one colorgroup is down

			group.id = groupid;
			group.sum_of_voxel = group.voxel_list.size();
			group.sum_of_accessibility = 0.0f;
			ptr_rdata->colorgroup_list.push_back(group);

			groupid += 1;
		}
	}

	///	fill the group id to each element
	for (int i = 0; i < ptr_rdata->colorgroup_list.size(); i++)
	{
		int tmp_group_id = ptr_rdata->colorgroup_list[i].id;
		for (int j = 0; j < ptr_rdata->colorgroup_list[i].voxel_list.size(); j++)
		{
			int x = ptr_rdata->colorgroup_list[i].voxel_list[j].x;
			int y = ptr_rdata->colorgroup_list[i].voxel_list[j].y;
			int z = ptr_rdata->colorgroup_list[i].voxel_list[j].z;

			ptr_rdata->matrix[x][y][z]->colorgroup_id = tmp_group_id;
		}
	}

}

GLVoxelModel*  CreateModelByEachGroup(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	srand(time(NULL));
	int num_of_group = ptr_rdata->colorgroup_list.size();

	vector<VoxelID> vid_list;
	vector<RGBAcolor> vid_list_color;

	// we don't need to draw inside voxel
	RGBAcolor rand_color = { 0, 0, 0, 255 };
	for (size_t g = 0; g < num_of_group; g++)
	{
		rand_color.R = (rand_color.R + rand()) % 255;
		rand_color.G = (rand_color.G + rand()) % 255;
		rand_color.B = (rand_color.B + rand()) % 255;
		int num_of_voxel = ptr_rdata->colorgroup_list[g].voxel_list.size();
		for (size_t v = 0; v < num_of_voxel; v++)
		{
			vid_list.push_back(ptr_rdata->colorgroup_list[g].voxel_list[v]);
			vid_list_color.push_back(rand_color);
		}
	}

	return CreateModelByID(ptr_VSpace, vid_list, vid_list_color);
}

GLVoxelModel*  CreateModelByOriginal(CVoxelSpace* pVSpace)
{
	vector<VoxelID> vid_list;
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < pVSpace->resolution_z; z++)
			{
				if (pVSpace->VSmatrix[x][y][z]->getType() != OUTSIDE)
				{
					vid_list.push_back({ x, y, z });
				}
			}
		}
	}

	return CreateModelByID(pVSpace, vid_list);
}

GLVoxelModel* CreateModelByDistanceTransform(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	int maxDistance = 10;
	int layer = 17;
	vector<VoxelID> vid_list;
	vector<RGBAcolor> vid_list_color;
	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		if (ptr_rdata->shell_voxels[i].id.z == layer)
		{
			HSVcolor hsv;
			hsv.H = ((float)ptr_rdata->shell_voxels[i].distance / maxDistance)*240.0;
			hsv.S = 1.0;
			hsv.V = 0.5;
			vid_list.push_back(ptr_rdata->shell_voxels[i].id);
			vid_list_color.push_back(glvmHSVtoRGBA(hsv));
		}

	}

	for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	{
		if (ptr_rdata->inside_voxels[i].id.z == layer)
		{
			HSVcolor hsv;
			hsv.H = ((float)ptr_rdata->inside_voxels[i].distance / maxDistance)*240.0;
			hsv.S = 1.0;
			hsv.V = 0.5;
			vid_list.push_back(ptr_rdata->inside_voxels[i].id);
			vid_list_color.push_back(glvmHSVtoRGBA(hsv));
		}
	}

	return CreateModelByID(ptr_VSpace, vid_list, vid_list_color);
}

GLVoxelModel* CreateModelByAccessibility(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	int recursion = 3;
	float weight = 0.1;
	float maxAccessibility = 6.0;
	for (size_t i = 1; i < recursion + 1; i++)
	{
		maxAccessibility *= 1 + 6 * pow(weight, i);
	}
	std::vector<VoxelID> vid_list;
	std::vector<RGBAcolor> vid_list_color;

	for (size_t i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		int x = vid_list[i].x;
		int y = vid_list[i].y;
		int z = vid_list[i].z;

		HSVcolor hsv;
		hsv.H = (GLfloat)(maxAccessibility - ptr_rdata->shell_voxels[i].accessibility) / maxAccessibility * 240.0;
		hsv.S = 1.0;
		hsv.V = 0.5;
		RGBAcolor rgba = glvmHSVtoRGBA(hsv);
		vid_list.push_back(ptr_rdata->shell_voxels[i].id);
		vid_list_color.push_back(rgba);
	}
	for (size_t i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	{
		int x = vid_list[i].x;
		int y = vid_list[i].y;
		int z = vid_list[i].z;

		HSVcolor hsv;
		hsv.H = (GLfloat)(maxAccessibility - ptr_rdata->inside_voxels[i].accessibility) / maxAccessibility * 240.0;
		hsv.S = 1.0;
		hsv.V = 0.5;
		RGBAcolor rgba = glvmHSVtoRGBA(hsv);
		vid_list.push_back(ptr_rdata->inside_voxels[i].id);
		vid_list_color.push_back(rgba);
	}
	return CreateModelByID(ptr_VSpace, vid_list, vid_list_color);
}

GLVoxelModel* CreateModelByPuzzle(CVoxelSpace* ptr_VSpace, PuzzlePiece puzzle)
{
	std::vector<VoxelID> vid_list;
	vector<RGBAcolor> color_list;
	const VoxelID err_id = { -1, -1, -1 };

	//	push all voxel on P1
	for (int i = 0; i < puzzle.voxel_list.size(); i++)
	{
		//	make sure voxel is exist
		if (puzzle.voxel_list[i] != err_id)
		{
			// tag seed voxel
			if (puzzle.voxel_list[i] == puzzle.seedvoxel_id)
			{
				vid_list.push_back(puzzle.voxel_list[i]);
				color_list.push_back({ 255, 0, 0, 255 });
			}
			else
			{
				vid_list.push_back(puzzle.voxel_list[i]);
				color_list.push_back({ 255, 255, 255, 255 });
			}
		}
	}

	//	push and tag block voxel
	const RGBAcolor block_color = { 0, 255, 0, 255 };
	if (puzzle.blockXm != err_id)
	{
		vid_list.push_back(puzzle.blockXm);
		color_list.push_back(block_color);
	}
	if (puzzle.blockXp != err_id)
	{
		vid_list.push_back(puzzle.blockXp);
		color_list.push_back(block_color);
	}
	if (puzzle.blockYm != err_id)
	{
		vid_list.push_back(puzzle.blockYm);
		color_list.push_back(block_color);
	}
	if (puzzle.blockYp != err_id)
	{
		vid_list.push_back(puzzle.blockYp);
		color_list.push_back(block_color);
	}
	if (puzzle.blockZm != err_id)
	{
		vid_list.push_back(puzzle.blockZm);
		color_list.push_back(block_color);
	}
	if (puzzle.blockZp != err_id)
	{
		vid_list.push_back(puzzle.blockZp);
		color_list.push_back(block_color);
	}

	return CreateModelByID(ptr_VSpace, vid_list, color_list);
}

void SortColorGroupByAccessibility(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	// calculate summary of accessibility
	for (size_t i = 0; i < ptr_rdata->colorgroup_list.size(); i++)
	{
		ptr_rdata->colorgroup_list[i].sum_of_accessibility = 0.0f;
		for (size_t j = 0; j < ptr_rdata->colorgroup_list[i].voxel_list.size(); j++)
		{
			int x = ptr_rdata->colorgroup_list[i].voxel_list[j].x;
			int y = ptr_rdata->colorgroup_list[i].voxel_list[j].y;
			int z = ptr_rdata->colorgroup_list[i].voxel_list[j].z;
			float acce = ptr_rdata->matrix[x][y][z]->accessibility;
			ptr_rdata->colorgroup_list[i].sum_of_accessibility += acce;
		}
	}

	/// base on accessibility,sort color group
	sort(ptr_rdata->colorgroup_list.begin(), ptr_rdata->colorgroup_list.end(), ColorgroupCompareByAccessibility);
}

bool ColorgroupCompareByAccessibility(colorgroup a, colorgroup b)
{
	return (a.sum_of_accessibility) < (b.sum_of_accessibility);
}

std::queue<VoxelID> PickCandidateSetForP1(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	std::queue<VoxelID> candidate_set;

	for (size_t i = 1; i < ptr_rdata->colorgroup_list.size(); i++)
	{
		for (size_t j = 0; j < ptr_rdata->colorgroup_list[i].voxel_list.size(); j++)
		{
			int x = ptr_rdata->colorgroup_list[i].voxel_list[j].x;
			int y = ptr_rdata->colorgroup_list[i].voxel_list[j].y;
			int z = ptr_rdata->colorgroup_list[i].voxel_list[j].z;

			bool is_up = !(ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z)) || (ptr_rdata->matrix[x][y + 1][z] == nullptr);
			bool is_down = !(ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z)) || (ptr_rdata->matrix[x][y - 1][z] == nullptr);

			bool is_right = !(ptr_VSpace->IsThisVoxelIDSafe(x + 1, y, z)) || (ptr_rdata->matrix[x + 1][y][z] == nullptr);
			bool is_left = !(ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z)) || (ptr_rdata->matrix[x - 1][y][z] == nullptr);
			bool is_front = !(ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1)) || (ptr_rdata->matrix[x][y][z + 1] == nullptr);
			bool is_back = !(ptr_VSpace->IsThisVoxelIDSafe(x, y, z)) || (ptr_rdata->matrix[x][y][z - 1] == nullptr);
			///	we identify a candidate	set of exterior voxels that have exactly a pair of adjacent exterior faces, with one being on the top
			/// also, if voxel is a isolated (accseeibility = 0) ,which means no color with same group, then no way this voxel can expand
			if (is_up && !is_down &&  ptr_rdata->matrix[x][y][z]->accessibility > 0)
			{
				if (is_right ^ is_left ^ is_front ^ is_back)
				{
					candidate_set.push({ x, y, z });
				}
			}
		}
	}

	return candidate_set;
}

std::queue<VoxelID> PickCandidateSetForP1ByY(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	std::queue<VoxelID> candidate_set;
	std::vector<VoxelID> tmp_vector_list;

	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		int x = ptr_rdata->shell_voxels[i].id.x;
		int y = ptr_rdata->shell_voxels[i].id.y;
		int z = ptr_rdata->shell_voxels[i].id.z;

		bool is_up = !(ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z)) || (ptr_rdata->matrix[x][y + 1][z] == nullptr);
		bool is_down = !(ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z)) || (ptr_rdata->matrix[x][y - 1][z] == nullptr);

		bool is_right = !(ptr_VSpace->IsThisVoxelIDSafe(x + 1, y, z)) || (ptr_rdata->matrix[x + 1][y][z] == nullptr);
		bool is_left = !(ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z)) || (ptr_rdata->matrix[x - 1][y][z] == nullptr);
		bool is_front = !(ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1)) || (ptr_rdata->matrix[x][y][z + 1] == nullptr);
		bool is_back = !(ptr_VSpace->IsThisVoxelIDSafe(x, y, z)) || (ptr_rdata->matrix[x][y][z - 1] == nullptr);

		tmp_vector_list.push_back({ x, y, z });
	}

	//sort by  y value
	sort(tmp_vector_list.begin(), tmp_vector_list.end(), CandidateVoxelCompareByY);

	//copy to output
	for (int i = 0; i < tmp_vector_list.size(); i++)
	{
		candidate_set.push(tmp_vector_list[i]);
	}

	return candidate_set;
}

bool CandidateVoxelCompareByY(VoxelID a, VoxelID b)
{
	return a.y > b.y;
}

PuzzlePiece /*GLVoxelModel**/ CreatePuzzleForP1(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	CalculateAccessibility(3, 0.1, ptr_VSpace, ptr_rdata);
	SeperateColorGroup(ptr_VSpace, ptr_rdata);
	//SortColorGroupByAccessibility(ptr_VSpace, ptr_rdata);
	CalculateDistanceTransform(ptr_VSpace, ptr_rdata);
	//return CreateModelByDistanceTransform(ptr_VSpace, ptr_rdata);


	////	檢查inside voxel
	//std::queue<VoxelID> v_q;
	//bool b = false;
	//for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	//{
	//	int x = ptr_rdata->inside_voxels[i].id.x;
	//	int y = ptr_rdata->inside_voxels[i].id.y;
	//	int z = ptr_rdata->inside_voxels[i].id.z;

	//	if (ptr_VSpace->IsThisVoxelIDSafe(x+1,y,z)
	//		&& ptr_rdata->matrix[x+1][y][z]!=nullptr
	//		&&ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z)
	//		&& ptr_rdata->matrix[x - 1][y][z] != nullptr
	//		&&ptr_VSpace->IsThisVoxelIDSafe(x, y+1, z)
	//		&& ptr_rdata->matrix[x][y+1][z] != nullptr
	//		&&ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z)
	//		&& ptr_rdata->matrix[x][y - 1][z] != nullptr
	//		&&ptr_VSpace->IsThisVoxelIDSafe(x, y, z+1)
	//		&& ptr_rdata->matrix[x][y][z+1] != nullptr
	//		&&ptr_VSpace->IsThisVoxelIDSafe(x, y, z-1)
	//		&& ptr_rdata->matrix[x][y][z-1] != nullptr
	//		)
	//	{
	//		b = true;
	//	}
	//	else
	//	{
	//		std::cout << x << "," << y << "," << z << "	is not inside voxel" << std::endl;
	//		b = false;
	//		v_q.push({x,y,z});
	//		//break;
	//	}
	//}


	PuzzlePiece P1;
	P1.direction = Yplus;
	//std::queue<VoxelID>  candidate_seed_voxel = PickCandidateSetForP1(ptr_VSpace, ptr_rdata);
	std::queue<VoxelID>  candidate_seed_voxel = PickCandidateSetForP1ByY(ptr_VSpace, ptr_rdata);

	bool P1_done = false;
	std::vector<VoxelID> P1_expandrange;
	VoxelID block_yn = { -1, -1, -1 };
	VoxelID block_xp = { -1, -1, -1 };
	VoxelID block_xn = { -1, -1, -1 };
	VoxelID block_zp = { -1, -1, -1 };
	VoxelID block_zn = { -1, -1, -1 };
	VoxelID seed_id = { -1, -1, -1 };
	vector<VoxelID> shortest_path;

	while (!candidate_seed_voxel.empty() && !P1_done)
	{
		//	pick a seed voxel which is belong to smallest accessibility group so far
		seed_id = candidate_seed_voxel.front();
		candidate_seed_voxel.pop();

		//=============bad way,put into another function===========
		//prepare visit[][][] to determin if allready push into
		std::vector<vector<vector<bool>>> visit;
		visit.resize(ptr_rdata->resolution[0]);
		for (int x = 0; x < ptr_rdata->resolution[0]; x++)
		{
			visit[x].resize(ptr_rdata->resolution[1]);
			for (int y = 0; y < ptr_rdata->resolution[1]; y++)
			{
				visit[x][y].resize(ptr_rdata->resolution[2]);
			}
		}
		for (int x = 0; x < ptr_rdata->resolution[0]; x++)
		{
			for (int y = 0; y < ptr_rdata->resolution[1]; y++)
			{
				for (int z = 0; z < ptr_rdata->resolution[2]; z++)
				{
					visit[x][y][z] = false;
				}
			}
		}

		//	pick all voxel on seed voxel's group ,which is top-free
		int group_index = getColorGroup(ptr_rdata->matrix[seed_id.x][seed_id.y][seed_id.z]->colorgroup_id, ptr_rdata);
		std::vector<VoxelID> top_free;
		for (int i = 0; i < ptr_rdata->colorgroup_list[group_index].voxel_list.size(); i++)
		{
			int x = ptr_rdata->colorgroup_list[group_index].voxel_list[i].x;
			int y = ptr_rdata->colorgroup_list[group_index].voxel_list[i].y;
			int z = ptr_rdata->colorgroup_list[group_index].voxel_list[i].z;

			if (!ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z) || ptr_rdata->matrix[x][y + 1][z] == nullptr)
			{
				P1_expandrange.push_back({ x, y, z });
				top_free.push_back({ x, y, z });
				visit[x][y][z] == true;
			}

		}
		//	all inside voxel under seed voxel
		for (int i = 0; i < top_free.size(); i++)
		{
			int x = top_free[i].x;
			int y = top_free[i].y;
			int z = top_free[i].z;

			for (int j = y - 1; j > 0 /*< ptr_rdata->resolution[1]*/; j--)
			{
				if (ptr_VSpace->IsThisVoxelIDSafe(x, j, z) == true && ptr_rdata->matrix[x][j][z] != nullptr)
				{
					if (ptr_VSpace->VSmatrix[x][j][z]->getType() == INSIDE
						&& !visit[x][j][z]
						/*&& Vec3fDotProduct({ 0, -1, 0 }, CalculateDistanceGradient({ x, j, z }, ptr_VSpace, ptr_rdata)) > 0
						&& ptr_rdata->matrix[x][j][z]->distance > 1*/)
					{
						P1_expandrange.push_back({ x, j, z });
						visit[x][j][z] = true;
					}
					else if (ptr_VSpace->VSmatrix[x][j][z]->getType() == SHELL
						&& ptr_VSpace->VSmatrix[x][j][z]->getColor() == ptr_VSpace->VSmatrix[seed_id.x][seed_id.y][seed_id.z]->getColor()
						&& !visit[x][j][z])
					{
						P1_expandrange.push_back({ x, j, z });
						visit[x][j][z] = true;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}

		//	find blocking pair
		vector<VoxelID> blocking_pair = FindDesitinationVoxel(seed_id, P1_expandrange, ptr_VSpace);
		VoxelID block_id = blocking_pair[0];
		VoxelID desitination_id = blocking_pair[1];
		// findout the blockee's direction
		if (blocking_pair[0].x > seed_id.x && blocking_pair[0].y == seed_id.y && blocking_pair[0].z == seed_id.z)
		{
			block_xp = blocking_pair[0];
		}
		else if (blocking_pair[0].x < seed_id.x && blocking_pair[0].y == seed_id.y && blocking_pair[0].z == seed_id.z)
		{
			block_xn = blocking_pair[0];
		}
		else if (blocking_pair[0].x == seed_id.x && blocking_pair[0].y < seed_id.y && blocking_pair[0].z == seed_id.z)
		{
			block_yn = blocking_pair[0];
		}
		else if (blocking_pair[0].x == seed_id.x && blocking_pair[0].y == seed_id.y && blocking_pair[0].z > seed_id.z)
		{
			block_zp = blocking_pair[0];
		}
		else if (blocking_pair[0].x == seed_id.x && blocking_pair[0].y == seed_id.y && blocking_pair[0].z < seed_id.z)
		{
			block_zn = blocking_pair[0];
		}

		//	erase the voxel below( y < block.y) the block_id
		for (int i = 0; i < P1_expandrange.size(); i++)
		{
			if (P1_expandrange[i].x == block_id.x && P1_expandrange[i].y < block_id.y && P1_expandrange[i].z == block_id.z)
			{
				P1_expandrange.erase(P1_expandrange.begin() + i);
			}
			else
			{
				break;
			}
		}
		//	find shortest path from seed voxel to block voxel, if no path exist => try next voxel
		vector<VoxelID> path = FindShortestPath(seed_id, desitination_id, P1_expandrange, ptr_VSpace, ptr_rdata);
		if (!path.empty())
		{
			//copy to shortest path
			shortest_path.resize(path.size());
			shortest_path = path;

			P1_done = true;
		}
		else
		{
			P1_done = false;
		}

		//	find other block voxel 
		{
			//	try to get direction on seed voxel
			// y-1
			if (ptr_VSpace->IsThisVoxelIDSafe(seed_id.x, seed_id.y - 1, seed_id.z) == true && ptr_rdata->matrix[seed_id.x][seed_id.y - 1][seed_id.z] != nullptr)
			{
				if (block_id.y >= seed_id.y)
				{
					for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
					{
						if (ptr_rdata->shell_voxels[i].id.x == seed_id.x && ptr_rdata->shell_voxels[i].id.y < seed_id.y && ptr_rdata->shell_voxels[i].id.z == seed_id.z)
						{
							block_yn = ptr_rdata->shell_voxels[i].id;
							break;
						}
					}
					for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
					{
						if (ptr_rdata->inside_voxels[i].id.x == seed_id.x && ptr_rdata->inside_voxels[i].id.y < seed_id.y && ptr_rdata->inside_voxels[i].id.z == seed_id.z)
						{
							block_yn = ptr_rdata->inside_voxels[i].id;
							break;
						}
					}
				}
				else
				{
					block_yn = block_id;
				}
				// erase below
				for (int i = 0; i < P1_expandrange.size(); i++)
				{
					if (P1_expandrange[i].x == block_yn.x && P1_expandrange[i].y < block_yn.y && P1_expandrange[i].z == block_yn.z)
					{
						P1_expandrange.erase(P1_expandrange.begin() + i);
					}
					else
					{
						break;
					}
				}
			}
			P1_expandrange.push_back(block_yn); //temporary
			// x-1
			if (ptr_VSpace->IsThisVoxelIDSafe(seed_id.x - 1, seed_id.y, seed_id.z) == true && ptr_rdata->matrix[seed_id.x - 1][seed_id.y][seed_id.z] != nullptr)
			{
				if (block_id.x >= seed_id.x)
				{
					for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
					{
						if (ptr_rdata->shell_voxels[i].id.x < seed_id.x && ptr_rdata->shell_voxels[i].id.y == seed_id.y && ptr_rdata->shell_voxels[i].id.z == seed_id.z)
						{
							block_xn = ptr_rdata->shell_voxels[i].id;
							break;
						}
					}
					for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
					{
						if (ptr_rdata->inside_voxels[i].id.x < seed_id.x && ptr_rdata->inside_voxels[i].id.y == seed_id.y && ptr_rdata->inside_voxels[i].id.z == seed_id.z)
						{
							block_xn = ptr_rdata->inside_voxels[i].id;
							break;
						}
					}
				}
				else
				{
					block_xn = block_id;
				}

				// erase below
				for (int i = 0; i < P1_expandrange.size(); i++)
				{
					if (P1_expandrange[i].x == block_xn.x && P1_expandrange[i].y < block_xn.y && P1_expandrange[i].z == block_xn.z)
					{
						P1_expandrange.erase(P1_expandrange.begin() + i);
					}
					else
					{
						break;
					}
				}
			}
			P1_expandrange.push_back(block_xn); //temporary
			// x+1
			if (ptr_VSpace->IsThisVoxelIDSafe(seed_id.x + 1, seed_id.y, seed_id.z) == true && ptr_rdata->matrix[seed_id.x + 1][seed_id.y][seed_id.z] != nullptr)
			{
				if (block_id.x <= seed_id.x)
				{
					for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
					{
						if (ptr_rdata->shell_voxels[i].id.x > seed_id.x && ptr_rdata->shell_voxels[i].id.y == seed_id.y && ptr_rdata->shell_voxels[i].id.z == seed_id.z)
						{
							block_xp = ptr_rdata->shell_voxels[i].id;
							break;
						}
					}
					for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
					{
						if (ptr_rdata->inside_voxels[i].id.x > seed_id.x && ptr_rdata->inside_voxels[i].id.y == seed_id.y && ptr_rdata->inside_voxels[i].id.z == seed_id.z)
						{
							block_xp = ptr_rdata->inside_voxels[i].id;
							break;
						}
					}
				}
				else
				{
					block_xp = block_id;
				}

				// erase below
				for (int i = 0; i < P1_expandrange.size(); i++)
				{
					if (P1_expandrange[i].x == block_xp.x && P1_expandrange[i].y < block_xp.y && P1_expandrange[i].z == block_xp.z)
					{
						P1_expandrange.erase(P1_expandrange.begin() + i);
					}
					else
					{
						break;
					}
				}
			}
			P1_expandrange.push_back(block_xp); //temporary
			// z-1
			if (ptr_VSpace->IsThisVoxelIDSafe(seed_id.x, seed_id.y, seed_id.z - 1) == true && ptr_rdata->matrix[seed_id.x][seed_id.y][seed_id.z - 1] != nullptr)
			{
				if (block_id.z >= seed_id.z)
				{
					for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
					{
						if (ptr_rdata->shell_voxels[i].id.x == seed_id.x && ptr_rdata->shell_voxels[i].id.y == seed_id.y && ptr_rdata->shell_voxels[i].id.z < seed_id.z)
						{
							block_zn = ptr_rdata->shell_voxels[i].id;
							break;
						}
					}
					for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
					{
						if (ptr_rdata->inside_voxels[i].id.x == seed_id.x && ptr_rdata->inside_voxels[i].id.y == seed_id.y && ptr_rdata->inside_voxels[i].id.z < seed_id.z)
						{
							block_zn = ptr_rdata->inside_voxels[i].id;
							break;
						}
					}
				}
				else
				{
					block_zn = block_id;
				}

				// erase below
				for (int i = 0; i < P1_expandrange.size(); i++)
				{
					if (P1_expandrange[i].x == block_zn.x && P1_expandrange[i].y < block_zn.y && P1_expandrange[i].z == block_zn.z)
					{
						P1_expandrange.erase(P1_expandrange.begin() + i);
					}
					else
					{
						break;
					}
				}
			}
			P1_expandrange.push_back(block_zn); //temporary
			// z+1
			if (ptr_VSpace->IsThisVoxelIDSafe(seed_id.x, seed_id.y, seed_id.z + 1) == true && ptr_rdata->matrix[seed_id.x][seed_id.y][seed_id.z + 1] != nullptr)
			{
				if (block_id.z <= seed_id.z)
				{
					for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
					{
						if (ptr_rdata->shell_voxels[i].id.x == seed_id.x && ptr_rdata->shell_voxels[i].id.y == seed_id.y && ptr_rdata->shell_voxels[i].id.z > seed_id.z)
						{
							block_zp = ptr_rdata->shell_voxels[i].id;
							break;
						}
					}
					for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
					{
						if (ptr_rdata->inside_voxels[i].id.x == seed_id.x && ptr_rdata->inside_voxels[i].id.y == seed_id.y && ptr_rdata->inside_voxels[i].id.z > seed_id.z)
						{
							block_zp = ptr_rdata->inside_voxels[i].id;
							break;
						}
					}
				}
				else
				{
					block_zp = block_id;
				}

				// erase below
				for (int i = 0; i < P1_expandrange.size(); i++)
				{
					if (P1_expandrange[i].x == block_zp.x && P1_expandrange[i].y < block_zp.y && P1_expandrange[i].z == block_zp.z)
					{
						P1_expandrange.erase(P1_expandrange.begin() + i);
					}
					else
					{
						break;
					}
				}
			}
			P1_expandrange.push_back(block_zp); //temporary
		}

		//==========================================================

		P1_done = true;
	}

	// prepare to draw shortest path
	vector<RGBAcolor> shortest_path_color;
	for (size_t i = 0; i < shortest_path.size(); i++)
	{
		if (i == 0) //start
		{
			shortest_path_color.push_back({ 0, 255, 0, 255 });
		}
		else if (i == shortest_path.size() - 1) //end
		{
			shortest_path_color.push_back({ 255, 0, 0, 255 });
			break;
		}
		else
		{
			shortest_path_color.push_back({ 50, 50, 50, 255 });
		}
	}

	//	prepare to draw seed voxel,block voxel and p1 range

	vector<RGBAcolor> color_list;
	for (int i = 0; i < P1_expandrange.size(); i++)
	{
		if (P1_expandrange[i] == seed_id)
		{
			color_list.push_back({ 255, 0, 0, 255 });
		}
		else if (P1_expandrange[i] == block_yn)
		{
			color_list.push_back({ 0, 255, 0, 255 });
		}
		else if (P1_expandrange[i] == block_xn)
		{
			color_list.push_back({ 0, 255, 0, 255 });
		}
		else if (P1_expandrange[i] == block_xp)
		{
			color_list.push_back({ 0, 255, 0, 255 });
		}
		else if (P1_expandrange[i] == block_zn)
		{
			color_list.push_back({ 0, 255, 0, 255 });
		}
		else if (P1_expandrange[i] == block_zp)
		{
			color_list.push_back({ 0, 255, 0, 255 });
		}
		else
		{
			color_list.push_back({ 255, 255, 255, 255 });
		}
	}

	//setup P1
	P1.seedvoxel_id = seed_id;
	P1.voxel_list = P1_expandrange;
	P1.blockXm = block_xn;
	P1.blockXp = block_xp;
	P1.blockYm = block_yn;
	P1.blockYp = { -1, -1, -1 };
	P1.blockZm = block_zn;
	P1.blockZp = block_zp;

	return P1;
	//return CreateModelByID(ptr_VSpace, shortest_path, shortest_path_color);
	//return CreateModelByID(ptr_VSpace, P1_expandrange, color_list);
}

GLVoxelModel* /*bool*/ IsPuzzleSuccess(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata, PuzzlePiece* puzzle)
{
	std::vector<vector<vector<bool>>> visit;

	visit.resize(ptr_VSpace->resolution_x);
	for (int x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		visit[x].resize(ptr_VSpace->resolution_y);
		for (int y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			visit[x][y].resize(ptr_VSpace->resolution_z, false);
		}
	}

	std::vector<vector<vector<bool>>> isOnPuzzle;

	isOnPuzzle.resize(ptr_VSpace->resolution_x);
	for (int x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		isOnPuzzle[x].resize(ptr_VSpace->resolution_y);
		for (int y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			isOnPuzzle[x][y].resize(ptr_VSpace->resolution_z, false);
		}
	}

	for (int i = 0; i < puzzle->voxel_list.size(); i++)
	{
		if (ptr_VSpace->IsThisVoxelIDSafe(puzzle->voxel_list[i].x, puzzle->voxel_list[i].y, puzzle->voxel_list[i].z))
		{
			isOnPuzzle[puzzle->voxel_list[i].x][puzzle->voxel_list[i].y][puzzle->voxel_list[i].z] = true;
		}
	}

	std::vector<VoxelID> vid_list;		//保存p1以外的剩餘voxel座標 
	std::vector<RGBAcolor> color_list;	//如果剩下的voxel有孤立的情況會用不同顏色
	bool Isdone = false;
	srand(time(NULL));
	while (!Isdone)
	{
		RGBAcolor rand_color = { rand() % 256, rand() % 256, rand() % 256, 255 };
		std::queue<VoxelID> q;

		//找出一個未被拜訪過且不在P1、屬於模型的voxel
		//從inside voxel 找
		for (int i = 0; i < ptr_VSpace->inside_voxels.size(); i++)
		{
			int x = ptr_VSpace->inside_voxels[i].getID().x;
			int y = ptr_VSpace->inside_voxels[i].getID().y;
			int z = ptr_VSpace->inside_voxels[i].getID().z;
			if (!isOnPuzzle[x][y][z])
			{
				if (!visit[x][y][z])
				{
					// push to queue and record as visit
					visit[x][y][z] = true;
					q.push({ x, y, z });
					// record to list and color
					vid_list.push_back({ x, y, z });
					color_list.push_back(rand_color);
					// 
					Isdone = false;
					break;
				}
				else
				{
					Isdone = true;
				}
			}
		}
		//從shell voxel找
		if (Isdone)
		{
			for (int i = 0; i < ptr_VSpace->shell_voxels.size(); i++)
			{
				int x = ptr_VSpace->shell_voxels[i].getID().x;
				int y = ptr_VSpace->shell_voxels[i].getID().y;
				int z = ptr_VSpace->shell_voxels[i].getID().z;
				if (!isOnPuzzle[x][y][z])
				{
					if (!visit[x][y][z])
					{
						// push to queue and record as visit
						visit[x][y][z] = true;
						q.push({ x, y, z });
						// record to list and color
						vid_list.push_back({ x, y, z });
						color_list.push_back(rand_color);
						// 
						Isdone = false;
						break;
					}
					else
					{
						Isdone = true;
					}
				}
			}
		}

		//int abc = 0;
		while (!q.empty())
		{
			//std::wcout << abc++ << std::endl;
			VoxelID tmp_id = q.front();
			q.pop();

			// 往六鄰域尋找
			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_id.x + 1, tmp_id.y, tmp_id.z) && !isOnPuzzle[tmp_id.x + 1][tmp_id.y][tmp_id.z] && ptr_VSpace->VSmatrix[tmp_id.x + 1][tmp_id.y][tmp_id.z]->getType() != OUTSIDE && !visit[tmp_id.x + 1][tmp_id.y][tmp_id.z])
			{
				visit[tmp_id.x + 1][tmp_id.y][tmp_id.z] = true;
				q.push({ tmp_id.x + 1, tmp_id.y, tmp_id.z });
				vid_list.push_back({ tmp_id.x + 1, tmp_id.y, tmp_id.z });
				color_list.push_back(rand_color);
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_id.x - 1, tmp_id.y, tmp_id.z) && !isOnPuzzle[tmp_id.x - 1][tmp_id.y][tmp_id.z] && ptr_VSpace->VSmatrix[tmp_id.x - 1][tmp_id.y][tmp_id.z]->getType() != OUTSIDE && !visit[tmp_id.x - 1][tmp_id.y][tmp_id.z])
			{
				visit[tmp_id.x - 1][tmp_id.y][tmp_id.z] = true;
				q.push({ tmp_id.x - 1, tmp_id.y, tmp_id.z });
				vid_list.push_back({ tmp_id.x - 1, tmp_id.y, tmp_id.z });
				color_list.push_back(rand_color);
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_id.x, tmp_id.y + 1, tmp_id.z) && !isOnPuzzle[tmp_id.x][tmp_id.y + 1][tmp_id.z] && ptr_VSpace->VSmatrix[tmp_id.x][tmp_id.y + 1][tmp_id.z]->getType() != OUTSIDE && !visit[tmp_id.x][tmp_id.y + 1][tmp_id.z])
			{
				visit[tmp_id.x][tmp_id.y + 1][tmp_id.z] = true;
				q.push({ tmp_id.x, tmp_id.y + 1, tmp_id.z });
				vid_list.push_back({ tmp_id.x, tmp_id.y + 1, tmp_id.z });
				color_list.push_back(rand_color);
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_id.x, tmp_id.y - 1, tmp_id.z) && !isOnPuzzle[tmp_id.x][tmp_id.y - 1][tmp_id.z] && ptr_VSpace->VSmatrix[tmp_id.x][tmp_id.y - 1][tmp_id.z]->getType() != OUTSIDE && !visit[tmp_id.x][tmp_id.y - 1][tmp_id.z])
			{
				visit[tmp_id.x][tmp_id.y - 1][tmp_id.z] = true;
				q.push({ tmp_id.x, tmp_id.y - 1, tmp_id.z });
				vid_list.push_back({ tmp_id.x, tmp_id.y - 1, tmp_id.z });
				color_list.push_back(rand_color);
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_id.x, tmp_id.y, tmp_id.z + 1) && !isOnPuzzle[tmp_id.x][tmp_id.y][tmp_id.z + 1] && ptr_VSpace->VSmatrix[tmp_id.x][tmp_id.y][tmp_id.z + 1]->getType() != OUTSIDE && !visit[tmp_id.x][tmp_id.y][tmp_id.z + 1])
			{
				visit[tmp_id.x][tmp_id.y][tmp_id.z + 1] = true;
				q.push({ tmp_id.x, tmp_id.y, tmp_id.z + 1 });
				vid_list.push_back({ tmp_id.x, tmp_id.y, tmp_id.z + 1 });
				color_list.push_back(rand_color);
			}
			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_id.x, tmp_id.y, tmp_id.z - 1) && !isOnPuzzle[tmp_id.x][tmp_id.y][tmp_id.z - 1] && ptr_VSpace->VSmatrix[tmp_id.x][tmp_id.y][tmp_id.z - 1]->getType() != OUTSIDE && !visit[tmp_id.x][tmp_id.y][tmp_id.z - 1])
			{
				visit[tmp_id.x][tmp_id.y][tmp_id.z - 1] = true;
				q.push({ tmp_id.x, tmp_id.y, tmp_id.z - 1 });
				vid_list.push_back({ tmp_id.x, tmp_id.y, tmp_id.z - 1 });
				color_list.push_back(rand_color);
			}
		}
	}//end while

	{	// check if rest voxel is connected
		bool isPuzzleSuccess = true;
		bool isbreak = false;
		for (int x = 0; x < ptr_VSpace->resolution_x; x++)
		{
			for (int y = 0; y < ptr_VSpace->resolution_y; y++)
			{
				for (int z = 0; z < ptr_VSpace->resolution_z; z++)
				{
					if (ptr_VSpace->VSmatrix[x][y][z]->getType() != OUTSIDE && !isbreak)
					{
						if (!isOnPuzzle[x][y][z])
						{
							if (visit[x][y][z])
							{
								isPuzzleSuccess = true;
								continue;
							}
							else
							{
								isPuzzleSuccess = false;
								isbreak = true;
							}
						}
					}
				}
			}
		}
		std::cout << "[MESSAGE:]" << "Is P1 success?" << std::boolalpha << isPuzzleSuccess << std::endl;
	}

	//return isPuzzleSuccess;

	return CreateModelByID(ptr_VSpace, vid_list, color_list);
}

/*PuzzlePiece*/ GLVoxelModel* CreatePuzzleForP2(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata, PuzzlePiece* p1)
{
	//	重新定義剩下的voxel

	//	清空 remain data
	ptr_rdata->iterative += 1;
	ptr_rdata->shell_voxels.clear();
	ptr_rdata->inside_voxels.clear();
	ptr_rdata->colorgroup_list.clear();
	ptr_rdata->border_voxels.clear();
	ptr_rdata->last_neighbors.clear();

	for (int x = 0; x < ptr_rdata->resolution[0]; x++)
	{
		for (int y = 0; y < ptr_rdata->resolution[1]; y++)
		{
			for (int z = 0; z < ptr_rdata->resolution[2]; z++)
			{
				ptr_rdata->matrix[x][y][z] == nullptr;
			}
		}
	}

	//	重新加入P1以外的remain data
	std::vector<vector<vector<bool>>> Isbelongtop1;
	Isbelongtop1.resize(ptr_VSpace->resolution_x);
	for (int x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		Isbelongtop1[x].resize(ptr_VSpace->resolution_y);
		for (int y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			Isbelongtop1[x][y].resize(ptr_VSpace->resolution_z);
		}
	}

	for (int x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		for (int y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			for (int z = 0; z < ptr_VSpace->resolution_z; z++)
			{
				Isbelongtop1[x][y][z] = false;
			}
		}
	}

	//int total = 0;
	for (int i = 0; i < p1->voxel_list.size(); i++)
	{
		int x = p1->voxel_list[i].x;
		int y = p1->voxel_list[i].y;
		int z = p1->voxel_list[i].z;
		if (ptr_VSpace->IsThisVoxelIDSafe(x, y, z))
		{
			Isbelongtop1[x][y][z] = true;
			//total++;
		}
	}

	// 加入p1以外的voxel
	for (int i = 0; i < ptr_VSpace->shell_voxels.size(); i++)
	{
		int x = ptr_VSpace->shell_voxels[i].getID().x;
		int y = ptr_VSpace->shell_voxels[i].getID().y;
		int z = ptr_VSpace->shell_voxels[i].getID().z;
		if (Isbelongtop1[x][y][z] == false)
		{
			remainelement re = { { x, y, z }, -1, 0.0f, 0 };
			ptr_rdata->shell_voxels.push_back(re);
		}
	}
	for (int i = 0; i < ptr_VSpace->inside_voxels.size(); i++)
	{
		int x = ptr_VSpace->inside_voxels[i].getID().x;
		int y = ptr_VSpace->inside_voxels[i].getID().y;
		int z = ptr_VSpace->inside_voxels[i].getID().z;
		if (Isbelongtop1[x][y][z] == false)
		{
			remainelement re = { { x, y, z }, -1, 0.0f, 0 };
			ptr_rdata->inside_voxels.push_back(re);
		}
	}
	// 將剩餘的(p1以外)voxel指標指給matrix
	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		int x = ptr_rdata->shell_voxels[i].id.x;
		int y = ptr_rdata->shell_voxels[i].id.y;
		int z = ptr_rdata->shell_voxels[i].id.z;
		ptr_rdata->matrix[x][y][z] = &(ptr_rdata->shell_voxels[i]);
	}
	for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	{
		int x = ptr_rdata->inside_voxels[i].id.x;
		int y = ptr_rdata->inside_voxels[i].id.y;
		int z = ptr_rdata->inside_voxels[i].id.z;
		ptr_rdata->matrix[x][y][z] = &(ptr_rdata->inside_voxels[i]);
	}

	//找回border voxel(利用先前標記好的P1範圍)
	for (int x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		for (int y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			for (int z = 0; z < ptr_VSpace->resolution_z; z++)
			{
				if (ptr_rdata->matrix[x][y][z] != nullptr)
				{
					//	檢查6鄰域是否相鄰空值->border voxel
					if ((ptr_VSpace->IsThisVoxelIDSafe(x + 1, y, z) && ptr_rdata->matrix[x + 1][y][z] == nullptr)
						|| (ptr_VSpace->IsThisVoxelIDSafe(x - 1, y, z) && ptr_rdata->matrix[x - 1][y][z] == nullptr)
						|| (ptr_VSpace->IsThisVoxelIDSafe(x, y + 1, z) && ptr_rdata->matrix[x][y + 1][z] == nullptr)
						|| (ptr_VSpace->IsThisVoxelIDSafe(x, y - 1, z) && ptr_rdata->matrix[x][y - 1][z] == nullptr)
						|| (ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1) && ptr_rdata->matrix[x][y][z + 1] == nullptr)
						|| (ptr_VSpace->IsThisVoxelIDSafe(x, y, z - 1) && ptr_rdata->matrix[x][y][z - 1] == nullptr))
					{
						ptr_rdata->border_voxels.push_back({ x, y, z });
					}
				}
			}
		}
	}

	//-------------remain data 重建完成---------
	CalculateAccessibility(3, 0.1, ptr_VSpace, ptr_rdata);
	SeperateColorGroup(ptr_VSpace, ptr_rdata);
	CalculateDistanceTransform(ptr_VSpace, ptr_rdata);

	/*尋找seed voxel*/
	vector<VoxelID> candadite_set;
	{
		//暫時性的解法
		for (int x = 0; x < ptr_rdata->resolution[0]; x++)
		{
			for (int y = 0; y < ptr_rdata->resolution[1]; y++)
			{
				for (int z = 0; z < ptr_rdata->resolution[2]; z++)
				{
					//	找尋remain data中鄰近p1的
					if (ptr_rdata->matrix[x][y][z] != nullptr)
					{
						//	鄰近的方向要跟上面決定好的方向一致
						if ((ptr_VSpace->IsThisVoxelIDSafe(x, y, z + 1) && Isbelongtop1[x][y][z + 1]))
						{
							candadite_set.push_back({ x, y, z });
						}
					}
				}
			}
		}
	}

	/*選擇一個其餘5個方向都有可以block的seed voxel*/
	VoxelID seed_voxel;
	for (int i = 0; i < candadite_set.size(); i++)
	{
		int tmp_x = candadite_set[i].x;
		int tmp_y = candadite_set[i].y;
		int tmp_z = candadite_set[i].z;
		if (//x+1
			ptr_VSpace->IsThisVoxelIDSafe(tmp_x + 1, tmp_y, tmp_z)
			&& ptr_rdata->matrix[tmp_x + 1][tmp_y][tmp_z]
			// x-1
			&& ptr_VSpace->IsThisVoxelIDSafe(tmp_x - 1, tmp_y, tmp_z)
			&& ptr_rdata->matrix[tmp_x - 1][tmp_y][tmp_z]
			// y+1
			&& ptr_VSpace->IsThisVoxelIDSafe(tmp_x, tmp_y + 1, tmp_z)
			&& ptr_rdata->matrix[tmp_x][tmp_y + 1][tmp_z]
			// y-1
			&& ptr_VSpace->IsThisVoxelIDSafe(tmp_x, tmp_y - 1, tmp_z)
			&& ptr_rdata->matrix[tmp_x][tmp_y - 1][tmp_z]
			// z-1
			&& ptr_VSpace->IsThisVoxelIDSafe(tmp_x, tmp_y, tmp_z - 1)
			&& ptr_rdata->matrix[tmp_x][tmp_y][tmp_z - 1])
		{
			seed_voxel = { tmp_x, tmp_y, tmp_z };
		}
	}

	//	找到shortest path的目標
	//	從seed voxel沿著p2抽出的方向尋找目標
	VoxelID tmp_destination;
	VoxelID destination_voxel;
	for (int i = seed_voxel.z + 1; i < ptr_VSpace->resolution_z; i++)
	{
		if (ptr_VSpace->VSmatrix[seed_voxel.x][seed_voxel.y][i]->getType() == SHELL)// {x,i,z} 位在原本模型的表層體素
		{
			tmp_destination = { seed_voxel.x, seed_voxel.y, i };
			break;
		}
	}

	//	針對找到的目標進行檢查,如果是在P1或者顏色跟seedvoxel不同就要接著找
	bool is_ContinueSearch = false;
	if (ptr_VSpace->IsThisVoxelIDSafe(tmp_destination.x, tmp_destination.y, tmp_destination.z))
	{
		if (ptr_VSpace->VSmatrix[seed_voxel.x][seed_voxel.y][seed_voxel.z]->getType() == SHELL)
		{
			if (!Isbelongtop1[tmp_destination.x][tmp_destination.y][tmp_destination.z]
				&& (ptr_VSpace->VSmatrix[tmp_destination.x][tmp_destination.y][tmp_destination.z]->getColor() == ptr_VSpace->VSmatrix[seed_voxel.x][seed_voxel.y][seed_voxel.z]->getColor()))
			{
				is_ContinueSearch = true;
			}
			else
			{
				is_ContinueSearch = false;
				destination_voxel = tmp_destination;
			}
		}
		else if (ptr_VSpace->VSmatrix[seed_voxel.x][seed_voxel.y][seed_voxel.z]->getType() == INSIDE)
		{
			if (!Isbelongtop1[tmp_destination.x][tmp_destination.y][tmp_destination.z])
			{
				is_ContinueSearch = true;
			}
			else
			{
				is_ContinueSearch = false;
				destination_voxel = tmp_destination;
			}
		}
	}

	//	如果前面的結果是true->用bfs搜尋直到符合條件
	if (is_ContinueSearch)
	{
		std::vector<vector<vector<bool>>> visit;
		
	}

	// 確定找到目標後要給出範圍跑最短路徑
	VoxelColor p2_color;
	if (ptr_VSpace->VSmatrix[seed_voxel.x][seed_voxel.y][seed_voxel.z]->getType() == SHELL)
	{
		p2_color = ptr_VSpace->VSmatrix[seed_voxel.x][seed_voxel.y][seed_voxel.z]->getColor();
	}
	else if (ptr_VSpace->VSmatrix[destination_voxel.x][destination_voxel.y][destination_voxel.z]->getType() == SHELL)
	{
		p2_color = ptr_VSpace->VSmatrix[destination_voxel.x][destination_voxel.y][destination_voxel.z]->getColor();
	}

	std::vector<VoxelID> p2_expand_range;
	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		VoxelID tmp_id = ptr_rdata->shell_voxels[i].id;
		if (ptr_VSpace->VSmatrix[tmp_id.x][tmp_id.y][tmp_id.z]->getColor() == p2_color)
		{
			//沿著p2方向納入所有的voxel 
			for (int z = tmp_id.z; z > 0; z--)
			{
				if (ptr_VSpace->VSmatrix[tmp_id.x][tmp_id.y][z]->getColor() == p2_color || ptr_VSpace->VSmatrix[tmp_id.x][tmp_id.y][z]->getType() == INSIDE)
				{
					p2_expand_range.push_back({ tmp_id.x, tmp_id.y, z });
				}
				else
				{
					break;
				}
			}
		}
	}

	std::vector<VoxelID> shortest_path = FindShortestPath(seed_voxel, destination_voxel, p2_expand_range, ptr_VSpace, ptr_rdata);

	//	將所有shortest path 路徑上的元素沿著p2方向擴張到表面
	std::vector<VoxelID> P2_vid_list;
	for (int i = 0; i < shortest_path.size(); i++)
	{
		VoxelID tmp = shortest_path[i];
		for (int z = tmp.z; z < ptr_VSpace->resolution_z; z++)
		{
			if (ptr_VSpace->IsThisVoxelIDSafe(tmp.x, tmp.y, z) && ptr_rdata->matrix[tmp.x][tmp.y][z] != nullptr)
			{
				if (ptr_VSpace->VSmatrix[tmp.x][tmp.y][z]->getColor() == p2_color || ptr_VSpace->VSmatrix[tmp.x][tmp.y][z]->getType() == INSIDE)
				{
					P2_vid_list.push_back({ tmp.x, tmp.y, z });
				}
			}
		}
	}

	////	尋找其他5個blocking voxel
	////
	//VoxelID block_xn = { -1, -1, -1 };
	//VoxelID	block_xp = { -1, -1, -1 };
	//VoxelID block_yn = { -1, -1, -1 };
	//VoxelID block_yp = { -1, -1, -1 };
	//VoxelID block_zn = { -1, -1, -1 };

	//std::vector<vector<vector<bool>>> Isbelong_shortestpath;
	//{
	//	//初始化
	//	Isbelong_shortestpath.resize(ptr_rdata->resolution[0]);
	//	for (int x = 0; x < ptr_rdata->resolution[0]; x++)
	//	{
	//		Isbelong_shortestpath[x].resize(ptr_rdata->resolution[1]);
	//		for (int y = 0; y < ptr_rdata->resolution[1]; y++)
	//		{
	//			Isbelong_shortestpath[x][y].resize(ptr_rdata->resolution[2], false);
	//		}
	//	}
	//	//填入value
	//	for (int i = 0; i < shortest_path.size(); i++)
	//	{
	//		int x = shortest_path[i].x;
	//		int y = shortest_path[i].y;
	//		int z = shortest_path[i].z;
	//		Isbelong_shortestpath[x][y][z] = true;
	//	}
	//}

	//std::vector<vector<vector<bool>>> Isbelong_P2expandrange;
	//{
	//	//初始化
	//	Isbelong_P2expandrange.resize(ptr_rdata->resolution[0]);
	//	for (int x = 0; x < ptr_rdata->resolution[0]; x++)
	//	{
	//		Isbelong_P2expandrange[x].resize(ptr_rdata->resolution[1]);
	//		for (int y = 0; y < ptr_rdata->resolution[1]; y++)
	//		{
	//			Isbelong_P2expandrange[x][y].resize(ptr_rdata->resolution[2], false);
	//		}
	//	}
	//	//填值
	//	for (int i = 0; i < p2_expand_range.size(); i++)
	//	{
	//		int x = p2_expand_range[i].x;
	//		int y = p2_expand_range[i].y;
	//		int z = p2_expand_range[i].z;
	//		Isbelong_P2expandrange[x][y][z] = true;
	//	}
	//}

	////find xn,xp,yn,yp block voxel
	//{
	//	//直接走訪所有最短路徑上的node(已經可以視為p2的node)
	//	for (int i = 0; i < shortest_path.size(); i++)
	//	{
	//		VoxelID tmp = shortest_path[i];
	//		//每一個都來尋找這4個block
	//		if (!ptr_VSpace->IsThisVoxelIDSafe(block_xn.x, block_xn.y, block_xn.z) /*檢查是否合法*/
	//			|| ptr_rdata->matrix[block_xn.x][block_xn.y][block_xn.z] != nullptr	/*是否屬於rest data*/
	//			|| !Isbelong_P2expandrange[block_xn.x][block_xn.y][block_xn.z])	/*不能在p2擴張的範圍*/
	//		{
	//			//block_xn沒被找過,開始搜尋
	//			for (int j = tmp.x - 1; j >= 0; j--)
	//			{
	//				if (ptr_rdata->matrix[j][tmp.y][tmp.z] != nullptr
	//					&& !Isbelong_P2expandrange[j][tmp.y][tmp.z])
	//				{
	//					block_xn = { j, tmp.y, tmp.z };
	//					break;
	//				}
	//			}
	//		}
	//		if (!ptr_VSpace->IsThisVoxelIDSafe(block_xp.x, block_xp.y, block_xp.z)
	//			|| ptr_rdata->matrix[block_xp.x][block_xp.y][block_xp.z]!=nullptr
	//			|| !Isbelong_P2expandrange[block_xp.x][block_xp.y][block_xp.z])
	//		{
	//			//開始找block_xp
	//			for (int j = tmp.x + 1; j < ptr_rdata->resolution[0]; j++)
	//			{
	//				if (ptr_rdata->matrix[j][tmp.y][tmp.z] != nullptr
	//					&& !Isbelong_P2expandrange[j][tmp.y][tmp.z])
	//				{
	//					block_xp = { j, tmp.y, tmp.z };
	//					break;
	//				}
	//			}
	//		}
	//		if (!ptr_VSpace->IsThisVoxelIDSafe(block_yn.x, block_yn.y, block_yn.z)
	//			|| ptr_rdata->matrix[block_yn.x][block_yn.y][block_yn.z] != nullptr
	//			|| !Isbelong_P2expandrange[block_yn.x][block_yn.y][block_yn.z])
	//		{
	//			//找 block_yn
	//			for (int j = tmp.y - 1; j >= 0; j--)
	//			{
	//				if (ptr_rdata->matrix[tmp.x][j][tmp.z] != nullptr
	//					&& !Isbelong_P2expandrange[tmp.x][j][tmp.z])
	//				{
	//					block_yn = { tmp.x,j,tmp.z };
	//					break;
	//				}
	//			}
	//		}
	//		if (!ptr_VSpace->IsThisVoxelIDSafe(block_yp.x, block_yp.y, block_yp.z)
	//			|| ptr_rdata->matrix[block_yp.x][block_yp.y][block_yp.z] != nullptr
	//			|| !Isbelong_P2expandrange[block_yp.x][block_yp.y][block_yp.z])
	//		{
	//			//找 block_yp
	//			for (int j = tmp.y + 1; j < ptr_rdata->resolution[1]; j++)
	//			{
	//				if (ptr_rdata->matrix[tmp.x][j][tmp.z] != nullptr
	//					&& !Isbelong_P2expandrange[tmp.x][j][tmp.z])
	//				{
	//					block_yp = { tmp.x,j,tmp.z };
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}
	//
	//// 如果前面的不成功,找尋seed voxel之下的voxel並聯通過去
	//{
	//	//先確認有哪些方向不成功
	//	bool is_block_xn_success = false,
	//		is_block_xp_success = false,
	//		is_block_yn_success = false,
	//		is_block_yp_success = false,
	//		is_block_zn_success = false;
	//	if (ptr_VSpace->IsThisVoxelIDSafe(block_xn.x, block_xn.y, block_xn.z) /*檢查是否合法*/
	//		&& ptr_rdata->matrix[block_xn.x][block_xn.y][block_xn.z] != nullptr	/*是否屬於rest data*/
	//		&& !Isbelong_P2expandrange[block_xn.x][block_xn.y][block_xn.z])	/*不能在p2擴張的範圍*/
	//	{
	//		is_block_xn_success = true;
	//	}
	//	if (ptr_VSpace->IsThisVoxelIDSafe(block_xp.x, block_xp.y, block_xp.z)
	//		&& ptr_rdata->matrix[block_xp.x][block_xp.y][block_xp.z] != nullptr
	//		&& !Isbelong_P2expandrange[block_xp.x][block_xp.y][block_xp.z])
	//	{
	//		is_block_xp_success = true;
	//	}
	//	if (ptr_VSpace->IsThisVoxelIDSafe(block_yn.x, block_yn.y, block_yn.z)
	//		&& ptr_rdata->matrix[block_yn.x][block_yn.y][block_yn.z] != nullptr
	//		&& !Isbelong_P2expandrange[block_yn.x][block_yn.y][block_yn.z])
	//	{
	//		is_block_yn_success = true;
	//	}
	//	if (ptr_VSpace->IsThisVoxelIDSafe(block_yp.x, block_yp.y, block_yp.z)
	//		&& ptr_rdata->matrix[block_yp.x][block_yp.y][block_yp.z] != nullptr
	//		&& !Isbelong_P2expandrange[block_yp.x][block_yp.y][block_yp.z])
	//	{
	//		is_block_yp_success = true;
	//	}
	//	if (ptr_VSpace->IsThisVoxelIDSafe(block_zn.x, block_zn.y, block_zn.z)
	//		&& ptr_rdata->matrix[block_zn.x][block_zn.y][block_zn.z] != nullptr
	//		&& !Isbelong_P2expandrange[block_zn.x][block_zn.y][block_zn.z])
	//	{
	//		is_block_zn_success = true;
	//	}
	//	//找到一個在shortest path之下的voxel並嘗試連通過去
	//	bool is_tryNextVexel = true;
	//	std::queue<VoxelID> q;
	//	VoxelID tmp_block = { seed_voxel.x, seed_voxel.y, seed_voxel.z-1 };
	//	// 如果該voxel可以被當作block voxel而且剛好可以有路連過去
	//	if (ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x, tmp_block.y, tmp_block.z)
	//		&& ptr_rdata->matrix[tmp_block.x][tmp_block.y][tmp_block.z] != nullptr
	//		&& !Isbelong_shortestpath[tmp_block.x][tmp_block.y][tmp_block.z])
	//	{
	//		if (!is_block_xn_success)
	//		{
	//			//檢查該方向是否有voxel
	//			bool b = ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x - 1, tmp_block.y, tmp_block.z)
	//				&& ptr_rdata->matrix[tmp_block.x - 1][tmp_block.y][tmp_block.z] != nullptr
	//				&& !Isbelong_shortestpath[tmp_block.x - 1][tmp_block.y][tmp_block.z];
	//			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x-1, tmp_block.y, tmp_block.z)
	//				&& ptr_rdata->matrix[tmp_block.x-1][tmp_block.y][tmp_block.z] != nullptr
	//				&& !Isbelong_shortestpath[tmp_block.x-1][tmp_block.y][tmp_block.z])
	//			{
	//				VoxelID tmp_block_xn = { tmp_block.x - 1, tmp_block.y, tmp_block.z };
	//			}
	//			//以最短路徑連過去

	//			//如果最短路徑成功 設該方向的block為成功
	//		}
	//		if (!is_block_xp_success)
	//		{
	//			//檢查該方向是否有voxel
	//			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x + 1, tmp_block.y, tmp_block.z)
	//				&& ptr_rdata->matrix[tmp_block.x + 1][tmp_block.y][tmp_block.z] != nullptr
	//				&& !Isbelong_shortestpath[tmp_block.x + 1][tmp_block.y][tmp_block.z])
	//			{
	//				VoxelID tmp_block_xp = { tmp_block.x + 1, tmp_block.y, tmp_block.z };
	//			}
	//		}
	//		if (!is_block_yn_success)
	//		{
	//			//檢查該方向是否有voxel
	//			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x, tmp_block.y-1, tmp_block.z)
	//				&& ptr_rdata->matrix[tmp_block.x][tmp_block.y-1][tmp_block.z] != nullptr
	//				&& !Isbelong_shortestpath[tmp_block.x][tmp_block.y-1][tmp_block.z])
	//			{
	//				VoxelID tmp_block_yn = { tmp_block.x, tmp_block.y-1, tmp_block.z };
	//			}
	//		}
	//		if (!is_block_yp_success)
	//		{
	//			//檢查該方向是否有voxel
	//			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x, tmp_block.y+1, tmp_block.z)
	//				&& ptr_rdata->matrix[tmp_block.x ][tmp_block.y+1][tmp_block.z] != nullptr
	//				&& !Isbelong_shortestpath[tmp_block.x ][tmp_block.y+1][tmp_block.z])
	//			{
	//				VoxelID tmp_block_yp = { tmp_block.x, tmp_block.y+1, tmp_block.z };
	//			}
	//		}
	//		if (!is_block_zn_success)
	//		{
	//			bool b = ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x, tmp_block.y, tmp_block.z - 1)
	//				&& ptr_rdata->matrix[tmp_block.x][tmp_block.y][tmp_block.z - 1] != nullptr
	//				&& !Isbelong_shortestpath[tmp_block.x][tmp_block.y][tmp_block.z - 1];
	//			//檢查該方向是否有voxel
	//			if (ptr_VSpace->IsThisVoxelIDSafe(tmp_block.x, tmp_block.y, tmp_block.z-1)
	//				&& ptr_rdata->matrix[tmp_block.x][tmp_block.y][tmp_block.z-1] != nullptr
	//				&& !Isbelong_shortestpath[tmp_block.x][tmp_block.y][tmp_block.z-1])
	//			{
	//				VoxelID tmp_block_zn = { tmp_block.x, tmp_block.y, tmp_block.z-1 };
	//			}
	//		}

	//	}
	//}


	//	開始準備輸出模型
	std::vector<VoxelID> vid_list;
	std::vector<RGBAcolor> vid_color;
	
	vid_list.push_back(seed_voxel);
	////	show shortest path
	//vid_list = shortest_path;
	//for (int i = 0; i < vid_list.size(); i++)
	//{
	//	if (vid_list[i] == seed_voxel)
	//	{
	//		vid_color.push_back({ 255, 0, 0, 255 });
	//	}
	//	if (vid_list[i] == destination_voxel)
	//	{
	//		vid_color.push_back({ 0, 0, 255, 255 });
	//	}
	//}

	//// add block voxel
	//vid_list.push_back(block_xn);
	//vid_color.push_back({ 0, 255, 0, 255 });
	//vid_list.push_back(block_xp);
	//vid_color.push_back({ 0, 255, 0, 255 });
	//vid_list.push_back(block_yn);
	//vid_color.push_back({ 0, 255, 0, 255 });
	//vid_list.push_back(block_yp);
	//vid_color.push_back({ 0, 255, 0, 255 });
	//vid_list.push_back(block_zn);
	//vid_color.push_back({ 0, 255, 0, 255 });

	return CreateModelByID(ptr_VSpace, vid_list);
	//return CreateModelByID(ptr_VSpace, vid_list, vid_color);
}


int getColorGroup(const int group_id, remain_data* ptr_rdata)
{
	bool isExist = false;
	int group_index;
	for (int i = 0; i < ptr_rdata->colorgroup_list.size(); i++)
	{
		if (ptr_rdata->colorgroup_list[i].id == group_id)
		{
			group_index = i;
			isExist = true;
			break;
		}
	}
	return isExist ? group_index : -1;
}

void CalculateDistanceTransform(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	//	初始化,令所有voxel的 distance = -1,表示未被拜訪過
	for (int i = 0; i < ptr_rdata->shell_voxels.size(); i++)
	{
		ptr_rdata->shell_voxels[i].distance = -1;
	}
	for (int i = 0; i < ptr_rdata->inside_voxels.size(); i++)
	{
		ptr_rdata->inside_voxels[i].distance = -1;
	}

	//	將border_voxel對應的distance重新設為 0
	for (int i = 0; i < ptr_rdata->border_voxels.size(); i++)
	{
		int x = ptr_rdata->border_voxels[i].x;
		int y = ptr_rdata->border_voxels[i].y;
		int z = ptr_rdata->border_voxels[i].z;
		if (ptr_rdata->matrix[x][y][z] != nullptr)
		{
			ptr_rdata->matrix[x][y][z]->distance = 0;
		}
		else
		{
			std::cout << "there exist border voxel doesn't belong to remain data:"
				<< x << ", " << y << ", " << z << std::endl;
			return;
			break;
		}
	}

	/*	bfs way to count distance transform	*/

	//	準備一個queue存bfs
	std::queue<VoxelID> b_queue;

	//	先push所有的border_voxel做起點
	for (int i = 0; i < ptr_rdata->border_voxels.size(); i++)
	{
		b_queue.push(ptr_rdata->border_voxels[i]);
	}

	//	重複直到remain_data的voxel都算過一遍(distance >=0)
	VoxelID tmp_vid;
	while (!b_queue.empty())
	{
		//	取出最上方一個點
		tmp_vid = b_queue.front();
		b_queue.pop();

		//	加入未拜訪(計算)的六鄰域,並設定distance+=1
		//	x+1
		if (ptr_VSpace->IsThisVoxelIDSafe(tmp_vid.x + 1, tmp_vid.y, tmp_vid.z) && ptr_rdata->matrix[tmp_vid.x + 1][tmp_vid.y][tmp_vid.z] != nullptr && ptr_rdata->matrix[tmp_vid.x + 1][tmp_vid.y][tmp_vid.z]->distance == -1)
		{
			ptr_rdata->matrix[tmp_vid.x + 1][tmp_vid.y][tmp_vid.z]->distance = ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z]->distance + 1;
			b_queue.push({ tmp_vid.x + 1, tmp_vid.y, tmp_vid.z });
		}
		//	x-1
		if (ptr_VSpace->IsThisVoxelIDSafe(tmp_vid.x - 1, tmp_vid.y, tmp_vid.z) && ptr_rdata->matrix[tmp_vid.x - 1][tmp_vid.y][tmp_vid.z] != nullptr && ptr_rdata->matrix[tmp_vid.x - 1][tmp_vid.y][tmp_vid.z]->distance == -1)
		{
			ptr_rdata->matrix[tmp_vid.x - 1][tmp_vid.y][tmp_vid.z]->distance = ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z]->distance + 1;
			b_queue.push({ tmp_vid.x - 1, tmp_vid.y, tmp_vid.z });
		}
		//	y+1
		if (ptr_VSpace->IsThisVoxelIDSafe(tmp_vid.x, tmp_vid.y + 1, tmp_vid.z) && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y + 1][tmp_vid.z] != nullptr && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y + 1][tmp_vid.z]->distance == -1)
		{
			ptr_rdata->matrix[tmp_vid.x][tmp_vid.y + 1][tmp_vid.z]->distance = ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z]->distance + 1;
			b_queue.push({ tmp_vid.x, tmp_vid.y + 1, tmp_vid.z });
		}
		//	y-1
		if (ptr_VSpace->IsThisVoxelIDSafe(tmp_vid.x, tmp_vid.y - 1, tmp_vid.z) && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y - 1][tmp_vid.z] != nullptr && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y - 1][tmp_vid.z]->distance == -1)
		{
			ptr_rdata->matrix[tmp_vid.x][tmp_vid.y - 1][tmp_vid.z]->distance = ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z]->distance + 1;
			b_queue.push({ tmp_vid.x, tmp_vid.y - 1, tmp_vid.z });
		}
		//	z+1
		if (ptr_VSpace->IsThisVoxelIDSafe(tmp_vid.x, tmp_vid.y, tmp_vid.z + 1) && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z + 1] != nullptr && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z + 1]->distance == -1)
		{
			ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z + 1]->distance = ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z]->distance + 1;
			b_queue.push({ tmp_vid.x, tmp_vid.y, tmp_vid.z + 1 });
		}
		//	z-1
		if (ptr_VSpace->IsThisVoxelIDSafe(tmp_vid.x, tmp_vid.y, tmp_vid.z - 1) && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z - 1] != nullptr && ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z - 1]->distance == -1)
		{
			ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z - 1]->distance = ptr_rdata->matrix[tmp_vid.x][tmp_vid.y][tmp_vid.z]->distance + 1;
			b_queue.push({ tmp_vid.x, tmp_vid.y, tmp_vid.z - 1 });
		}
	}
}

vec3f CalculateDistanceGradient(VoxelID vox, CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	//x+1
	int d_xp;
	if (ptr_VSpace->IsThisVoxelIDSafe(vox.x + 1, vox.y, vox.z) && ptr_rdata->matrix[vox.x + 1][vox.y][vox.z] != nullptr)
	{
		d_xp = ptr_rdata->matrix[vox.x + 1][vox.y][vox.z]->distance;
	}
	else
	{
		d_xp = 0;
	}
	//x-1
	int d_xn;
	if (ptr_VSpace->IsThisVoxelIDSafe(vox.x - 1, vox.y, vox.z) && ptr_rdata->matrix[vox.x - 1][vox.y][vox.z] != nullptr)
	{
		d_xn = ptr_rdata->matrix[vox.x - 1][vox.y][vox.z]->distance;
	}
	else
	{
		d_xn = 0;
	}
	//y+1
	int d_yp;
	if (ptr_VSpace->IsThisVoxelIDSafe(vox.x, vox.y + 1, vox.z) && ptr_rdata->matrix[vox.x][vox.y + 1][vox.z] != nullptr)
	{
		d_yp = ptr_rdata->matrix[vox.x][vox.y + 1][vox.z]->distance;
	}
	else
	{
		d_yp = 0;
	}
	//y-1
	int d_yn;
	if (ptr_VSpace->IsThisVoxelIDSafe(vox.x, vox.y - 1, vox.z) && ptr_rdata->matrix[vox.x][vox.y - 1][vox.z] != nullptr)
	{
		d_yn = ptr_rdata->matrix[vox.x][vox.y - 1][vox.z]->distance;
	}
	else
	{
		d_yn = 0;
	}
	//z+1
	int d_zp;
	if (ptr_VSpace->IsThisVoxelIDSafe(vox.x, vox.y, vox.z + 1) && ptr_rdata->matrix[vox.x][vox.y][vox.z + 1] != nullptr)
	{
		d_zp = ptr_rdata->matrix[vox.x][vox.y][vox.z + 1]->distance;
	}
	else
	{
		d_zp = 0;
	}
	//z-1
	int d_zn;
	if (ptr_VSpace->IsThisVoxelIDSafe(vox.x, vox.y, vox.z - 1) && ptr_rdata->matrix[vox.x][vox.y][vox.z - 1] != nullptr)
	{
		d_zn = ptr_rdata->matrix[vox.x][vox.y][vox.z - 1]->distance;
	}
	else
	{
		d_zn = 0;
	}

	return{ (d_xp - d_xn) / 2, (d_yp - d_yn) / 2, (d_zp - d_zn) / 2 };
}

GLfloat Vec3fDotProduct(vec3f a, vec3f b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vector<VoxelID> FindShortestPath(VoxelID start, VoxelID end, CVoxelSpace* pVSpace, vector<VoxelID> vid_list, vector<vector<vector<float>>>& vid_accessibility)
{
	vector<vector<vector<float>>> distance; //紀錄所有點的最短路徑距離
	vector<vector<vector<bool>>> visit; //是否有拜訪過
	vector<vector<vector<bool>>> IsOnSearchRange; //是否在搜尋範圍內
	std::queue<VoxelID> Q;

	distance.resize(pVSpace->resolution_x);
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		distance[x].resize(pVSpace->resolution_y);
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			distance[x][y].resize(pVSpace->resolution_z);
		}
	}

	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < pVSpace->resolution_z; z++)
			{
				distance[x][y][z] = 1e9;
			}
		}
	}


	visit.resize(pVSpace->resolution_x);
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		visit[x].resize(pVSpace->resolution_y);
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			visit[x][y].resize(pVSpace->resolution_z);
		}
	}

	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < pVSpace->resolution_z; z++)
			{
				visit[x][y][z] = false;
			}
		}
	}

	IsOnSearchRange.resize(pVSpace->resolution_x);
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		IsOnSearchRange[x].resize(pVSpace->resolution_y);
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			IsOnSearchRange[x][y].resize(pVSpace->resolution_z);
		}
	}

	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < pVSpace->resolution_z; z++)
			{
				IsOnSearchRange[x][y][z] = false;
			}
		}
	}

	for (size_t i = 0; i < vid_list.size(); i++)
	{
		IsOnSearchRange[vid_list[i].x][vid_list[i].y][vid_list[i].z] = true;
	}

	//	1.設定初始值且push起點進queue
	distance[start.x][start.y][start.z] = 0.0f;
	visit[start.x][start.y][start.z] = true;
	Q.push(start);

	while (!Q.empty())
	{
		// 取出最上面的點
		VoxelID a = Q.front();
		Q.pop();

		// 從六鄰域拜訪過的點中找出此點的最短路徑長度，並設定給distance[a.x][a.y][a.z]
		if (pVSpace->IsThisVoxelIDSafe(a.x + 1, a.y, a.z) && visit[a.x + 1][a.y][a.z] && IsOnSearchRange[a.x + 1][a.y][a.z] && distance[a.x + 1][a.y][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x + 1][a.y][a.z] + 1;
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x - 1, a.y, a.z) && visit[a.x - 1][a.y][a.z] && IsOnSearchRange[a.x - 1][a.y][a.z] && distance[a.x - 1][a.y][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x - 1][a.y][a.z] + 1;
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y + 1, a.z) && visit[a.x][a.y + 1][a.z] && IsOnSearchRange[a.x][a.y + 1][a.z] && distance[a.x][a.y + 1][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y + 1][a.z] + 1;
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y - 1, a.z) && visit[a.x][a.y - 1][a.z] && IsOnSearchRange[a.x][a.y - 1][a.z] && distance[a.x][a.y - 1][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y - 1][a.z] + 1;
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y, a.z + 1) && visit[a.x][a.y][a.z + 1] && IsOnSearchRange[a.x][a.y][a.z + 1] && distance[a.x][a.y][a.z + 1] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y][a.z + 1] + 1;
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y, a.z - 1) && visit[a.x][a.y][a.z - 1] && IsOnSearchRange[a.x][a.y][a.z - 1] && distance[a.x][a.y][a.z - 1] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y][a.z - 1] + 1;
		}

		////此點已計算出最短路徑->設定為拜訪過
		//visit[a.x][a.y][a.z] = true;

		//將六鄰域中未拜訪過的點加入queue，
		if (pVSpace->IsThisVoxelIDSafe(a.x + 1, a.y, a.z) && !visit[a.x + 1][a.y][a.z] && IsOnSearchRange[a.x + 1][a.y][a.z])
		{
			/*std::cout << "push:" << "{" << a.x + 1 << "," << a.y << "," << a.z << "}" << std::endl;*/
			visit[a.x + 1][a.y][a.z] = true;
			Q.push({ a.x + 1, a.y, a.z });
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x - 1, a.y, a.z) && !visit[a.x - 1][a.y][a.z] && IsOnSearchRange[a.x - 1][a.y][a.z])
		{
			/*std::cout << "push:" << "{" << a.x - 1 << "," << a.y << "," << a.z << "}" << std::endl;*/
			visit[a.x - 1][a.y][a.z] = true;
			Q.push({ a.x - 1, a.y, a.z });
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y + 1, a.z) && !visit[a.x][a.y + 1][a.z] && IsOnSearchRange[a.x][a.y + 1][a.z])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y + 1 << "," << a.z << "}" << std::endl;*/
			visit[a.x][a.y + 1][a.z] = true;
			Q.push({ a.x, a.y + 1, a.z });
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y - 1, a.z) && !visit[a.x][a.y - 1][a.z] && IsOnSearchRange[a.x][a.y - 1][a.z])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y - 1 << "," << a.z << "}" << std::endl;*/
			visit[a.x][a.y - 1][a.z] = true;
			Q.push({ a.x, a.y - 1, a.z });
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y, a.z + 1) && !visit[a.x][a.y][a.z + 1] && IsOnSearchRange[a.x][a.y][a.z + 1])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y << "," << a.z + 1 << "}" << std::endl;*/
			visit[a.x][a.y][a.z + 1] = true;
			Q.push({ a.x, a.y, a.z + 1 });
		}
		if (pVSpace->IsThisVoxelIDSafe(a.x, a.y, a.z - 1) && !visit[a.x][a.y][a.z - 1] && IsOnSearchRange[a.x][a.y][a.z - 1])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y << "," << a.z - 1 << "}" << std::endl;*/
			visit[a.x][a.y][a.z - 1] = true;
			Q.push({ a.x, a.y, a.z - 1 });
		}
	}

	//	find path from end to start
	vector<VoxelID> shortest_path;

	VoxelID child = end;
	bool stop = false;
	shortest_path.push_back(end);
	while (!stop)
	{
		float min_distance = 1e9;
		VoxelID parent;
		if (pVSpace->IsThisVoxelIDSafe(child.x + 1, child.y, child.z) && IsOnSearchRange[child.x + 1][child.y][child.z] && distance[child.x + 1][child.y][child.z] < min_distance)
		{
			parent = { child.x + 1, child.y, child.z };
			min_distance = distance[child.x + 1][child.y][child.z];
		}
		if (pVSpace->IsThisVoxelIDSafe(child.x - 1, child.y, child.z) && IsOnSearchRange[child.x - 1][child.y][child.z] && distance[child.x - 1][child.y][child.z] < min_distance)
		{
			parent = { child.x - 1, child.y, child.z };
			min_distance = distance[child.x - 1][child.y][child.z];
		}
		if (pVSpace->IsThisVoxelIDSafe(child.x, child.y + 1, child.z) && IsOnSearchRange[child.x][child.y + 1][child.z] && distance[child.x][child.y + 1][child.z] < min_distance)
		{
			parent = { child.x, child.y + 1, child.z };
			min_distance = distance[child.x][child.y + 1][child.z];
		}
		if (pVSpace->IsThisVoxelIDSafe(child.x, child.y - 1, child.z) && IsOnSearchRange[child.x][child.y - 1][child.z] && distance[child.x][child.y - 1][child.z] < min_distance)
		{
			parent = { child.x, child.y - 1, child.z };
			min_distance = distance[child.x][child.y - 1][child.z];
		}
		if (pVSpace->IsThisVoxelIDSafe(child.x, child.y, child.z + 1) && IsOnSearchRange[child.x][child.y][child.z + 1] && distance[child.x][child.y][child.z + 1] < min_distance)
		{
			parent = { child.x, child.y, child.z + 1 };
			min_distance = distance[child.x][child.y][child.z + 1];
		}
		if (pVSpace->IsThisVoxelIDSafe(child.x, child.y, child.z - 1) && IsOnSearchRange[child.x][child.y][child.z - 1] && distance[child.x][child.y][child.z - 1] < min_distance)
		{
			parent = { child.x, child.y, child.z - 1 };
			min_distance = distance[child.x][child.y][child.z - 1];
		}

		//	add parent to shortest path
		std::cout << "parent: " << parent.x << "," << parent.y << "," << parent.z << std::endl;
		shortest_path.push_back(parent);

		//	if parent == start then stop
		if (parent.x == start.x && parent.y == start.y && parent.z == start.z)
		{
			stop = true;
		}
		//	else keep searching
		else
		{
			child = parent;
		}
	}

	return shortest_path;
}

vector<VoxelID> FindShortestPath(VoxelID start, VoxelID end, vector<VoxelID> vid_list, CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata)
{
	vector<vector<vector<float>>> distance; //紀錄所有點的最短路徑距離
	vector<vector<vector<bool>>> visit; //是否有拜訪過
	vector<vector<vector<bool>>> IsOnSearchRange; //是否在搜尋範圍內
	std::queue<VoxelID> Q;

	distance.resize(ptr_VSpace->resolution_x);
	for (size_t x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		distance[x].resize(ptr_VSpace->resolution_y);
		for (size_t y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			distance[x][y].resize(ptr_VSpace->resolution_z);
		}
	}

	for (size_t x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < ptr_VSpace->resolution_z; z++)
			{
				distance[x][y][z] = 1e9;
			}
		}
	}


	visit.resize(ptr_VSpace->resolution_x);
	for (size_t x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		visit[x].resize(ptr_VSpace->resolution_y);
		for (size_t y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			visit[x][y].resize(ptr_VSpace->resolution_z);
		}
	}

	for (size_t x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < ptr_VSpace->resolution_z; z++)
			{
				visit[x][y][z] = false;
			}
		}
	}

	IsOnSearchRange.resize(ptr_VSpace->resolution_x);
	for (size_t x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		IsOnSearchRange[x].resize(ptr_VSpace->resolution_y);
		for (size_t y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			IsOnSearchRange[x][y].resize(ptr_VSpace->resolution_z);
		}
	}

	for (size_t x = 0; x < ptr_VSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < ptr_VSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < ptr_VSpace->resolution_z; z++)
			{
				IsOnSearchRange[x][y][z] = false;
			}
		}
	}

	for (size_t i = 0; i < vid_list.size(); i++)
	{
		IsOnSearchRange[vid_list[i].x][vid_list[i].y][vid_list[i].z] = true;
	}

	//	1.設定初始值且push起點進queue
	distance[start.x][start.y][start.z] = 0.0f;
	visit[start.x][start.y][start.z] = true;
	Q.push(start);

	while (!Q.empty())
	{
		// 取出最上面的點
		VoxelID a = Q.front();
		Q.pop();

		// 從六鄰域拜訪過的點中找出此點的最短路徑長度，並設定給distance[a.x][a.y][a.z]
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x + 1, a.y, a.z) && visit[a.x + 1][a.y][a.z] && IsOnSearchRange[a.x + 1][a.y][a.z] && distance[a.x + 1][a.y][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x + 1][a.y][a.z] + 1;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x - 1, a.y, a.z) && visit[a.x - 1][a.y][a.z] && IsOnSearchRange[a.x - 1][a.y][a.z] && distance[a.x - 1][a.y][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x - 1][a.y][a.z] + 1;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y + 1, a.z) && visit[a.x][a.y + 1][a.z] && IsOnSearchRange[a.x][a.y + 1][a.z] && distance[a.x][a.y + 1][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y + 1][a.z] + 1;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y - 1, a.z) && visit[a.x][a.y - 1][a.z] && IsOnSearchRange[a.x][a.y - 1][a.z] && distance[a.x][a.y - 1][a.z] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y - 1][a.z] + 1;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y, a.z + 1) && visit[a.x][a.y][a.z + 1] && IsOnSearchRange[a.x][a.y][a.z + 1] && distance[a.x][a.y][a.z + 1] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y][a.z + 1] + 1;
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y, a.z - 1) && visit[a.x][a.y][a.z - 1] && IsOnSearchRange[a.x][a.y][a.z - 1] && distance[a.x][a.y][a.z - 1] + 1 < distance[a.x][a.y][a.z])
		{
			distance[a.x][a.y][a.z] = distance[a.x][a.y][a.z - 1] + 1;
		}

		////此點已計算出最短路徑->設定為拜訪過
		//visit[a.x][a.y][a.z] = true;

		//將六鄰域中未拜訪過的點加入queue，
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x + 1, a.y, a.z) && !visit[a.x + 1][a.y][a.z] && IsOnSearchRange[a.x + 1][a.y][a.z])
		{
			/*std::cout << "push:" << "{" << a.x + 1 << "," << a.y << "," << a.z << "}" << std::endl;*/
			visit[a.x + 1][a.y][a.z] = true;
			Q.push({ a.x + 1, a.y, a.z });
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x - 1, a.y, a.z) && !visit[a.x - 1][a.y][a.z] && IsOnSearchRange[a.x - 1][a.y][a.z])
		{
			/*std::cout << "push:" << "{" << a.x - 1 << "," << a.y << "," << a.z << "}" << std::endl;*/
			visit[a.x - 1][a.y][a.z] = true;
			Q.push({ a.x - 1, a.y, a.z });
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y + 1, a.z) && !visit[a.x][a.y + 1][a.z] && IsOnSearchRange[a.x][a.y + 1][a.z])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y + 1 << "," << a.z << "}" << std::endl;*/
			visit[a.x][a.y + 1][a.z] = true;
			Q.push({ a.x, a.y + 1, a.z });
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y - 1, a.z) && !visit[a.x][a.y - 1][a.z] && IsOnSearchRange[a.x][a.y - 1][a.z])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y - 1 << "," << a.z << "}" << std::endl;*/
			visit[a.x][a.y - 1][a.z] = true;
			Q.push({ a.x, a.y - 1, a.z });
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y, a.z + 1) && !visit[a.x][a.y][a.z + 1] && IsOnSearchRange[a.x][a.y][a.z + 1])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y << "," << a.z + 1 << "}" << std::endl;*/
			visit[a.x][a.y][a.z + 1] = true;
			Q.push({ a.x, a.y, a.z + 1 });
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(a.x, a.y, a.z - 1) && !visit[a.x][a.y][a.z - 1] && IsOnSearchRange[a.x][a.y][a.z - 1])
		{
			/*std::cout << "push:" << "{" << a.x << "," << a.y << "," << a.z - 1 << "}" << std::endl;*/
			visit[a.x][a.y][a.z - 1] = true;
			Q.push({ a.x, a.y, a.z - 1 });
		}
	}

	//	find path from end to start
	vector<VoxelID> shortest_path;

	VoxelID child = end;
	bool stop = false;
	shortest_path.push_back(end);
	while (!stop)
	{
		// 找到一個最小的當父節點
		float min_distance = 1e9;
		VoxelID parent = {-1,-1,-1};
		if (ptr_VSpace->IsThisVoxelIDSafe(child.x + 1, child.y, child.z) && IsOnSearchRange[child.x + 1][child.y][child.z] && distance[child.x + 1][child.y][child.z] < min_distance)
		{
			parent = { child.x + 1, child.y, child.z };
			min_distance = distance[child.x + 1][child.y][child.z];
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(child.x - 1, child.y, child.z) && IsOnSearchRange[child.x - 1][child.y][child.z] && distance[child.x - 1][child.y][child.z] < min_distance)
		{
			parent = { child.x - 1, child.y, child.z };
			min_distance = distance[child.x - 1][child.y][child.z];
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(child.x, child.y + 1, child.z) && IsOnSearchRange[child.x][child.y + 1][child.z] && distance[child.x][child.y + 1][child.z] < min_distance)
		{
			parent = { child.x, child.y + 1, child.z };
			min_distance = distance[child.x][child.y + 1][child.z];
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(child.x, child.y - 1, child.z) && IsOnSearchRange[child.x][child.y - 1][child.z] && distance[child.x][child.y - 1][child.z] < min_distance)
		{
			parent = { child.x, child.y - 1, child.z };
			min_distance = distance[child.x][child.y - 1][child.z];
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(child.x, child.y, child.z + 1) && IsOnSearchRange[child.x][child.y][child.z + 1] && distance[child.x][child.y][child.z + 1] < min_distance)
		{
			parent = { child.x, child.y, child.z + 1 };
			min_distance = distance[child.x][child.y][child.z + 1];
		}
		if (ptr_VSpace->IsThisVoxelIDSafe(child.x, child.y, child.z - 1) && IsOnSearchRange[child.x][child.y][child.z - 1] && distance[child.x][child.y][child.z - 1] < min_distance)
		{
			parent = { child.x, child.y, child.z - 1 };
			min_distance = distance[child.x][child.y][child.z - 1];
		}

		//	if parent == start then stop
		if (parent.x == start.x && parent.y == start.y && parent.z == start.z)
		{
			stop = true;
			//	add parent to shortest path
			shortest_path.push_back(parent);
		}
		//	else keep searching
		else
		{
			child = parent;
			//	add parent to shortest path
			shortest_path.push_back(parent);
		}
	}

	return shortest_path;
}

//use seedvoxel do bfs search ,find pair belong to vn,but block voxel doesnt need to belong to expand range
vector<VoxelID> FindDesitinationVoxel(VoxelID seedid, vector<VoxelID> vid_list, CVoxelSpace* pVSpace)
{
	VoxelID block_id;
	VoxelID desitination_id;

	bool is_up = !(pVSpace->IsThisVoxelIDSafe(seedid.x, seedid.y + 1, seedid.z)) || (pVSpace->VSmatrix[seedid.x][seedid.y + 1][seedid.z]->getType() == OUTSIDE);
	bool is_down = !(pVSpace->IsThisVoxelIDSafe(seedid.x, seedid.y - 1, seedid.z)) || (pVSpace->VSmatrix[seedid.x][seedid.y - 1][seedid.z]->getType() == OUTSIDE);
	bool is_right = !(pVSpace->IsThisVoxelIDSafe(seedid.x + 1, seedid.y, seedid.z)) || (pVSpace->VSmatrix[seedid.x + 1][seedid.y][seedid.z]->getType() == OUTSIDE);
	bool is_left = !(pVSpace->IsThisVoxelIDSafe(seedid.x - 1, seedid.y, seedid.z)) || (pVSpace->VSmatrix[seedid.x - 1][seedid.y][seedid.z]->getType() == OUTSIDE);
	bool is_front = !(pVSpace->IsThisVoxelIDSafe(seedid.x, seedid.y, seedid.z + 1)) || (pVSpace->VSmatrix[seedid.x][seedid.y][seedid.z + 1]->getType() == OUTSIDE);
	bool is_back = !(pVSpace->IsThisVoxelIDSafe(seedid.x, seedid.y, seedid.z - 1)) || (pVSpace->VSmatrix[seedid.x][seedid.y][seedid.z - 1]->getType() == OUTSIDE);

	vector<vector<vector<bool>>> visit;
	visit.resize(pVSpace->resolution_x);
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		visit[x].resize(pVSpace->resolution_y);
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			visit[x][y].resize(pVSpace->resolution_z);
		}
	}
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < pVSpace->resolution_z; z++)
			{
				visit[x][y][z] == false;
			}
		}
	}

	vector<vector<vector<bool>>> IsBelongToVidList;
	IsBelongToVidList.resize(pVSpace->resolution_x);
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		IsBelongToVidList[x].resize(pVSpace->resolution_y);
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			IsBelongToVidList[x][y].resize(pVSpace->resolution_z);
		}
	}
	for (size_t x = 0; x < pVSpace->resolution_x; x++)
	{
		for (size_t y = 0; y < pVSpace->resolution_y; y++)
		{
			for (size_t z = 0; z < pVSpace->resolution_z; z++)
			{
				IsBelongToVidList[x][y][z] = false;
			}
		}
	}
	for (size_t i = 0; i < vid_list.size(); i++)
	{
		IsBelongToVidList[vid_list[i].x][vid_list[i].y][vid_list[i].z] = true;
	}

	std::queue<VoxelID> bfs_queue;
	//	push root(seed voxel id) into queue and set visit[root] = true
	if (!is_down && IsBelongToVidList[seedid.x][seedid.y - 1][seedid.z] == true)
	{
		bfs_queue.push({ seedid.x, seedid.y - 1, seedid.z });
		visit[seedid.x][seedid.y - 1][seedid.z] = true;
	}
	if (!is_up && IsBelongToVidList[seedid.x][seedid.y + 1][seedid.z] == true)
	{
		bfs_queue.push({ seedid.x, seedid.y + 1, seedid.z });
		visit[seedid.x][seedid.y + 1][seedid.z] = true;
	}
	if (!is_right && IsBelongToVidList[seedid.x + 1][seedid.y][seedid.z] == true)
	{
		bfs_queue.push({ seedid.x + 1, seedid.y, seedid.z });
		visit[seedid.x + 1][seedid.y][seedid.z] = true;
	}
	if (!is_left && IsBelongToVidList[seedid.x - 1][seedid.y][seedid.z] == true)
	{
		bfs_queue.push({ seedid.x - 1, seedid.y, seedid.z });
		visit[seedid.x - 1][seedid.y][seedid.z] = true;
	}
	if (!is_front && IsBelongToVidList[seedid.x][seedid.y][seedid.z + 1] == true)
	{
		bfs_queue.push({ seedid.x, seedid.y, seedid.z + 1 });
		visit[seedid.x][seedid.y][seedid.z + 1] = true;
	}
	if (!is_back && IsBelongToVidList[seedid.x][seedid.y][seedid.z - 1] == true)
	{
		bfs_queue.push({ seedid.x, seedid.y, seedid.z - 1 });
		visit[seedid.x][seedid.y][seedid.z - 1] = true;
	}

	while (!bfs_queue.empty())
	{
		VoxelID from = bfs_queue.front();
		bfs_queue.pop();

		//	try to get (block/desitination) pair from 
		if (is_right)
		{
			if (pVSpace->IsThisVoxelIDSafe(from.x - 1, from.y, from.z) && IsBelongToVidList[from.x - 1][from.y][from.z] && !visit[from.x - 1][from.y][from.z])
			{
				block_id = from;
				desitination_id = { from.x - 1, from.y, from.z };
				break;
			}
		}
		else if (is_left)
		{
			if (pVSpace->IsThisVoxelIDSafe(from.x + 1, from.y, from.z) && IsBelongToVidList[from.x + 1][from.y][from.z] && !visit[from.x + 1][from.y][from.z])
			{
				block_id = from;
				desitination_id = { from.x + 1, from.y, from.z };
				break;
			}
		}
		else if (is_front)
		{
			if (pVSpace->IsThisVoxelIDSafe(from.x, from.y, from.z - 1) && IsBelongToVidList[from.x][from.y][from.z - 1] && !visit[from.x][from.y][from.z - 1])
			{
				block_id = from;
				desitination_id = { from.x, from.y, from.z - 1 };
				break;
			}
		}
		else if (is_back)
		{
			if (pVSpace->IsThisVoxelIDSafe(from.x, from.y, from.z + 1) && IsBelongToVidList[from.x][from.y][from.z + 1] && !visit[from.x][from.y][from.z + 1])
			{
				block_id = from;
				desitination_id = { from.x, from.y, from.z + 1 };
				break;
			}
		}

		//	push the rest nonvisit point to queue
		if (pVSpace->IsThisVoxelIDSafe(from.x, from.y + 1, from.z) && IsBelongToVidList[from.x][from.y + 1][from.z] && visit[from.x][from.y + 1][from.z] == false)
		{
			bfs_queue.push({ from.x, from.y + 1, from.z });
			visit[from.x][from.y + 1][from.z] = true;
		}
		if (pVSpace->IsThisVoxelIDSafe(from.x, from.y - 1, from.z) && IsBelongToVidList[from.x][from.y - 1][from.z] && visit[from.x][from.y - 1][from.z] == false)
		{
			bfs_queue.push({ from.x, from.y - 1, from.z });
			visit[from.x][from.y - 1][from.z] = true;
		}
		if (pVSpace->IsThisVoxelIDSafe(from.x + 1, from.y, from.z) && IsBelongToVidList[from.x + 1][from.y][from.z] && visit[from.x + 1][from.y][from.z] == false)
		{
			bfs_queue.push({ from.x + 1, from.y, from.z });
			visit[from.x + 1][from.y][from.z] = true;
		}
		if (pVSpace->IsThisVoxelIDSafe(from.x - 1, from.y, from.z) && IsBelongToVidList[from.x - 1][from.y][from.z] && visit[from.x - 1][from.y][from.z] == false)
		{
			bfs_queue.push({ from.x - 1, from.y, from.z });
			visit[from.x - 1][from.y][from.z] = true;
		}

		if (pVSpace->IsThisVoxelIDSafe(from.x, from.y, from.z + 1) && IsBelongToVidList[from.x][from.y][from.z + 1] && visit[from.x][from.y][from.z + 1] == false)
		{
			bfs_queue.push({ from.x, from.y, from.z + 1 });
			visit[from.x][from.y][from.z + 1] = true;
		}
		if (pVSpace->IsThisVoxelIDSafe(from.x, from.y, from.z - 1) && IsBelongToVidList[from.x][from.y][from.z - 1] && visit[from.x][from.y][from.z - 1] == false)
		{
			bfs_queue.push({ from.x, from.y, from.z - 1 });
			visit[from.x][from.y][from.z - 1] = true;
		}
	}

	vector<VoxelID> result;
	result.push_back(block_id);
	result.push_back(desitination_id);
	return result;
}