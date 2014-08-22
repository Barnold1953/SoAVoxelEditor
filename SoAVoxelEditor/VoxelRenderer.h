#pragma once
#include "GlobalStructs.h"


//Class that handles the graphics and rendering for voxels
class VoxelRenderer {
public:
    static void initialize(int w, int h, int l);
    static void drawVoxels(class Camera* camera);
    static void addVoxel(int x, int y, int z, const GLubyte* color);
    static void removeVoxel(int x, int y, int z);

private:
    static long long getIndex(int x, int y, int z);
    static glm::vec3 getCoord(long long index);

    static std::map<long long, glm::vec4> _voxels;
    static Mesh _mesh;

    static int _width;
    static int _height;
    static int _length;
};

