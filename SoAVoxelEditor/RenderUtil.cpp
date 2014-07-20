#include "RenderUtil.h"
#include "Camera.h"
#include "Shader.h"
#include "GlobalStructs.h"
#include "Errors.h"

Mesh* RenderUtil::_mesh = nullptr;

bool RenderUtil::checkGlError(){
    GLenum err = glGetError();
    if (err != GL_NO_ERROR){
        switch (err){
        case GL_OUT_OF_MEMORY:
            error("Out of memory! Try lowering the voxel view distance.");
            return 1;
        case GL_INVALID_ENUM:
            error("GL_INVALID_ENUM - An unacceptable value is specified for an enumerated argument.");
            return 0;
        case GL_INVALID_VALUE:
            error("GL_INVALID_VALUE - A numeric argument is out of range.");
            return 0;
        case GL_INVALID_OPERATION:
            error("GL_INVALID_OPERATION - The specified operation is not allowed in the current state.");
            return 0;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error("The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete.");
            return 0;
        default:
            error(("OpenGL ERROR (" + to_string(err) + string(") ")).c_str());
            return 0;
        }
    }
    return 0;
}

void RenderUtil::drawLine(Camera *camera, glm::vec3 p1, glm::vec3 p2, GLubyte r, GLubyte g, GLubyte b, int thickness)
{
    GridVertex verts[2];

    gridShader.bind();

    glm::mat4 modelMatrix(1);
   
    const glm::vec3 &position = camera->getPosition();

    modelMatrix[3][0] = -position.x;
    modelMatrix[3][1] = -position.y;
    modelMatrix[3][2] = -position.z;
 

    glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

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

void RenderUtil::uploadMesh(GLuint* vboID, GLuint* iboID, const BlockVertex* blockVertices, const int numVertices, const GLuint* indices, const int numIndices) {
    //generate a buffer object for the vboID. after this call, vboID will be a number > 0
    glGenBuffers(1, vboID);
    //generate buffer object for the indices
    glGenBuffers(1, iboID);

    //bind the buffers into the correct slots
    glBindBuffer(GL_ARRAY_BUFFER, *vboID);

    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(BlockVertex), blockVertices, GL_STATIC_DRAW);

    //now do the same thing for the elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboID);

    //Don't need vTot, just use the ratio of indices to verts, which is 6/4
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void RenderUtil::uploadMesh(GLuint* vboID, GLuint* iboID, const glm::vec3* vertices, const int numVertices, const GLuint* indices, const int numIndices) {
    //generate a buffer object for the vboID. after this call, vboID will be a number > 0
    glGenBuffers(1, vboID);
    //generate buffer object for the indices
    glGenBuffers(1, iboID);

    //bind the buffers into the correct slots
    glBindBuffer(GL_ARRAY_BUFFER, *vboID);

    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

    //now do the same thing for the elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboID);

    //Don't need vTot, just use the ratio of indices to verts, which is 6/4
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void RenderUtil::initializeWireframeBox() {
    _mesh = new Mesh;
    const static int numVertices = 8;
    const static int numIndices = 24;
    const static glm::vec3 vertices[numVertices] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),

        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 1.0f),
    };
    const static GLuint indices[numIndices] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        0, 4,
        1, 5,
        2, 6,
        3, 7,
        4, 5,
        5, 6,
        6, 7,
        7, 4
    };

    uploadMesh(&_mesh->vboID, &_mesh->iboID, vertices, numVertices, indices, numIndices);
    _mesh->numIndices = numIndices;
}

void RenderUtil::drawWireframeBox(class Camera* camera, const glm::vec3& position, const glm::vec3& size, const glm::vec4& color) {
    if(!_mesh) initializeWireframeBox();

    wireframeShader.bind();

    glBindBuffer(GL_ARRAY_BUFFER, _mesh->vboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->iboID);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    const glm::vec3 &cameraPosition = camera->getPosition();

    glm::mat4 modelMatrix(1);
    modelMatrix[3][0] = -cameraPosition.x + position.x;
    modelMatrix[3][1] = -cameraPosition.y + position.y;
    modelMatrix[3][2] = -cameraPosition.z + position.z;
    modelMatrix[0][0] = size.x;
    modelMatrix[1][1] = size.y;
    modelMatrix[2][2] = size.z;
    glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

    glUniformMatrix4fv(blockShader.mvpID, 1, GL_FALSE, &MVP[0][0]);
    glUniform4f(wireframeShader.colorID, color.r, color.g, color.b, color.a);

    glLineWidth(2);

    glDrawElements(GL_LINES, _mesh->numIndices, GL_UNSIGNED_INT, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    wireframeShader.unBind();
}

void RenderUtil::releaseWireframeBox() {
    if(_mesh) {
        glDeleteBuffers(1, &_mesh->vboID);
        glDeleteBuffers(1, &_mesh->iboID);
        delete _mesh;
    }
}