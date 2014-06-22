#include "VoxelRenderer.h"
#include "GlobalStructs.h"
#include "Shader.h"
#include "Camera.h"
#include "RenderUtil.h"
#include "TextureManager.h"

bool VoxelRenderer::_changed;
vector <BlockVertex> VoxelRenderer::_currentVerts;
GLuint *VoxelRenderer::_currentIndices;
BlockMesh VoxelRenderer::_baseMesh;

void VoxelRenderer::initialize(int w, int h, int l) {
    _changed = true;

    int cubeTot = w*h*l;

    _currentVerts.reserve(24 * cubeTot);
    _currentIndices = new GLuint[cubeTot * 36];

    int j = 0;
    for (int i = 0; i < w*h*l * 36; i += 6){
        _currentIndices[i] = j;
        _currentIndices[i + 1] = j + 1;
        _currentIndices[i + 2] = j + 2;
        _currentIndices[i + 3] = j + 2;
        _currentIndices[i + 4] = j + 3;
        _currentIndices[i + 5] = j;
        j += 4;
    }

    for (int i = 0; i < 24; i++){
        _baseMesh.verts[i].position.x = cubeVertices[i * 3];
        _baseMesh.verts[i].position.y = cubeVertices[i * 3 + 1];
        _baseMesh.verts[i].position.z = cubeVertices[i * 3 + 2];

        _baseMesh.verts[i].normal.x = cubeNormals[i * 3];
        _baseMesh.verts[i].normal.y = cubeNormals[i * 3 + 1];
        _baseMesh.verts[i].normal.z = cubeNormals[i * 3 + 2];

        _baseMesh.verts[i].color[0] = 255;
        _baseMesh.verts[i].color[1] = 0;
        _baseMesh.verts[i].color[2] = 0;
        _baseMesh.verts[i].color[3] = 255;

        _baseMesh.verts[i].text.x = cubeTextCoords[i * 2];
        _baseMesh.verts[i].text.y = cubeTextCoords[i * 2 + 1];

        _baseMesh.verts[i].selected = 0.0;
    }

}

void VoxelRenderer::drawVoxels(Camera *camera) {

    //Opengl Resource for Opengl 3.3+ http://www.opengl-tutorial.org/

    //*************** here is some example draw code. This is temporary, and should not really be used. ****************
    blockShader.bind();

    glm::mat4 modelMatrix(1);
    //this is a fast way to set up the translation. This is equivalent to a translatef
    //We translate by the negative position of the camera. This causes the world to move around the camera, rather 
    //than the camera to move around the world.
    modelMatrix[3][0] = -camera->position.x;
    modelMatrix[3][1] = -camera->position.y;
    modelMatrix[3][2] = -camera->position.z;

    glm::mat4 MVP = camera->projectionMatrix * camera->viewMatrix * modelMatrix;

    //send our uniform data, the matrix, the light position, and the texture data
    glUniformMatrix4fv(blockShader.mvpID, 1, GL_FALSE, &MVP[0][0]);

    glm::vec3 lightPos = camera->position;
    lightPos = glm::normalize(lightPos);
    glUniform3f(blockShader.lightPosID, lightPos.x, lightPos.y, lightPos.z);


    //this shouldn't still work!!!!
    glBindTexture(GL_TEXTURE_2D, TextureManager::getCubeTexture('b')->data);
    RenderUtil::checkGlError();
    glUniform1i(blockShader.textPosID, 0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::getSelectedTexture('b')->data);
    RenderUtil::checkGlError();
    glUniform1i(blockShader.textSelPosID, 1);

    //Static variables, so they are intitialized onces and remain for the life of the program.
    //In reality, your meshes should be stored in a class somewhere, but this is just an example
    static GLuint vboID = 0;
    static GLuint elementsID = 0;

    //initialize the buffer, only happens once
    if (_changed == true){
        //generate a buffer object for the vboID. after this call, vboID will be a number > 0
        glGenBuffers(1, &vboID);
        //generate buffer object for the indices
        glGenBuffers(1, &elementsID);

        //bind the buffers into the correct slots
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        glBufferData(GL_ARRAY_BUFFER, _currentVerts.size() * sizeof(BlockVertex), &_currentVerts[0], GL_STATIC_DRAW);

        //now do the same thing for the elements
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);

        //Don't need vTot, just use the ratio of indices to verts, which is 6/4
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 / 4 * _currentVerts.size() * sizeof(GLuint), _currentIndices, GL_STATIC_DRAW);

        _changed = false;
    } else{ //we already initialized the buffers on another frame
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
    }

    //set our attribute pointers using our interleaved vertex data. Last parameter is offset into the vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)0); //vertexPosition
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(BlockVertex), (void *)12); //vertexColor
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)16); //vertexNormal
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)28); //textureCoordinates
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)36); //textureType

    //Finally, draw our data. The last parameter is the offset into the bound buffer
    glDrawElements(GL_TRIANGLES, 6 / 4 * _currentVerts.size(), GL_UNSIGNED_INT, NULL);

    blockShader.unBind();

    if (drawDebugLine){
        RenderUtil::drawLine(camera, debugP1, debugP2, 255, 0, 255, 5);
    }
}