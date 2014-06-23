#pragma once
#include "Texture.h"
#include <map>

using namespace std;

//Static Texture management class
class TextureManager
{
public:
    static void loadTextures();
    static Texture* getCubeTexture(char tag);
    static Texture* getSelectedTexture(char tag);
private:
    static map <char, Texture*> cubeTextures;
    static map <char, Texture*> cubeSelectedTextures;
};

