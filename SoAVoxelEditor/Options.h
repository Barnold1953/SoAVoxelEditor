#pragma once
//
// Options.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <iostream>
#include <string>

using namespace std;


//struct that stores all the graphics options, such as resolution, fps, ect.
struct GraphicsOptions
{
	//dumb constructor sets up the defaults with an initialization list
	GraphicsOptions() : screenWidth(1024), screenHeight(768), maxFps(60), fov(75), isFullscreen(0), gamma(1.0), isVsync(1), isBorderless(0) {}

	int screenWidth, screenHeight; 
	int nativeWidth, nativeHeight; //stores the native screenWidth and screenHeight for this users monitor
	int maxFps;
	int fov;
	float gamma; //TODO: make use of this
	bool isFullscreen;
	bool isBorderless;
	bool isVsync;
};

//we only use a single global instance of the class, this is essentially the singleton pattern.
extern GraphicsOptions graphicsOptions; 

//controls options, like mouse speed, key bindings, ect
struct ControlsOptions
{
	ControlsOptions() : mouseSensitivity(1.0), invertMouse(0) {}
	
	//TODO: make use of these
	float mouseSensitivity;
	bool invertMouse;
};

extern ControlsOptions controlsOptions;

int saveOptions(const string fileName);

int loadOptions(const string fileName);