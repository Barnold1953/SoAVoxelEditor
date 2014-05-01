#pragma once
//
// FileManager.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <vector>
#include <string>

using namespace std;

//struct used when loading .ini files
//it stores a key and value, and can return
//the value as several types of data
struct IniValue{
	IniValue() : isFloat(0){}
	IniValue(const string &k, const string &v);
	IniValue(const string &k, const double f);
	string key;
	string val;
	double fval;
	bool isFloat;

	bool GetBool();
	int GetInt();
	float GetFloat();
	double GetDouble();
	string GetStr();
};

//this enum represents the indexes for all keys that care registered for .ini files in this application
enum INI_KEYS{INI_NONE, INI_SCREENWIDTH, INI_SCREENHEIGHT, INI_MAXFPS, INI_INVERTMOUSE, INI_MOUSESENSITIVITY, INI_FULLSCREEN, INI_FOV, INI_GAMMA, INI_VSYNC, INI_BORDERLESS};

//This class holds a bunch of input and output helper functions
class FileManager
{
public:
	FileManager();
	void initialize();

	int loadIniFile(string filePath, vector <vector <IniValue> > &iniValues, vector <string> &iniSections);
	int saveIniFile(string filePath, vector <vector <IniValue> > &iniValues, vector <string> &iniSections);
	INI_KEYS getIniVal(const string &s);

private:
	void registerIniKeys();
};

extern FileManager fileManager;

