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
    bool addVoxel(Voxel* newV, int x, int y, int z);//changed these to bools so editor knows if a command needs to be added to the command stack
    bool removeVoxel(int x, int y, int z);
    Voxel* getVoxel(int x, int y, int z);

    void drawVoxels(Camera *camera);
    void drawGrid(Camera *camera);

    //getters
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    int getLength() const { return _length; }
    int getVTot() const { return _vTot; }

private:
    int _height, _width, _length, _layerSize, _vTot;
    vector <Voxel*> _voxels;
};

