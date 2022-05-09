#include "ResPuzzleModel.h"
#include "ResPuzzleView.h"
#include <vector>
#include <queue> //std::queue

#pragma once
void InitializeRemainData(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

void CalculateAccessibility(int recursion, float weight, CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

void SeperateColorGroup(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

GLVoxelModel*  CreateModelByEachGroup(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

GLVoxelModel*  CreateModelByOriginal(CVoxelSpace* pVSpace);

GLVoxelModel* CreateModelByDistanceTransform(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

GLVoxelModel* CreateModelByAccessibility(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

GLVoxelModel* CreateModelByPuzzle(CVoxelSpace* ptr_VSpace, PuzzlePiece puzzle);

void SortColorGroupByAccessibility(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

bool ColorgroupCompareByAccessibility(colorgroup a,colorgroup b);

std::queue<VoxelID> PickCandidateSetForP1ByY(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

bool CandidateVoxelCompareByY(VoxelID a, VoxelID b);

std::queue<VoxelID> PickCandidateSetForP1(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

PuzzlePiece /*GLVoxelModel**/ CreatePuzzleForP1(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

/*PuzzlePiece*/ GLVoxelModel* CreatePuzzleForP2(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata, PuzzlePiece* p1);

GLVoxelModel* /*bool*/ IsPuzzleSuccess(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata, PuzzlePiece* puzzle);

int getColorGroup(const int group_id, remain_data* ptr_rdata);

void CalculateDistanceTransform(CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

vec3f CalculateDistanceGradient(VoxelID vox, CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

GLfloat Vec3fDotProduct(vec3f a, vec3f b);

vector<VoxelID> FindShortestPath(VoxelID start, VoxelID end, CVoxelSpace* pVSpace, vector<VoxelID> vid_list, vector<vector<vector<float>>>& vid_accessibility);

vector<VoxelID> FindShortestPath(VoxelID start, VoxelID end, vector<VoxelID> vid_list, CVoxelSpace* ptr_VSpace, remain_data* ptr_rdata);

vector<VoxelID> FindDesitinationVoxel(VoxelID seedid, vector<VoxelID> vid_list, CVoxelSpace* pVSpace);
