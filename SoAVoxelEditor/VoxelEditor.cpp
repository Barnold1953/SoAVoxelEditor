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
	//_voxelGrid = new VoxelGrid(10, 10, 10);
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

	if (_state == 'i'){
		glm::vec3 temp;
		Voxel *tempVox = _voxelGrid->getVoxel(_currentIntersect.x, _currentIntersect.y, _currentIntersect.z);
		if (_currentIntersect.y < 0){
			temp = _currentIntersect - (_clickDirection * _step);
			tempVox = _voxelGrid->getVoxel(temp.x, temp.y, temp.z);
			if (tempVox != NULL){
				RenderUtil::drawReferenceVoxel(camera, glm::vec3((int)temp.x, (int)temp.y, (int)temp.z), _brushVoxelCoords);
			}
		}
		else if (tempVox != NULL){
			if (tempVox->type != '\0'){
				temp = _currentIntersect - (_clickDirection * _step);
				tempVox = _voxelGrid->getVoxel(temp.x, temp.y, temp.z);
				if (tempVox != NULL){
					RenderUtil::drawReferenceVoxel(camera, glm::vec3((int)temp.x, (int)temp.y, (int)temp.z), _brushVoxelCoords);
				}
			}
		}
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

		c->v = new Voxel;
		*c->v = *_currentVoxel;

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
		
		c->v = new Voxel;
		*c->v = *_currentVoxel;

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
					c->v = new Voxel();
                    *c->v = *_currentVoxel;
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
				Voxel tv = *_voxelGrid->getVoxel(i, j, k);
                if(_voxelGrid->removeVoxel(i, j, k)) {
                    Command* c = new Command;
                    c->type = 'r';
                    c->coord.x = i;
                    c->coord.y = j;
                    c->coord.z = k;
					*c->v = tv;
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

bool VoxelEditor::removeCheck(glm::vec3 location, glm::vec3 direction){
	if (location.x < 0 && direction.x < 0)
		return 1;
	if (location.y < 0 && direction.y < 0)
		return 1;
	if (location.z < 0 && direction.z < 0)
		return 1;
	return 0;
}

void VoxelEditor::handleClick(){
	glm::vec3 tempV = _currentIntersect;
	Voxel *tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);

	printf("Intersect at <%f,%f,%f>\n", (float)tempV.x, (float)tempV.y, (float)tempV.z);

	switch (_state) {
	case 's':
		if (_currentIntersect.y < 0) {
			tempV -= (_clickDirection * _step);
			tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
			if (tempVox != NULL) {
				if (!_selectedFirstBlock) {
					_selectedFirstBlock = true;
					_selectedX1 = tempV.x;
					_selectedY1 = tempV.y;
					_selectedZ1 = tempV.z;
					std::cout << "X1: " << _selectedX1 << " Y1: " << _selectedY1 << " Z1: " << _selectedZ1 << std::endl;
				}
				else if (!_selectedSecondBlock) {
					_selectedSecondBlock = true;
					_selectedX2 = tempV.x;
					_selectedY2 = tempV.y;
					_selectedZ2 = tempV.z;
					std::cout << "X2: " << _selectedX2 << " Y2: " << _selectedY2 << " Z2: " << _selectedZ2 << std::endl;
				}
				else {
					_selectedFirstBlock = false;
					_selectedSecondBlock = false;
					std::cout << "Removed selected volume" << std::endl;
				}
			}
		}
		else if (tempVox != NULL) {
			if (tempVox->type != '\0') {
				//tempV = _clickDirection * (i - _step) + _clickStart;
				tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
				if (tempVox != NULL) {
					if (!_selectedFirstBlock) {
						_selectedFirstBlock = true;
						_selectedX1 = tempV.x;
						_selectedY1 = tempV.y;
						_selectedZ1 = tempV.z;
						std::cout << "X1: " << _selectedX1 << " Y1: " << _selectedY1 << " Z1: " << _selectedZ1 << std::endl;
					}
					else if (!_selectedSecondBlock) {
						_selectedSecondBlock = true;
						_selectedX2 = tempV.x;
						_selectedY2 = tempV.y;
						_selectedZ2 = tempV.z;
						std::cout << "X2: " << _selectedX2 << " Y2: " << _selectedY2 << " Z2: " << _selectedZ2 << std::endl;
					}
					else {
						_selectedFirstBlock = false;
						_selectedSecondBlock = false;
						std::cout << "Removed selected volume" << std::endl;
					}
				}
			}
		}
		break;
	case 'i':
		if (tempV.y < 0){
			tempV -= (_clickDirection * _step);
			tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
			if (tempVox != NULL){
				if (_brushVoxelCoords.size() > 0)
					drawBrush();
				else
					addVoxel(tempV.x, tempV.y, tempV.z);
			}
		}
		else if (tempVox != NULL){
			if (tempVox->type != '\0'){
				tempV -= (_clickDirection * _step); 
				tempVox = _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z);
				if (tempVox != NULL){
					if (tempVox->type == '\0'){
						glm::vec3 apos = tempV;
					}
					if (_brushVoxelCoords.size() > 0)
						drawBrush();
					else
						addVoxel(tempV.x, tempV.y, tempV.z);
				}
			}
		}
		break;
	case 'r':
		if (tempVox != NULL){
			if (tempVox->type != '\0'){
				removeVoxel(tempV.x, tempV.y, tempV.z);
			}
		}
		break;
	}
}

void VoxelEditor::findIntersect(const glm::vec3 &startPosition, const glm::vec3 &direction) {
	float i = 0.1f;
    glm::vec3 base = direction, tempV;
    Voxel *tempVox;

	_clickStart = startPosition;
	_clickDirection = direction;

    drawDebugLine = false;
    debugP1 = startPosition;

	while (i < _maxStep){
		tempV = direction * i + startPosition;
		if (_voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z) != NULL && _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z)->type != 0 && _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z)->type != '\0'){
			//cout << _voxelGrid->getVoxel(tempV.x, tempV.y, tempV.z)->type << endl;
			_currentIntersect = tempV;
			break;
		}
		else if ((tempV.x < 0 && direction.x < 0) || (tempV.z < 0 && direction.z < 0) || (tempV.y < 0 && direction.y < 0)){
			_currentIntersect = tempV;
			break;
		}
		else{
			_currentIntersect = tempV;
		//	break;
		}
		i += _step;
	}


    debugP2 = tempV;
}

void VoxelEditor::brushRange(){
	if (_brushVoxelCoords.size() > 0){
		_brushVoxelCoords.clear();
		_brushVoxels.clear();
		printf("Brush cleared\n");
	}
	else if (_selectedFirstBlock && _selectedSecondBlock && _state == 's') {
		makeBrush(_selectedX1, _selectedY1, _selectedZ1, _selectedX2, _selectedY2, _selectedZ2);
	}
}

void VoxelEditor::makeBrush(int x1, int y1, int z1, int x2, int y2, int z2){
	Voxel *tv;
	
	int startX = x1 < x2 ? x1 : x2;
	int startY = y1 < y2 ? y1 : y2;
	int startZ = z1 < z2 ? z1 : z2;

	int endX = x2 <= x1 ? x1 : x2;
	int endY = y2 <= y1 ? y1 : y2;
	int endZ = z2 <= z1 ? z1 : z2;
	for (int i = startX; i <= endX; i++) {
		for (int j = startY; j <= endY; j++) {
			for (int k = startZ; k <= endZ; k++) {
				tv = _voxelGrid->getVoxel(i, j, k);
				if (tv != nullptr && tv->type != 0){
					_brushVoxelCoords.push_back(glm::vec3(i - startX, j - startY, k - startZ));
					_brushVoxels.push_back(*tv);
				}
			}
		}
	}
	printf("Brush created\n");
}

void VoxelEditor::drawBrush(){
	_currentIntersect -= _step * _clickDirection;
	for (int i = 0; i < _brushVoxelCoords.size(); i++){
		if ((int)_currentIntersect.x + _brushVoxelCoords[i].x >= _voxelGrid->getWidth())
			return;
		if ((int)_currentIntersect.y + _brushVoxelCoords[i].y >= _voxelGrid->getHeight())
			return;
		if ((int)_currentIntersect.z + _brushVoxelCoords[i].z >= _voxelGrid->getHeight())
			return;
		if (_voxelGrid->getVoxel((int)_currentIntersect.x + _brushVoxelCoords[i].x, (int)_currentIntersect.y + _brushVoxelCoords[i].y, (int)_currentIntersect.z + _brushVoxelCoords[i].z)->type != 0){
			printf("Brush blocked\n");
			return;
		}
	}
	
	vector <Command*> tempComList;

	for (int i = 0; i < _brushVoxels.size(); i++){
		_voxelGrid->addVoxel(_brushVoxels[i], _brushVoxelCoords[i].x + (int)_currentIntersect.x, _brushVoxelCoords[i].y + (int)_currentIntersect.y, _brushVoxelCoords[i].z + (int)_currentIntersect.z);
		Command* c = new Command;
		c->type = 'i';
		c->coord.x = _brushVoxelCoords[i].x + (int)_currentIntersect.x;
		c->coord.y = _brushVoxelCoords[i].y + (int)_currentIntersect.y;
		c->coord.z = _brushVoxelCoords[i].z + (int)_currentIntersect.z;
		c->v = new Voxel;
		*c->v = _brushVoxels[i];
		tempComList.push_back(c);
	}

	if (tempComList.size() > 0) newCommand(tempComList);
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