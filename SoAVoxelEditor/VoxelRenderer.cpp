#include "VoxelRenderer.h"
#include "GlobalStructs.h"
#include "Shader.h"
#include "Camera.h"
#include "RenderUtil.h"
#include "TextureManager.h"
#include "Voxel.h"
#include "TextureManager.h"

std::map<long long, glm::vec4> VoxelRenderer::_voxels;
Mesh VoxelRenderer::_mesh;

int VoxelRenderer::_width;
int VoxelRenderer::_height;
int VoxelRenderer::_length;

void VoxelRenderer::initialize(int w, int h, int l) {
    _width = w;
    _height = h;
    _length = 1;

    std::vector<BlockVertex> vertices(24);
    std::vector<GLuint> indices(36);

    int j = 0;
    for (int i = 0; i < 36; i += 6){
        indices[i] = j;
        indices[i + 1] = j + 1;
        indices[i + 2] = j + 2;
        indices[i + 3] = j + 2;
        indices[i + 4] = j + 3;
        indices[i + 5] = j;
        j += 4;
    }

    for (int i = 0; i < 24; i++){
        vertices[i].position.x = cubeVertices[i * 3];
        vertices[i].position.y = cubeVertices[i * 3 + 1];
        vertices[i].position.z = cubeVertices[i * 3 + 2];

        vertices[i].normal.x = cubeNormals[i * 3];
        vertices[i].normal.y = cubeNormals[i * 3 + 1];
        vertices[i].normal.z = cubeNormals[i * 3 + 2];
    }

    _mesh = Mesh();

    RenderUtil::uploadMesh(&_mesh.vboID, &_mesh.iboID, &vertices[0], vertices.size(), &indices[0], indices.size());
    _mesh.numIndices = indices.size();
}

void VoxelRenderer::drawVoxels(Camera *camera) {
    blockShader.bind();

    const glm::vec3 &position = camera->getPosition();

    glm::mat4 modelMatrix(1);
    modelMatrix[3][0] = -position.x;
    modelMatrix[3][1] = -position.y;
    modelMatrix[3][2] = -position.z;

    glm::vec3 lightPos = position;
    lightPos = glm::normalize(lightPos);
    glUniform3f(blockShader.lightPosID, lightPos.x, lightPos.y, lightPos.z);

    glBindBuffer(GL_ARRAY_BUFFER, _mesh.vboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh.iboID);

    //set our attribute pointers using our interleaved vertex data. Last parameter is offset into the vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)0); //vertexPosition
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)12); //vertexNormal

    glm::vec3 voxelPosition;
    glm::mat4 vp = camera->getProjectionMatrix() * camera->getViewMatrix();
    glm::mat4 mvp(1);
    for(auto i = _voxels.begin(); i != _voxels.end(); i++) {
        voxelPosition = getCoord(i->first);

        modelMatrix[3][0] = -position.x + voxelPosition.x;
        modelMatrix[3][1] = -position.y + voxelPosition.y;
        modelMatrix[3][2] = -position.z + voxelPosition.z;

        mvp = vp * modelMatrix;

        glUniformMatrix4fv(blockShader.mvpID, 1, GL_FALSE, &mvp[0][0]);
        glUniform4f(blockShader.colorID, i->second.r, i->second.g, i->second.b, i->second.a);

        glDrawElements(GL_TRIANGLES, _mesh.numIndices, GL_UNSIGNED_INT, (void*)0);
    }

    blockShader.unBind();

    if (drawDebugLine){
        RenderUtil::drawLine(camera, debugP1, debugP2, 255, 0, 255, 5);
    }
}

void VoxelRenderer::addVoxel(int x, int y, int z, const GLubyte* color) {
    _voxels[getIndex(x, y, z)] = glm::vec4(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f, color[3] / 255.0f);
}

void VoxelRenderer::removeVoxel(int x, int y, int z) {
    long long index = getIndex(x, y, z);
    if(_voxels.find(index) != _voxels.end())
        _voxels.erase(index);
}

long long VoxelRenderer::getIndex(int x, int y, int z) {
    return x + y * _width + z * _width * _height;
}

glm::vec3 VoxelRenderer::getCoord(long long index) {
    GLuint x = index % _width;
    index /= _width;
    GLuint y = index % _height;
    index /= _height;
    GLuint z = index;
    return glm::vec3(x, y, z);
}