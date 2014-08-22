#pragma once
#include <GL\glew.h>
#include <string>

using namespace std;

//Describes a voxel, for flyweight programming pattern
struct VoxelType {
    unsigned short id; // This value should only be changed by the VoxelTypeRegistry
    string name;
};

//Grid voxel
struct Voxel {
    unsigned short type;
	GLubyte color[4];
    char sideVisibility;
};