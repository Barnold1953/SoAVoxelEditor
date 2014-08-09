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

struct Command{
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

    void fillSelected();
    void fillRange(int x1, int y1, int z1, int x2, int y2, int z2);
    void removeSelected();
    void removeRange(int x1, int y1, int z1, int x2, int y2, int z2);

    void cycleState();
	char getState(){ return _state; }
    void toggleFillGrid();
	
    void newCommand(vector <Command*> lCom);//I use vectors since when we have brushes you will want to change more than one voxel with an undo/redo call
	void undo();
	void redo();
    void execute(vector <Command*> lCom);

	void handleClick();
    void findIntersect(const glm::vec3 &startPosition, const glm::vec3 &direction);
	bool removeCheck(glm::vec3 location, glm::vec3 direction);
	glm::vec3 getIntersect(){ return _currentIntersect; }

    void setCurrentVoxel(const Voxel& voxel);
    Voxel* getCurrentVoxel() { return _currentVoxel; }
	void drawReferenceBlock(glm::vec3 intersect);

private:
	const float _step = 0.1f, _maxStep = 50.0f;
	glm::vec3 _clickStart, _clickDirection;
	VoxelGrid *_voxelGrid;
    Voxel *_currentVoxel;
	int _width, _height;
    char _state;
    vector < vector <Command*> > _commandStack, _fluxStack;
    bool _selectedFirstBlock;
    int _selectedX1, _selectedY1, _selectedZ1;
    bool _selectedSecondBlock;
    int _selectedX2, _selectedY2, _selectedZ2;
	glm::vec3 _currentIntersect, _currentMarker;
};

