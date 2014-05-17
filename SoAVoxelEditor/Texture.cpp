#include "Texture.h"
//
// Shader.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <iostream>
#include <vector>
#include <fstream>
//#include <string>
#include "Errors.h"

//should work with BMP, PNG, JPG, TGA, DDS, PSD, HDR
//
texture::texture(string location){
	data = SOIL_load_OGL_texture
		(
		location.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	if (data == 0){
		printf("SOIL loading error: '%s'.  Press any key to exit.\n", SOIL_last_result());
		string a;
		cin >> a;
		exit(100);
	}
}