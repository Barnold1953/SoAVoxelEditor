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

struct Voxel;

struct command{
	char type;
	glm::vec3 coord;
	Voxel *v;
};

class VoxelEditor
{
public:
	VoxelEditor();
	~VoxelEditor();

	void initialize();

	void draw(class Camera *camera);

	void update();

    void addVoxel(int x, int y, int z);
    void removeVoxel(int x, int y, int z);

    void cycleState();
    void toggleFillGrid();
	
	void newCommand(map<char, command> *lCom);//I use map since when we have brushes you will want to change more than one voxel with an undo/redo call
	void undo();
	void redo();
	void execute(map<char, command> *lCom);

    void findIntersect(const glm::vec3 &startPosition, const glm::vec3 &direction);

private:
	VoxelGrid *_voxelGrid;
    Voxel *_currentVoxel;
	int _width, _height;
    char _state;
	vector < map<char, command> *> _commandStack, _fluxStack;
};

