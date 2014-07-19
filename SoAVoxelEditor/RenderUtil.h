#pragma once
#include <Windows.h>
#include <GL\glew.h>
#include <glm\glm.hpp>

#include "GlobalStructs.h"

// Static class that provides various helpful rendering utilities
class RenderUtil
{
public:

    static bool checkGlError();
    static void drawLine(class Camera *camera, glm::vec3 p1, glm::vec3 p2, GLubyte r, GLubyte g, GLubyte b, int thickness);
    static void uploadMesh(GLuint* vboID, GLuint* iboID, const BlockVertex* blockVertices, const int numVertices, const GLuint* indices, const int numIndices);
};

