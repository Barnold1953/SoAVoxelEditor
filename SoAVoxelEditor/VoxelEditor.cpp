#include "VoxelEditor.h"
#include "Camera.h"
#include "VoxelRenderer.h"
#include "Voxel.h"

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

    VoxelRenderer::initialize(_voxelGrid->getWidth(), _voxelGrid->getHeight(), _voxelGrid->getLength());
}

void VoxelEditor::draw(Camera *camera) {
    _voxelGrid->drawGrid(camera);
    _voxelGrid->drawVoxels(camera);
}

void VoxelEditor::update() {
}

void VoxelEditor::addVoxel(int x, int y, int z) {
	if (_voxelGrid->addVoxel(_currentVoxel, x, y, z) == 1){
		map<char, command> *tempComList = new map<char, command>;
		command c;
		c.type = 'i';
		c.coord.x = x;
		c.coord.y = y;
		c.coord.z = z;
		c.v = _currentVoxel;
		tempComList->insert(make_pair('i', c));
		_commandStack.push_back(tempComList);
		_fluxStack.clear();
	}
}

void VoxelEditor::removeVoxel(int x, int y, int z) {
	if (_voxelGrid->removeVoxel(x, y, z) == 1){
		map<char, command> *tempComList = new map<char, command>;
		command c;
		c.type = 'r';
		c.coord.x = x;
		c.coord.y = y;
		c.coord.z = z;
		c.v = _currentVoxel;
		tempComList->insert(make_pair('r', c));
		_commandStack.push_back(tempComList);
		_fluxStack.clear();
	}
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

void VoxelEditor::findIntersect(const glm::vec3 &startPosition, const glm::vec3 &direction) {
    float i = 0.1f;
    glm::vec3 base = direction, tempV;
    Voxel *tempVox;

    drawDebugLine = 1;
    debugP1 = startPosition;

    const float step = 0.1f;
    const float maxStep = 50.0f;

    while (i < maxStep){
        tempV = direction * i + startPosition;
        if (tempV.x >= 0 && tempV.z >= 0){
            tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);

            switch (_state) {
            case 's':
                if (tempV.z < 0){
                    i = maxStep; //force it to stop
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
                        addVoxel(tempV.x, tempV.y, tempV.z);
                    }
                    i = maxStep; //force it to stop
                    break;
                } else if (tempVox != NULL){
                    if (tempVox->type != '\0'){
                        tempV = direction * (i - step) + startPosition;
                        tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
                        if (tempVox != NULL){
                            if (tempVox->type == '\0'){
                                glm::vec3 apos = tempV;
                            }
                            addVoxel(tempV.x, tempV.y, tempV.z);
                        }
                        i = maxStep; //force it to stop
                        break;
                    }
                }
                break;
            case 'r':
                if (tempV.z < 0 && direction.z < 0){
                    i = maxStep; //force it to stop
                    break;
                }
                if (tempVox != NULL){
                    if (tempVox->type != '\0'){
                        removeVoxel(tempV.x, tempV.y, tempV.z);
                        i = maxStep; //force it to stop
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

//a more functionality is added, more cases need to be created for undo/redo
void VoxelEditor::newCommand(map<char, command> *lCom){
	_commandStack.push_back(lCom);
}

void VoxelEditor::execute(map<char, command> *lCom){
	map<char, command>::iterator it;

	for (it = lCom->begin(); it != lCom->end(); it++){
		switch (it->first)
		{
		case 'i':
			_voxelGrid->removeVoxel(it->second.coord.x, it->second.coord.y, it->second.coord.z);
			printf("executing remove\n");
			break;
		case 'r':
			_voxelGrid->addVoxel(it->second.v, it->second.coord.x, it->second.coord.y, it->second.coord.z);
			printf("executing insert\n");
			break;
		}
	}
}

void VoxelEditor::undo(){
	if (_commandStack.size() < 1){
		printf("_commandStack is empty\n");
		return;
	}

	map<char, command> *lCom, *newCom;
	map<char, command>::iterator it;

	newCom = new map<char, command>;
	lCom = _commandStack[_commandStack.size() - 1];
	execute(lCom);

	for (it = lCom->begin(); it != lCom->end(); it++){
		if (it->first == 'i'){
			newCom->insert(make_pair('r', it->second));
		}
		else if (it->first == 'r'){
			newCom->insert(make_pair('i', it->second));
		}
	}
	
	_fluxStack.push_back(newCom);
	_commandStack.pop_back();
}

void VoxelEditor::redo(){
	if (_fluxStack.size() < 1){
		printf("_fluxStack is empty\n");
		return;
	}
	map<char, command> *lCom, *newCom;
	map<char, command>::iterator it;

	newCom = new map<char, command>;
	lCom = _fluxStack[_fluxStack.size() - 1];
	execute(lCom);

	for (it = lCom->begin(); it != lCom->end(); it++){
		if (it->first == 'i'){
			newCom->insert(make_pair('r', it->second));
		}
		else if (it->first == 'r'){
			newCom->insert(make_pair('i', it->second));
		}
	}

	_commandStack.push_back(newCom);
	_fluxStack.pop_back();
}