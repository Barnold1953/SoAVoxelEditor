#include "VoxelEditor.h"
#include "Camera.h"
#include "VoxelRenderer.h"

//
// VoxelEditor.cpp
//
// Copyright 2014 Seed Of Andromeda
//

VoxelEditor::VoxelEditor() : _voxelGrid(NULL), _currentVoxel(NULL), _state('s')
{
}


VoxelEditor::~VoxelEditor()
{
}

void VoxelEditor::initialize() {
    _voxelGrid = new VoxelGrid(10, 10, 10);
    _currentVoxel = new Voxel;
    _currentVoxel->type = 'b';
    _currentVoxel->selected = false;

    VoxelRenderer::initialize(_voxelGrid->_width, _voxelGrid->_height, _voxelGrid->_length);
}

void VoxelEditor::draw(Camera *camera) {
    _voxelGrid->drawGrid(camera);
    _voxelGrid->drawVoxels(camera);
}

void VoxelEditor::update() {
}

void VoxelEditor::addVoxel(int x, int y, int z) {
    _voxelGrid->addVoxel(_currentVoxel, x, y, z);
}

void VoxelEditor::removeVoxel(int x, int y, int z) {
    _voxelGrid->removeVoxel(x, y, z);
}

void VoxelEditor::cycleState() {
    if (_state == 's'){
        printf("Insert mode activated\n");
        _state = 'i';
    } else if (_state == 'i'){
        printf("Remove mode activated\n");
        _state = 'r';
    } else {
        printf("Select mode activated\n");
        _state = 's';
    }
}

//Couldn't think of a better name. If this is a debug function lets remove it, it doesn't seem practical
void VoxelEditor::toggleFillGrid() {
    if (_voxelGrid->_vTot > 0){
        _voxelGrid->clearGrid();
    } else{
        _voxelGrid->fillGrid();
    }
}