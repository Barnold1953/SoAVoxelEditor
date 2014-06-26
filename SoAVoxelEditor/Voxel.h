#pragma once
#include <GL/GL.h>
#include <string>

using namespace std;

//Describes a voxel, for flyweight programming pattern
struct VoxelType
{
    string name;
};

//Grid voxel
struct Voxel{
    char type;
    bool selected;
	GLubyte color[4];
};