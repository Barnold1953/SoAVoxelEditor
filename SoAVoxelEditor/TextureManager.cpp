#include "TextureManager.h"

//Static declaration
map <char, texture*> TextureManager::cubeTextures;
map <char, texture*> TextureManager::cubeSelectedTextures;

void TextureManager::loadTextures() {
    cubeTextures.insert(make_pair('b', new texture("Data/black.png", 'b')));
    cubeSelectedTextures.insert(make_pair('b', new texture("Data/blackSelected.png", 'b')));
}

//Returns the cube texture associated with tag
texture* TextureManager::getCubeTexture(char tag) {
    return cubeTextures[tag];
}

//Returns the cube selected texture associated with tag
texture* TextureManager::getSelectedTexture(char tag) {
    return cubeSelectedTextures[tag];
}