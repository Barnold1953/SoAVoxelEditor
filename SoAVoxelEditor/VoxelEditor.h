#pragma once
//
// VoxelEditor.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <GL/glew.h>
#include <string>

#include "VoxelGrid.h"

using namespace std;

class VoxelEditor
{
public:
	VoxelEditor();
	~VoxelEditor();

	void initialize();

	void draw(class Camera *camera);

	void update();

    void addVoxel(int, int, int);
    void removeVoxel(int, int, int);

    void cycleState();
    void toggleFillGrid();

    void findIntersect(const glm::vec3 &startPosition, const glm::vec3 &direction);

private:
	VoxelGrid *_voxelGrid; 
    Voxel *_currentVoxel;
	int _width, _height;
    char _state;
};

