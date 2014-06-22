#include "VoxelEditor.h"
#include "Camera.h"
#include "VoxelRenderer.h"

//
// VoxelEditor.cpp
//
// Copyright 2014 Seed Of Andromeda
//

VoxelEditor::VoxelEditor() : voxelGrid(NULL), currentVoxel(NULL), state('s')
{
}


VoxelEditor::~VoxelEditor()
{
}

void VoxelEditor::initialize() {
    voxelGrid = new VoxelGrid(10, 10, 10);
    currentVoxel = new Voxel;
    currentVoxel->type = 'b';
    currentVoxel->selected = false;

    VoxelRenderer::initialize(voxelGrid->w, voxelGrid->h, voxelGrid->l);
}

void VoxelEditor::draw(Camera *camera) {
    voxelGrid->drawGrid(camera);
    voxelGrid->drawVoxels(camera);
}

void VoxelEditor::update() {
}

void VoxelEditor::addVoxel(int x, int y, int z) {
    voxelGrid->addVoxel(currentVoxel, x, y, z);
}

void VoxelEditor::removeVoxel(int x, int y, int z) {
    voxelGrid->removeVoxel(x, y, z);
}

void VoxelEditor::cycleState() {
    if (state == 's'){
        printf("Insert mode activated\n");
        state = 'i';
    } else if (state == 'i'){
        printf("Remove mode activated\n");
        state = 'r';
    } else {
        printf("Select mode activated\n");
        state = 's';
    }
}

//Couldn't think of a better name. If this is a debug function lets remove it, it doesn't seem practical
void VoxelEditor::toggleFillGrid() {
    if (voxelGrid->vTot > 0){
        voxelGrid->clearGrid();
    } else{
        voxelGrid->fillGrid();
    }
}