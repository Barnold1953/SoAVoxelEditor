#include "RenderUtil.h"
#include "Camera.h"
#include "Shader.h"
#include "GlobalStructs.h"

RenderUtil::RenderUtil()
{
}


RenderUtil::~RenderUtil()
{
}


void RenderUtil::drawLine(Camera *camera, glm::vec3 p1, glm::vec3 p2, GLubyte r, GLubyte g, GLubyte b, int thickness)
{
    GridVertex verts[2];

    gridShader.bind();

    glm::mat4 modelMatrix(1);
   
    modelMatrix[3][0] = -camera->position.x;
    modelMatrix[3][1] = -camera->position.y;
    modelMatrix[3][2] = -camera->position.z;
 

    glm::mat4 MVP = camera->projectionMatrix * camera->viewMatrix * modelMatrix;

    glUniformMatrix4fv(gridShader.mvpID, 1, GL_FALSE, &MVP[0][0]);

    static GLuint vboID = 0;

    if (vboID == 0){
        glGenBuffers(1, &vboID);
    }

    verts[0].position = p1;
    verts[0].color[0] = r;
    verts[0].color[1] = g;
    verts[0].color[2] = b;
    verts[0].color[3] = 255;
    verts[1].position = p2;
    verts[1].color[0] = r;
    verts[1].color[1] = g;
    verts[1].color[2] = b;
    verts[1].color[3] = 255;

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
   
    // orphan the buffer for speed
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), NULL, GL_STREAM_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void *)0); //vertexPosition
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GridVertex), (void *)12); //vertexColor

    glLineWidth(thickness);

    //when drawing lines theres no bonus for using indices so we just use draw arrays
    //so we unbind the element array buffer for good measure
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDrawArrays(GL_LINES, 0, 2);

    gridShader.unBind();

    glLineWidth(1);
}