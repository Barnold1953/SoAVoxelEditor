#pragma once
#define _CRT_SECURE_NO_WARNINGS
//
// FileManager.cpp
//
// Copyright 2014 Seed Of Andromeda
//
#include "FileManager.h"
#include <fstream>
#include <map>
#include <string>
#include "Errors.h"

FileManager fileManager;

FileManager::FileManager()
{
}
void FileManager::initialize()
{
	registerIniKeys();
}

//if we use a map to look up keys and bind them to an integer index, then we can
//use a switch statement, rather than a huge if else block.

map <string, INI_KEYS> iniMap;

//this function binds each string that can occur as a key in an .ini file to an integer so we can easily use
//a switch statement to parse it
void FileManager::registerIniKeys()
{
	iniMap["none"] = INI_NONE;
	iniMap["screenWidth"] = INI_SCREENWIDTH;
	iniMap["screenHeight"] = INI_SCREENHEIGHT;
	iniMap["maxFps"] = INI_MAXFPS;
	iniMap["invertMouse"] = INI_INVERTMOUSE;
	iniMap["mouseSensitivity"] = INI_MOUSESENSITIVITY;
	iniMap["fullscreen"] = INI_FULLSCREEN;
	iniMap["fov"] = INI_FOV;
	iniMap["gamma"] = INI_GAMMA;
	iniMap["vSync"] = INI_VSYNC;
	iniMap["borderless"] = INI_BORDERLESS;
}


//loads a .ini file. Might not be EXACTLY like the spec, but it works fine. Stores the fields in iniValues, and the seconds in iniSections
//each element i of iniValues is a vector containing iniValues correspoinding to the section i of iniSections
//returns 1 on failure
int FileManager::loadIniFile(string filePath, vector <vector <IniValue> > &iniValues, vector <string> &iniSections)
{
	char buffer[1024];
	int n;
	string currentName = "";
	string s, s2;
	ifstream file;
	int currSection = 0;

	iniSections.push_back(""); //since sections are optional, we first push the empty section as element 0.
	iniValues.push_back(vector <IniValue>());

	if (filePath.length() == 0) return 1;
	file.open((filePath).c_str());
	if (file.fail()){
		error(filePath + " could not be opened for read");
		perror((filePath).c_str());
		return 1;
	}
	while (file.getline(buffer, 1024)){
		if (buffer[0] == '#' || buffer[0] == ';') continue;
		if (strlen(buffer) <= 2) continue;

		s.clear();
		if (buffer[0] == '['){ //grab the section name
			for (n = 1; buffer[n] != ']' && buffer[n] != '\0'; n++) s += buffer[n];
			currentName = s;
			iniSections.push_back(currentName);
			iniValues.push_back(vector <IniValue>());
			currSection++;
		}
		else{ //its a key value pair

			for (n = 0; buffer[n] == ' ' || buffer[n] == '\t'; n++); //ignore spaces and tabs
			for (n; buffer[n] != '='; n++) s += buffer[n]; //grab the string name
			for (n; buffer[n] == ' ' || buffer[n] == '\t'; n++); //ignore spaces and tabs
			n++; //pass '='
			for (n; buffer[n] == ' ' || buffer[n] == '\t'; n++); //ignore spaces and tabs

			iniValues[currSection].push_back(IniValue());
			iniValues[currSection].back().key = s;


			s2.clear();
			for (n; buffer[n] != '\n' && buffer[n] != '\0'; n++) s2 += buffer[n]; //grab the string value
			iniValues[currSection].back().val = s2;
		}
	}
	file.close();
	return 0;
}

//saves an .ini file using the values and sections specified in the parameters. Returns 1 on fail
int FileManager::saveIniFile(string filePath, vector <vector <IniValue> > &iniValues, vector <string> &iniSections)
{
	ofstream file;
	file.open(filePath.c_str());
	if (file.fail()){
		error(filePath + " could not be opened for write");
		perror((filePath).c_str());
		return 1;
	}

	for (int i = 0; i < iniSections.size(); i++)
	{
		if (iniSections[i].size()){ file << "[" + iniSections[i] + "]\n"; }

		for (int j = 0; j < iniValues[i].size(); j++){
			if (iniValues[i][j].isFloat){
				file << iniValues[i][j].key << "= " << iniValues[i][j].fval << "\n";
			}
			else{
				file << iniValues[i][j].key << "= " << iniValues[i][j].val << "\n";
			}
		}
	}
	file.close();
	return 0;
}

//returns the key associated with the value string
INI_KEYS FileManager::getIniVal(const string &s)
{
	auto it = iniMap.find(s);
	if (it == iniMap.end()){
		return INI_NONE;
	}
	else{
		return it->second;
	}
}

//stores a non float in iniValue
IniValue::IniValue(const string &k, const string &v) : isFloat(0)
{
	key = k;
	val = v;
}
//stores a double in inivalue
IniValue::IniValue(const string &k, const double f) : isFloat(1)
{
	key = k;
	fval = f;
}
//returns a bool value
bool IniValue::GetBool(){
	int i;
	if (sscanf(val.c_str(), "%d", &i) != 1){
		error("failure parsing ini file. " + key + " expects bool value type. Value: " + val);
	}
	return (i != 0);
}

//returns ini value as int
int IniValue::GetInt(){
	int rv;
	if (sscanf(val.c_str(), "%d", &rv) != 1){
		error("failure parsing ini file. " + key + " expects integer value type. Value: " + val);
	}
	return rv;
}

//returns ini value as double
float IniValue::GetFloat(){
	float rv;
	if (sscanf(val.c_str(), "%f", &rv) != 1){
		error("failure parsing ini file. " + key + " expects float value type. Value: " + val);
	}
	return rv;
}

//returns ini value as double
double IniValue::GetDouble(){
	double rv;
	if (sscanf(val.c_str(), "%lf", &rv) != 1){
		error("failure parsing ini file. " + key + " expects double value type. Value: " + val);
	}
	return rv;
}

//returns ini value as str
string IniValue::GetStr(){
	return val;
}
