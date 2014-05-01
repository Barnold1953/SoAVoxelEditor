#pragma once
//
// Options.cpp
//
// Copyright 2014 Seed Of Andromeda
//
#include "Options.h"
#include "FileManager.h"

GraphicsOptions graphicsOptions;
ControlsOptions controlsOptions;

//saves the options in .ini format
//returns 1 for failure
int saveOptions(const string fileName)
{
	vector <vector <IniValue> > iniValues;
	vector <string> iniSections;

	iniSections.push_back("GraphicsOptions");
	iniValues.push_back(vector<IniValue>());


	iniValues.back().push_back(IniValue("borderless", to_string(graphicsOptions.isBorderless)));
	iniValues.back().push_back(IniValue("fov", graphicsOptions.fov));
	iniValues.back().push_back(IniValue("fullScreen", to_string(graphicsOptions.isFullscreen)));
	iniValues.back().push_back(IniValue("gamma", graphicsOptions.gamma));
	iniValues.back().push_back(IniValue("screenWidth", to_string(graphicsOptions.screenWidth)));
	iniValues.back().push_back(IniValue("screenHeight", to_string(graphicsOptions.screenHeight)));
	iniValues.back().push_back(IniValue("maxFps", to_string(graphicsOptions.maxFps)));
	iniValues.back().push_back(IniValue("vSync", to_string(graphicsOptions.isVsync)));

	iniSections.push_back("ControlsOptions");
	iniValues.push_back(vector<IniValue>());
	iniValues.back().push_back(IniValue("invertMouse", to_string(controlsOptions.invertMouse)));
	iniValues.back().push_back(IniValue("mouseSensitivity", controlsOptions.mouseSensitivity));

	if (fileManager.saveIniFile(fileName, iniValues, iniSections)) return 1;
	return 0;
}

//loads the options in .ini format
//returns 1 for failure
int loadOptions(const string fileName)
{
	vector <vector <IniValue> > iniValues;
	vector <string> iniSections;
	if (fileManager.loadIniFile(fileName, iniValues, iniSections)) return 1;

	int iVal;
	IniValue *iniVal;
	for (int i = 0; i < iniSections.size(); i++){
		for (int j = 0; j < iniValues[i].size(); j++){
			iniVal = &(iniValues[i][j]);

			iVal = fileManager.getIniVal(iniVal->key);
			switch (iVal){
			case INI_SCREENWIDTH:
				graphicsOptions.screenWidth = iniVal->GetInt();
				break;
			case INI_SCREENHEIGHT:
				graphicsOptions.screenHeight = iniVal->GetInt();
				break;
			case INI_MAXFPS:
				graphicsOptions.maxFps = iniVal->GetInt();
				break;
			case INI_MOUSESENSITIVITY:
				controlsOptions.mouseSensitivity = iniVal->GetFloat();
				break;
			case INI_INVERTMOUSE:
				controlsOptions.invertMouse = iniVal->GetBool();
				break;
			case INI_FULLSCREEN:
				graphicsOptions.isFullscreen = iniVal->GetBool();
				break;
			case INI_VSYNC:
				graphicsOptions.isVsync = iniVal->GetBool();
				break;
			case INI_GAMMA:
				graphicsOptions.gamma = iniVal->GetFloat();
				break;
			case INI_BORDERLESS:
				graphicsOptions.isBorderless = iniVal->GetBool();
				break;
			}
		}
	}
	return 0;
}