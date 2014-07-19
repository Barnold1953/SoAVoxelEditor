#include "VoxelEditor.h"
#include "Camera.h"
#include "VoxelRenderer.h"
#include "Voxel.h"
#include <iostream>
#include<string>

//
// VoxelEditor.cpp
//
// Copyright 2014 Seed Of Andromeda
//

VoxelEditor::VoxelEditor(): _voxelGrid(NULL), _currentVoxel(NULL), _state('i'), selectedFirstBlock(false), selectedSecondBlock(false)
{
}


VoxelEditor::~VoxelEditor()
{
}

void VoxelEditor::initialize() {
    std::cout << "Please input a size for your model" << std::endl;
    int width;
    int height;
    int length;
    std::string input;
    std::cin >> input;
    width = std::stoi(input);
    std::cin >> input;
    height = std::stoi(input);
    std::cin >> input;
    length = std::stoi(input);
    _voxelGrid = new VoxelGrid(width, height, length);
    _currentVoxel = new Voxel;
    _currentVoxel->type = 'b';
    _currentVoxel->selected = false;
    _currentVoxel->color[0] = 0;
    _currentVoxel->color[1] = 255;
    _currentVoxel->color[2] = 255;
    _currentVoxel->color[3] = 255;
    VoxelRenderer::initialize(_voxelGrid->getWidth(), _voxelGrid->getHeight(), _voxelGrid->getLength());
}

void VoxelEditor::draw(Camera *camera) {
    _voxelGrid->drawGrid(camera);
    _voxelGrid->drawVoxels(camera);
}

void VoxelEditor::update() {
}

void VoxelEditor::addVoxel(int x, int y, int z) {
	if (_voxelGrid->addVoxel(*_currentVoxel, x, y, z)){
        vector <Command*> tempComList;
        Command* c = new Command;
		c->type = 'i';
		c->coord.x = x;
		c->coord.y = y;
		c->coord.z = z;
		c->v = _currentVoxel;
		tempComList.push_back(c);
		newCommand(tempComList);
	}
}

void VoxelEditor::removeVoxel(int x, int y, int z) {
	if (_voxelGrid->removeVoxel(x, y, z)){
        vector <Command*> tempComList;
        Command* c = new Command;
		c->type = 'r';
		c->coord.x = x;
		c->coord.y = y;
		c->coord.z = z;
		c->v = _currentVoxel;
		tempComList.push_back(c);
		newCommand(tempComList);
	}
}

void VoxelEditor::fillRange(int x1, int y1, int z1, int x2, int y2, int z2) {
    vector <Command*> tempComList;
    int startX = x1 < x2 ? x1 : x2;
    int startY = y1 < y2 ? y1 : y2;
    int startZ = z1 < z2 ? z1 : z2;

    int endX = x2 <= x1 ? x1 : x2;
    int endY = y2 <= y1 ? y1 : y2;
    int endZ = z2 <= z1 ? z1 : z2;
    for(int i = startX; i <= endX; i++) {
        for(int j = startY; j <= endY; j++) {
            for(int k = startZ; k <= endZ; k++) {
                if(_voxelGrid->addVoxel(*_currentVoxel, i, j, k)) {
                    Command* c = new Command;
                    c->type = 'i';
                    c->coord.x = i;
                    c->coord.y = j;
                    c->coord.z = k;
                    c->v = _currentVoxel;
                    tempComList.push_back(c);
                }
            }
        }
    }
    if(tempComList.size() > 0) {
        newCommand(tempComList);
    }
}

void VoxelEditor::fillSelected() {
    if(selectedFirstBlock && selectedSecondBlock) {
        fillRange(selectedX1, selectedY1, selectedZ1, selectedX2, selectedY2, selectedZ2);
    }
}

void VoxelEditor::removeRange(int x1, int y1, int z1, int x2, int y2, int z2) {
    vector <Command*> tempComList;
    int startX = x1 < x2 ? x1 : x2;
    int startY = y1 < y2 ? y1 : y2;
    int startZ = z1 < z2 ? z1 : z2;

    int endX = x2 <= x1 ? x1 : x2;
    int endY = y2 <= y1 ? y1 : y2;
    int endZ = z2 <= z1 ? z1 : z2;
    for(int i = startX; i <= endX; i++) {
        for(int j = startY; j <= endY; j++) {
            for(int k = startZ; k <= endZ; k++) {
                if(_voxelGrid->removeVoxel(i, j, k)) {
                    Command* c = new Command;
                    c->type = 'r';
                    c->coord.x = i;
                    c->coord.y = j;
                    c->coord.z = k;
                    c->v = _currentVoxel;
                    tempComList.push_back(c);
                }
            }
        }
    }
    if(tempComList.size() > 0) newCommand(tempComList);
}

void VoxelEditor::removeSelected() {
    if(selectedFirstBlock && selectedSecondBlock) {
        removeRange(selectedX1, selectedY1, selectedZ1, selectedX2, selectedY2, selectedZ2);
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
                /*
                if(tempV.z < 0) {
                    i = maxStep; //force it to stop
                    break;
                }
                if(tempVox != NULL) {
                    if(!selectedFirstBlock) {
                        selectedFirstBlock = true;
                        selectedX1 = tempV.x;
                        selectedY1 = tempV.y;
                        selectedZ1 = tempV.z;
                        std::cout << "X1: " << selectedX1 << " Y1: " << selectedY1 << " Z1: " << selectedZ1 << std::endl;
                    } else if(!selectedSecondBlock) {
                        selectedSecondBlock = true;
                        selectedX2 = tempV.x;
                        selectedY2 = tempV.y;
                        selectedZ2 = tempV.z;
                        std::cout << "X2: " << selectedX2 << " Y2: " << selectedY2 << " Z2: " << selectedZ2 << std::endl;
                    } else {
                        selectedFirstBlock = false;
                        selectedSecondBlock = false;
                        std::cout << "Removed selected volume" << std::endl;
                    }
                    i = maxStep;
                }
                */
                if(tempV.y < 0) {
                    tempV = direction * (i - step) + startPosition;
                    tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
                    if(tempVox != NULL) {
                        if(!selectedFirstBlock) {
                            selectedFirstBlock = true;
                            selectedX1 = tempV.x;
                            selectedY1 = tempV.y;
                            selectedZ1 = tempV.z;
                            std::cout << "X1: " << selectedX1 << " Y1: " << selectedY1 << " Z1: " << selectedZ1 << std::endl;
                        } else if(!selectedSecondBlock) {
                            selectedSecondBlock = true;
                            selectedX2 = tempV.x;
                            selectedY2 = tempV.y;
                            selectedZ2 = tempV.z;
                            std::cout << "X2: " << selectedX2 << " Y2: " << selectedY2 << " Z2: " << selectedZ2 << std::endl;
                        } else {
                            selectedFirstBlock = false;
                            selectedSecondBlock = false;
                            std::cout << "Removed selected volume" << std::endl;
                        }
                    }
                    i = maxStep; //force it to stop
                    break;
                } else if(tempVox != NULL) {
                    if(tempVox->type != '\0') {
                        tempV = direction * (i - step) + startPosition;
                        tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
                        if(tempVox != NULL) {
                            if(!selectedFirstBlock) {
                                selectedFirstBlock = true;
                                selectedX1 = tempV.x;
                                selectedY1 = tempV.y;
                                selectedZ1 = tempV.z;
                                std::cout << "X1: " << selectedX1 << " Y1: " << selectedY1 << " Z1: " << selectedZ1 << std::endl;
                            } else if(!selectedSecondBlock) {
                                selectedSecondBlock = true;
                                selectedX2 = tempV.x;
                                selectedY2 = tempV.y;
                                selectedZ2 = tempV.z;
                                std::cout << "X2: " << selectedX2 << " Y2: " << selectedY2 << " Z2: " << selectedZ2 << std::endl;
                            } else {
                                selectedFirstBlock = false;
                                selectedSecondBlock = false;
                                std::cout << "Removed selected volume" << std::endl;
                            }
                        }
                        i = maxStep; //force it to stop
                        break;
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
void VoxelEditor::newCommand(vector <Command*> lCom) {
	_commandStack.push_back(lCom);
	for (int i = 0; i < _fluxStack.size(); i++){
		for (int j = 0; j < _fluxStack[i].size(); j++){
			delete _fluxStack[i][j];
		}
	}
	_fluxStack.clear();
}

void VoxelEditor::execute(vector <Command*> lCom) {
	for (int i = 0; i < lCom.size(); i++){
		switch (lCom[i]->type)
		{
		case 'i':
			_voxelGrid->removeVoxel(lCom[i]->coord.x, lCom[i]->coord.y, lCom[i]->coord.z);
			//printf("executing remove\n");
			break;
		case 'r':
			_voxelGrid->addVoxel(*lCom[i]->v, lCom[i]->coord.x, lCom[i]->coord.y, lCom[i]->coord.z);
			//printf("executing insert\n");
			break;
		}
	}
}

void VoxelEditor::undo(){
	if (_commandStack.size() < 1){
		printf("_commandStack is empty\n");
		return;
	}

    vector <Command*> lCom;

	lCom = _commandStack[_commandStack.size() - 1];
	execute(lCom);

	for (int i = 0; i < lCom.size(); i++){
		if (lCom[i]->type == 'i'){
			lCom[i]->type = 'r';
		}
		else if (lCom[i]->type == 'r'){
			lCom[i]->type = 'i';
		}
	}
	
	_fluxStack.push_back(lCom);
	_commandStack.pop_back();
}

void VoxelEditor::redo(){
	if (_fluxStack.size() < 1){
		printf("_fluxStack is empty\n");
		return;
	}

    vector <Command*> lCom;
	
	lCom = _fluxStack[_fluxStack.size() - 1];
	execute(lCom);

	for (int i = 0; i < lCom.size(); i++){
		if (lCom[i]->type == 'i'){
			lCom[i]->type = 'r';
		}
		else if (lCom[i]->type == 'r'){
			lCom[i]->type = 'i';
		}
	}

	_commandStack.push_back(lCom);
	_fluxStack.pop_back();
}