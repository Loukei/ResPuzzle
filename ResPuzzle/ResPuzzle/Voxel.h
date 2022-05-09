#pragma once
//=============
//VoxelType
//=============
enum VoxelType { INSIDE, SHELL, OUTSIDE, UNDEFINED };

struct VoxelColor
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;

	inline bool operator == (const VoxelColor& a_color) const
	{
		if (this->R == a_color.R && this->G == a_color.G && this->B == a_color.B && this->A == a_color.A)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool operator != (const VoxelColor& a_color) const
	{
		if (this->R == a_color.R && this->G == a_color.G && this->B == a_color.B && this->A == a_color.A)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
};

struct VoxelID
{
	int x;
	int y;
	int z;

	inline bool operator == (const VoxelID& a_id) const
	{
		if (this->x == a_id.x && this->y == a_id.y && this->z == a_id.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool operator != (const VoxelID& a_id) const
	{
		if (this->x == a_id.x && this->y == a_id.y && this->z == a_id.z)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
};

/// <summary>
/// CVoxel�y�z:
/// ��§@���@��voxel��ƪ�����A����voxel��������T�åB�إߦs���o�Ǹ�ƪ��覡
/// �n�`�N���O�@���`�٪Ŷ����Ҷq�A�������d��O��CVoxelSpace�Τ@�����åB�b�ݭnopenglø�X�ɥ�CVoxelSpace�Y�ɹB��
/// </summary>
class CVoxel
{
private:
	VoxelID id;			//	voxel's id
	VoxelType type;		//	voxel's type
	VoxelColor color;	//	voxel's color

public:

	CVoxel();

	CVoxel(VoxelID id, VoxelType type, VoxelColor color);

	~CVoxel();

	// return voxel type
	VoxelType getType();

	// return voxel id
	VoxelID getID();

	// return voxel's color
	VoxelColor getColor();

	//	set voxel's type
	void setType(VoxelType type);

	// set voxel's color
	void setColor(VoxelColor color);

	// set voxel's id
	void setID(VoxelID id);

	friend bool ISThose2VoxelSameColor(CVoxel* a, CVoxel* b);
};

/// EXAMPLE
/*
	#include "Voxel.h"
	...
	CVoxel vox(SHELL, {255,255,255,0});
	VoxelType type  = vox.getType();
	VoxelColor color = vox.getColor();
	float a = vox.getAccessibility();
	*/