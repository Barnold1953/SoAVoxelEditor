#pragma once
#include "Voxel.h"

#include <map>

class VoxelTypeRegistry {
public:
    VoxelTypeRegistry();
    ~VoxelTypeRegistry();

    void registerVoxelType(VoxelType* type);

    VoxelType* getVoxelType(unsigned short id);

private:
    std::map<unsigned short, VoxelType*> _voxelTypes;
};

