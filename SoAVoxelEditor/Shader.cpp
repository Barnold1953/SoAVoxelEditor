#include "Shader.h"
//
// Shader.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <iostream>
#include <vector>
#include <fstream>
#include "Errors.h"

BlockShader blockShader;
GridShader gridShader;

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path, GLuint &VertexShaderID, GLuint &FragmentShaderID);
void LinkShaders(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID);

//loads the shader files
void BlockShader::initialize(string dirPath)
{
	if (initialized) return;
	cout << "Loading blockShader\n";

	GLuint vID, fID;
	shaderID = LoadShaders((dirPath + "BlockShading.vert").c_str(), (dirPath + "BlockShading.frag").c_str(), vID, fID);
	glBindAttribLocation(shaderID, 0, "vertexPosition");
	glBindAttribLocation(shaderID, 1, "vertexColor");
	glBindAttribLocation(shaderID, 2, "vertexNormal");
	glBindAttribLocation(shaderID, 3, "textCoordinate");
	glBindAttribLocation(shaderID, 4, "textureType");
	LinkShaders(shaderID, vID, fID);

	mvpID = GetUniform(shaderID, "MVP");

	lightPosID = GetUniform(shaderID, "lightPosition_worldspace");

	textPosID = GetUniform(shaderID, "texture");
	textSelPosID = GetUniform(shaderID, "selTexture");

	//selected = GetUniform(shaderID, "selected");

	initialized = 1;
}

void BlockShader::bind()
{
	if (!initialized){
		error("SHADER BOUND BEFORE INITIALIZATION");
		int a;
		cin >> a;
	}
	glUseProgram(shaderID);
	//need to enable all the vertex attributes.
	glEnableVertexAttribArray(0); //vertexPosition
	glEnableVertexAttribArray(1); //vertexColor
	glEnableVertexAttribArray(2); //vertexNormal
	glEnableVertexAttribArray(3); //textureCoordinate
	glEnableVertexAttribArray(4); //textureType
}

void BlockShader::unBind()
{
	//need to disable all the vertex attributes. Otherwise, bugs occur when drawing other things on some graphics cards
	glDisableVertexAttribArray(0); //vertexPosition
	glDisableVertexAttribArray(1); //vertexColor
	glDisableVertexAttribArray(2); //vertexNormal
	glDisableVertexAttribArray(3); //textureCoordinate
	glDisableVertexAttribArray(4); //textureType
}

void GridShader::initialize(string dirPath)
{
	if (initialized) return;
	cout << "Loading gridShader\n";

	GLuint vID, fID;
	shaderID = LoadShaders((dirPath + "GridShading.vert").c_str(), (dirPath + "GridShading.frag").c_str(), vID, fID);
	glBindAttribLocation(shaderID, 0, "vertexPosition");
	glBindAttribLocation(shaderID, 1, "vertexColor");
	LinkShaders(shaderID, vID, fID);

	mvpID = GetUniform(shaderID, "MVP");

	initialized = 1;
}

void GridShader::bind()
{
	if (!initialized){
		error("SHADER BOUND BEFORE INITIALIZATION");
		int a;
		cin >> a;
	}
	glUseProgram(shaderID);
	//need to enable all the vertex attributes.
	glEnableVertexAttribArray(0); //vertexPosition
	glEnableVertexAttribArray(1); //vertexColor
}

void GridShader::unBind()
{
	//need to disable all the vertex attributes. Otherwise, bugs occur when drawing other things on some graphics cards
	glDisableVertexAttribArray(0); //vertexPosition
	glDisableVertexAttribArray(1); //vertexColor
}

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path, GLuint &VertexShaderID, GLuint &FragmentShaderID){
	int a;
	// Create the shaders
	VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else{
		printf("Vertex Shader did not open!\n");
		printf("Enter any key to exit...\n");
		cin >> a;
		exit(5553);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else{
		printf("Fragment Shader did not open!\n");
		printf("Enter any key to exit...\n");
		cin >> a;
		exit(5553);
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;



	// Compile Vertex Shader
	//printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);




	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	//fprintf(stdout, "\nLinking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	return ProgramID;
}

void LinkShaders(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID)
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

//gets a uniform from a shader
GLuint GetUniform(GLuint shader, const char * name)
{
	GLuint rval = glGetUniformLocation(shader, name);
	if (rval == GL_INVALID_INDEX){
		int a;
		printf("Uniform %s was not found in shader # %d.\n", name, shader);
		printf("Enter any key to exit...\n");
		cin >> a;
		exit(5233);
	}
	return rval;
}
