#include "stdafx.h"
#include "Voxel.h"

CVoxel::CVoxel()
{
	this->type = UNDEFINED;
	this->color = { 255, 255, 255, 0 };
}

CVoxel::~CVoxel()
{
}

// return voxel type
VoxelType CVoxel::getType()
{
	return this->type;
}

// return voxel's color
VoxelColor CVoxel::getColor()
{
	return this->color;
}


// set voxel's type
void CVoxel::setType(VoxelType type)
{
	this->type = type;
}

// set voxel's color
void CVoxel::setColor(VoxelColor color)
{
	this->color = color;
}

CVoxel::CVoxel(VoxelID id, VoxelType type, VoxelColor color)
{
	this->type = type;
	this->id = id;
	this->color = color;
}

bool ISThose2VoxelSameColor(CVoxel* a, CVoxel* b)
{
	VoxelColor VC_a = a->getColor();
	VoxelColor VC_b = b->getColor();

	if ((VC_a.R == VC_b.R) && (VC_a.G == VC_b.G) && (VC_a.B == VC_b.B) && (VC_a.A == VC_b.A))
	{
		return true;
	}
	else
	{
		return false;
	}

}

VoxelID CVoxel::getID()
{
	return this->id;
}

void CVoxel::setID(VoxelID id)
{
	this->id = id;
}