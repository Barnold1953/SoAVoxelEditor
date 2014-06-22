#pragma once
#include <GL/GL.h>
#include <string>

using namespace std;

//Describes a voxel, for flyweight programming pattern
struct VoxelType
{
    string name;
    GLubyte color[4];
};

//Grid voxel
struct Voxel{
    char type;
    bool selected;
};