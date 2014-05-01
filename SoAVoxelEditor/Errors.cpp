#pragma once
//
// Errors.cpp
//
// Copyright 2014 Seed Of Andromeda
//
#include "Errors.h"
#include <fstream>

void error(string msg)
{
	cerr << "Error: " << msg << "\n";
	ofstream out("errors.txt");
	if (!out.fail()){
		out << "Error: " << msg << "\n";
		out.close();
	}
}

void error(const char *msg)
{
	cerr << "Error: " << msg << "\n";
	ofstream out("errors.txt");
	if (!out.fail()){
		out << "Error: " << msg << "\n";
		out.close();
	}
}