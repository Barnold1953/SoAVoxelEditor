#pragma once
#include <Windows.h>
#include <GL\glew.h>
#include <glm\glm.hpp>

// Static class that provides various helpful rendering utilities
class RenderUtil
{
public:
    RenderUtil();
    ~RenderUtil();

    static void drawLine(class Camera *camera, glm::vec3 p1, glm::vec3 p2, GLubyte r, GLubyte g, GLubyte b, int thickness);
};

