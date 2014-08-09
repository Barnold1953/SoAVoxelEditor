#include "RenderUtil.h"
#include "Camera.h"
#include "Shader.h"
#include "GlobalStructs.h"
#include "Errors.h"

Mesh* RenderUtil::_mesh = nullptr;
Mesh* RenderUtil::_referenceCubeMesh = nullptr;
GLuint* RenderUtil::_referenceCubeIndices = new GLuint[36];
glm::vec3 RenderUtil::_lastPosition = glm::vec3(-1, -1, -1);
BlockMesh RenderUtil::_voxVerts = BlockMesh();
BlockMesh RenderUtil::_voxBaseVerts = BlockMesh();

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

void RenderUtil::initializeReferenceVoxel(){
	_referenceCubeIndices = new GLuint[36];
	
	for (int i = 0, j = 0; i < 36; i += 6, j += 4){
		_referenceCubeIndices[i] = j;
		_referenceCubeIndices[i + 1] = j + 1;
		_referenceCubeIndices[i + 2] = j + 2;
		_referenceCubeIndices[i + 3] = j + 2;
		_referenceCubeIndices[i + 4] = j + 3;
		_referenceCubeIndices[i + 5] = j;
	}

	for (int i = 0; i < 24; i++){
		_voxBaseVerts.verts[i].position.x = cubeVertices[i * 3];
		_voxBaseVerts.verts[i].position.y = cubeVertices[i * 3 + 1];
		_voxBaseVerts.verts[i].position.z = cubeVertices[i * 3 + 2];

		_voxBaseVerts.verts[i].normal.x = cubeNormals[i * 3];
		_voxBaseVerts.verts[i].normal.y = cubeNormals[i * 3 + 1];
		_voxBaseVerts.verts[i].normal.z = cubeNormals[i * 3 + 2];

		_voxBaseVerts.verts[i].color[0] = 255;
		_voxBaseVerts.verts[i].color[1] = 255;
		_voxBaseVerts.verts[i].color[2] = 255;
		_voxBaseVerts.verts[i].color[3] = 100;
	}

	_referenceCubeMesh = new Mesh();
	_referenceCubeMesh->iboID = 0;
	_referenceCubeMesh->vboID = 0;

	_lastPosition = glm::vec3(-1, -1, -1);
}

void RenderUtil::drawReferenceVoxel(class Camera* camera, const glm::vec3 position){
	if (!_referenceCubeMesh) initializeReferenceVoxel();

	blockShader.bind();

	glm::mat4 modelMatrix(1);
	modelMatrix[3][0] = -camera->getPosition().x;
	modelMatrix[3][1] = -camera->getPosition().y;
	modelMatrix[3][2] = -camera->getPosition().z;

	glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

	//send our uniform data, the matrix, the light position, and the texture data
	glUniformMatrix4fv(blockShader.mvpID, 1, GL_FALSE, &MVP[0][0]);

	glm::vec3 lightPos = position;
	lightPos = glm::normalize(lightPos);
	glUniform3f(blockShader.lightPosID, lightPos.x, lightPos.y, lightPos.z);

	if ((int)position.x != (int)_lastPosition.x || (int)position.y != (int)_lastPosition.y || (int)position.z != (int)_lastPosition.z){
		for (int i = 0; i < 24; i++){
			_voxVerts.verts[i] = _voxBaseVerts.verts[i];
			_voxVerts.verts[i].position.x += position.x;
			_voxVerts.verts[i].position.y += position.y;
			_voxVerts.verts[i].position.z += position.z;
		}
		_lastPosition = position;
		RenderUtil::uploadMesh(&_referenceCubeMesh->vboID, &_referenceCubeMesh->iboID, &_voxVerts.verts[0], 24, _referenceCubeIndices, 36);
	}
	else{
		glBindBuffer(GL_ARRAY_BUFFER, _referenceCubeMesh->vboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _referenceCubeMesh->iboID);
	}

	//initialize the buffer, only happens once
	
	//set our attribute pointers using our interleaved vertex data. Last parameter is offset into the vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)0); //vertexPosition
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(BlockVertex), (void *)12); //vertexColor
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)16); //vertexNormal

	//Finally, draw our data. The last parameter is the offset into the bound buffer
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	blockShader.unBind();
}