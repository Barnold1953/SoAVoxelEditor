#pragma once

//Class that handles the graphics and rendering for voxels
static class VoxelRenderer
{
public:
    static void initialize(int w, int h, int l);
    static void drawVoxels(class Camera *camera);
    static void addVoxel(int x, int y, int z);
    static void removeVoxel(int x, int y, int z);

private:
    static bool _changed;
    static vector <BlockVertex> _currentVerts;
    static GLuint *_currentIndices;
    static BlockMesh _baseMesh;
};

