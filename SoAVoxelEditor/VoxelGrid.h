#pragma once
#include "GlobalStructs.h"

#include <iostream>
#include <vector>

using namespace std;

//Forward declarations so we dont have to include camera or voxel here
class Camera;
struct Voxel;

class VoxelGrid
{
public:
    VoxelGrid(int width, int height, int length);
    void addVoxel(Voxel* newV, int x, int y, int z);
    void removeVoxel(int x, int y, int z);
    Voxel* getVoxel(int x, int y, int z);

    void drawVoxels(Camera *camera);
    void drawGrid(Camera *camera);

    void clearGrid();
    void fillGrid(Voxel *voxel); //not sure if we need this

    //getters
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    int getLength() const { return _length; }
    int getVTot() const { return _vTot; }

private:
    int _height, _width, _length, _layerSize, _vTot;
    vector <Voxel*> _voxels;
};

