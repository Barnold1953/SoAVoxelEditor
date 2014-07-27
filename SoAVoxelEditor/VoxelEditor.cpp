#include "VoxelEditor.h"
#include "Camera.h"
#include "VoxelRenderer.h"
#include "Voxel.h"
#include "RenderUtil.h"
#include <iostream>
#include <string>

//
// VoxelEditor.cpp
//
// Copyright 2014 Seed Of Andromeda
//

VoxelEditor::VoxelEditor(): _voxelGrid(NULL), _currentVoxel(NULL), _state('i'), _selectedFirstBlock(false), _selectedSecondBlock(false)
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
    _currentVoxel->color[0] = 0;
    _currentVoxel->color[1] = 255;
    _currentVoxel->color[2] = 255;
    _currentVoxel->color[3] = 255;
    VoxelRenderer::initialize(_voxelGrid->getWidth(), _voxelGrid->getHeight(), _voxelGrid->getLength());
}

void VoxelEditor::draw(Camera *camera) {
    _voxelGrid->drawGrid(camera);

    if(_selectedFirstBlock && _selectedSecondBlock) {
        int startX = _selectedX1 < _selectedX2 ? _selectedX1 : _selectedX2;
        int startY = _selectedY1 < _selectedY2 ? _selectedY1 : _selectedY2;
        int startZ = _selectedZ1 < _selectedZ2 ? _selectedZ1 : _selectedZ2;

        int endX = _selectedX2 <= _selectedX1 ? _selectedX1 : _selectedX2;
        int endY = _selectedY2 <= _selectedY1 ? _selectedY1 : _selectedY2;
        int endZ = _selectedZ2 <= _selectedZ1 ? _selectedZ1 : _selectedZ2;
        RenderUtil::drawWireframeBox(camera, glm::vec3(startX, startY, startZ), glm::vec3(endX - startX + 1, endY - startY + 1, endZ - startZ + 1), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    } else if(_selectedFirstBlock) {
        RenderUtil::drawWireframeBox(camera, glm::vec3(_selectedX1, _selectedY1, _selectedZ1), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    }

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

		GLubyte color[4];
		char type;
		color[0] = _currentVoxel->color[0];
		color[1] = _currentVoxel->color[1];
		color[2] = _currentVoxel->color[2];
		color[3] = _currentVoxel->color[3];
		type = _currentVoxel->type;
		c->v = new Voxel;
		c->v->color[0] = color[0];
		c->v->color[1] = color[1];
		c->v->color[2] = color[2];
		c->v->color[3] = color[3];
		c->v->type = type;

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
		
		GLubyte color[4];
		char type;
		color[0] = _currentVoxel->color[0];
		color[1] = _currentVoxel->color[1];
		color[2] = _currentVoxel->color[2];
		color[3] = _currentVoxel->color[3];
		type = _currentVoxel->type;
		c->v = new Voxel;
		c->v->color[0] = color[0];
		c->v->color[1] = color[1];
		c->v->color[2] = color[2];
		c->v->color[3] = color[3];
		c->v->type = type;

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
    if(_selectedFirstBlock && _selectedSecondBlock) {
        fillRange(_selectedX1, _selectedY1, _selectedZ1, _selectedX2, _selectedY2, _selectedZ2);
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
    if(_selectedFirstBlock && _selectedSecondBlock) {
        removeRange(_selectedX1, _selectedY1, _selectedZ1, _selectedX2, _selectedY2, _selectedZ2);
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

    drawDebugLine = false;
    debugP1 = startPosition;

    const float step = 0.1f;
    const float maxStep = 50.0f;

    while (i < maxStep){
        tempV = direction * i + startPosition;
        if (tempV.x >= 0 && tempV.z >= 0){
            tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);

            switch (_state) {
            case 's':
                if(tempV.y < 0) {
                    tempV = direction * (i - step) + startPosition;
                    tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
                    if(tempVox != NULL) {
                        if(!_selectedFirstBlock) {
                            _selectedFirstBlock = true;
                            _selectedX1 = tempV.x;
                            _selectedY1 = tempV.y;
                            _selectedZ1 = tempV.z;
                            std::cout << "X1: " << _selectedX1 << " Y1: " << _selectedY1 << " Z1: " << _selectedZ1 << std::endl;
                        } else if(!_selectedSecondBlock) {
                            _selectedSecondBlock = true;
                            _selectedX2 = tempV.x;
                            _selectedY2 = tempV.y;
                            _selectedZ2 = tempV.z;
                            std::cout << "X2: " << _selectedX2 << " Y2: " << _selectedY2 << " Z2: " << _selectedZ2 << std::endl;
                        } else {
                            _selectedFirstBlock = false;
                            _selectedSecondBlock = false;
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
                            if(!_selectedFirstBlock) {
                                _selectedFirstBlock = true;
                                _selectedX1 = tempV.x;
                                _selectedY1 = tempV.y;
                                _selectedZ1 = tempV.z;
                                std::cout << "X1: " << _selectedX1 << " Y1: " << _selectedY1 << " Z1: " << _selectedZ1 << std::endl;
                            } else if(!_selectedSecondBlock) {
                                _selectedSecondBlock = true;
                                _selectedX2 = tempV.x;
                                _selectedY2 = tempV.y;
                                _selectedZ2 = tempV.z;
                                std::cout << "X2: " << _selectedX2 << " Y2: " << _selectedY2 << " Z2: " << _selectedZ2 << std::endl;
                            } else {
                                _selectedFirstBlock = false;
                                _selectedSecondBlock = false;
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
			break;
		case 'r':
			_voxelGrid->addVoxel(*lCom[i]->v, lCom[i]->coord.x, lCom[i]->coord.y, lCom[i]->coord.z);
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

void VoxelEditor::setCurrentVoxel(const Voxel& voxel) {
    for(int i = 0; i < 4; i++)
        _currentVoxel->color[i] = voxel.color[i];
    _currentVoxel->type = voxel.type;
}