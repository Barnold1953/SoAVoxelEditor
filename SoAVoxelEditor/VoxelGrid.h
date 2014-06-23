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
    VoxelGrid(int, int, int);
    void addVoxel(Voxel*, int, int, int);
    void removeVoxel(int, int, int);
    Voxel* getVoxel(int, int, int);

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

