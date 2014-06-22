#pragma once
#include "GlobalStructs.h"
#include <iostream>
#include <vector>

using namespace std;

//Describes a voxel, for flyweight programming pattern
struct VoxelType
{
    string name;
    GLubyte color[4];
};

//Grid voxel
struct voxel{
    char type;
    bool selected;
};

class VoxelGrid
{
public:
    VoxelGrid(int, int, int);
    void addVoxel(voxel*, int, int, int);
    void removeVoxel(int, int, int);
    voxel* getVoxel(int, int, int);

    void drawVoxels(Camera *camera);
    void drawGrid(Camera *camera);

    int h, w, l, layerSize, vTot;
private:
    vector <voxel*> voxels;
};

