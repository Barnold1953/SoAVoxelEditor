#pragma once
#include "GlobalStructs.h"
#include "Direction.h"
#include <vector>

//Forward declarations so we dont have to include camera or voxel here
class Camera;
struct Voxel;

class VoxelGrid {
public:
    VoxelGrid(int width, int height, int length);
    bool addVoxel(const Voxel& newV, int x, int y, int z);//changed these to bools so editor knows if a command needs to be added to the command stack
    bool removeVoxel(int x, int y, int z);
    Voxel* getVoxel(int x, int y, int z);

    void setVoxelSideVisiblity(const int x, const int y, const int z, const Direction side, const bool visible);
    bool isVoxelSideVisible(const int x, const int y, const int z, const Direction side) const;
    inline bool isVoxelSideVisibleNoBoundsCheck(const int x, const int y, const int z, const Direction side) const;
    bool isVoxelVisible(const int x, const int y, const int z);

    void drawVoxels(Camera *camera);
    void drawGrid(Camera *camera);

    //getters
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    int getLength() const { return _length; }
    int getSize() const { return _size; }
    int getVTot() const { return _voxelCount; }

private:
    int _height, _width, _length, _layerSize, _size, _voxelCount;
    Voxel* _voxels;
};

