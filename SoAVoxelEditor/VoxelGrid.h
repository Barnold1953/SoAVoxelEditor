#pragma once
#include "GlobalStructs.h"
#include "Voxel.h"
#include <iostream>
#include <vector>

using namespace std;

class VoxelGrid
{
public:
    VoxelGrid(int, int, int);
    void addVoxel(Voxel*, int, int, int);
    void removeVoxel(int, int, int);
    Voxel* getVoxel(int, int, int);

    void drawVoxels(Camera *camera);
    void drawGrid(Camera *camera);

    int h, w, l, layerSize, vTot;
private:
    vector <Voxel*> voxels;
};

