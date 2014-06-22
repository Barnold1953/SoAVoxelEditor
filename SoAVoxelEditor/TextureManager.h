#pragma once
#include "Texture.h"
#include <map>

using namespace std;

//Static Texture management class
class TextureManager
{
public:
    static void loadTextures();
    inline static texture* getCubeTexture(char tag);
    inline static texture* getSelectedTexture(char tag);
private:
    static map <char, texture*> cubeTextures;
    static map <char, texture*> cubeSelectedTextures;
};

