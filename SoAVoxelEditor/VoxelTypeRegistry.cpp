#include "VoxelTypeRegistry.h"

VoxelTypeRegistry::VoxelTypeRegistry() {
    VoxelType* type = new VoxelType();
    type->name = "NULL";
    registerVoxelType(type);
}

VoxelTypeRegistry::~VoxelTypeRegistry() {
    for(auto i = _voxelTypes.begin(); i != _voxelTypes.end(); i++) {
        delete i->second;
    }
}

void VoxelTypeRegistry::registerVoxelType(VoxelType* type) {
    auto tmpIter = _voxelTypes.find(type->id);
    if(tmpIter != _voxelTypes.end()) {
        type->id = _voxelTypes.size();
        _voxelTypes[type->id] = type;
    }
}

VoxelType* VoxelTypeRegistry::getVoxelType(unsigned short id) {
    auto i = _voxelTypes.find(id);
    if(i != _voxelTypes.end()) {
        return i->second;
    } else {
        return nullptr;
    }
}
