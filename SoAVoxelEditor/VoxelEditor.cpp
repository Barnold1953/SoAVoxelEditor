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


void VoxelEditor::findIntersect(const glm::vec3 &startPosition, const glm::vec3 &direction) {
    float i = 0.1f;
    glm::vec3 base = direction, tempV;
    Voxel *tempVox;

    drawDebugLine = 1;
    debugP1 = startPosition;

    const float step = 0.1f;

    while (i < 50.0){
        tempV = direction * i + startPosition;
        if (tempV.x >= 0 && tempV.z >= 0){
            tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);

            switch (_state) {
            case 's':
                if (tempV.z < 0){
                    break;
                }
                if (tempVox != NULL){
                    if (tempVox->type != '\0'){
                        tempVox->selected = !(tempVox->selected);
                        VoxelRenderer::selectVoxel(tempV.x, tempV.y, tempV.z, tempVox->selected);
                    }
                }
                break;
            case 'i':
                if (tempV.y < 0){
                    tempV = direction * (i - step) + startPosition;
                    tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
                    if (tempVox != NULL){
                        _voxelGrid->addVoxel(_currentVoxel, tempV.x, tempV.y, tempV.z);
                        printf("addVoxel attempted at <%f,%f,%f>\n", tempV.x, tempV.y, tempV.z);
                    }
                    break;
                } else if (tempVox != NULL){
                    if (tempVox->type != '\0'){
                        tempV = direction * (i - step) + startPosition;
                        tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
                        if (tempVox != NULL){
                            if (tempVox->type == '\0'){
                                glm::vec3 apos = tempV;
                            }
                            _voxelGrid->addVoxel(_currentVoxel, tempV.x, tempV.y, tempV.z);
                            printf("addVoxel attempted at <%f,%f,%f>\n", tempV.x, tempV.y, tempV.z);
                        }
                        break;
                    }
                }
                break;
            case 'r':
                if (tempV.z < 0 && direction.z < 0){
                    break;
                }
                if (tempVox != NULL){
                    if (tempVox->type != '\0'){
                        _voxelGrid->removeVoxel(tempV.x, tempV.y, tempV.z);
                        break;
                    }
                }
                break;
            }
        }
        i += step;
    }

    debugP2 = tempV;
}