#pragma once

//Class that handles the graphics and rendering for voxels
static class VoxelRenderer
{
public:
    static void initialize(int w, int h, int l);
    static void drawVoxels(class Camera *camera);

private:
    static bool _changed;
    static vector <BlockVertex> _currentVerts;
    static GLuint *_currentIndices;
    static BlockMesh _baseMesh;
};

