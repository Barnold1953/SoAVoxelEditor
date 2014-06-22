#include "TextureManager.h"

//Static declaration
map <char, Texture*> TextureManager::cubeTextures;
map <char, Texture*> TextureManager::cubeSelectedTextures;

void TextureManager::loadTextures() {
    cubeTextures.insert(make_pair('b', new Texture("Data/black.png", 'b')));
    cubeSelectedTextures.insert(make_pair('b', new Texture("Data/blackSelected.png", 'b')));
}

//Returns the cube texture associated with tag
Texture* TextureManager::getCubeTexture(char tag) {
    return cubeTextures[tag];
}

//Returns the cube selected texture associated with tag
Texture* TextureManager::getSelectedTexture(char tag) {
    return cubeSelectedTextures[tag];
}