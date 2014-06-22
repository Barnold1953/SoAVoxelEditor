#include "VoxelEditor.h"
#include "Camera.h"
#include "VoxelRenderer.h"

//
// VoxelEditor.cpp
//
// Copyright 2014 Seed Of Andromeda
//

VoxelEditor::VoxelEditor() : voxelGrid(NULL), currentVoxel(NULL)
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

void VoxelEditor::placeVoxels() {

}

void VoxelEditor::removeVoxels() {

}