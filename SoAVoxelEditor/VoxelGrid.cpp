#include "VoxelGrid.h"
#include "Camera.h"
#include "RenderUtil.h"
#include "Shader.h"
#include "Voxel.h"
#include "VoxelRenderer.h"

#include <iostream>


VoxelGrid::VoxelGrid(int width, int height, int length):
_width(width),
_height(height),
_length(length),
_layerSize(width * height),
_size(width * height * length),
_voxelCount(0)
{
    _voxels = new Voxel[_size];
    for(int i = 0; i < _size; i++) {
        _voxels[i] = Voxel();
        _voxels[i].type = 0;
        _voxels[i].sideVisibility = 63;
    }
}

//i*layersize + j*width + k
//Y, Z, X basically‏

bool VoxelGrid::addVoxel(const Voxel& newV, int x, int y, int z){
    Voxel *tempV = getVoxel(x, y, z);
    
    if (tempV == nullptr){
        return false;
    }

    if (tempV->type == '\0'){
        _voxelCount++;
        tempV->type = newV.type;
        for(int i = 0; i < 4; i++)
            tempV->color[i] = newV.color[i];
        
        Voxel* tempVoxel = nullptr;

        VoxelRenderer::addVoxel(x, y, z, newV.color);

        setVoxelSideVisiblity(x + 1, y, z, Direction::WEST, false);
        if(!isVoxelVisible(x + 1, y, z))
            VoxelRenderer::removeVoxel(x + 1, y, z);

        setVoxelSideVisiblity(x, y + 1, z, Direction::BOTTOM, false);
        if(!isVoxelVisible(x, y + 1, z))
            VoxelRenderer::removeVoxel(x, y + 1, z);

        setVoxelSideVisiblity(x, y, z + 1, Direction::SOUTH, false);
        if(!isVoxelVisible(x, y, z + 1))
            VoxelRenderer::removeVoxel(x, y, z + 1);

        setVoxelSideVisiblity(x - 1, y, z, Direction::EAST, false);
        if(!isVoxelVisible(x - 1, y, z))
            VoxelRenderer::removeVoxel(x - 1, y, z);

        setVoxelSideVisiblity(x, y - 1, z, Direction::TOP, false);
        if(!isVoxelVisible(x, y - 1, z))
            VoxelRenderer::removeVoxel(x, y - 1, z);

        setVoxelSideVisiblity(x, y, z - 1, Direction::NORTH, false);
        if(!isVoxelVisible(x, y, z - 1))
            VoxelRenderer::removeVoxel(x, y, z - 1);

		return true;
    } else{
        //std::printf("Voxel space <%d,%d,%d> is occupied.\n", x, y, z);
		return false;
    }
}

bool VoxelGrid::removeVoxel(int x, int y, int z){
    Voxel *tempV = getVoxel(x, y, z);
    if (!tempV) return false;

    if (tempV->type == 0){
        //std::printf("Nothing to remove at <%d,%d,%d>.\n", x, y, z);
		return false;
    } else {
        _voxelCount--;
        tempV->type = 0;
        
        if(!isVoxelVisible(x, y, z))
            VoxelRenderer::removeVoxel(x, y, z);

        Voxel* tempVoxel = nullptr;
        setVoxelSideVisiblity(x + 1, y, z, Direction::WEST, true);
        if((tempVoxel = getVoxel(x + 1, y, z)) && tempVoxel->type != 0)
            VoxelRenderer::addVoxel(x + 1, y, z, tempVoxel->color);

        setVoxelSideVisiblity(x, y + 1, z, Direction::BOTTOM, true);
        if((tempVoxel = getVoxel(x, y + 1, z)) && tempVoxel->type != 0)
            VoxelRenderer::addVoxel(x, y + 1, z, tempVoxel->color);

        setVoxelSideVisiblity(x, y, z + 1, Direction::SOUTH, true);
        if((tempVoxel = getVoxel(x, y, z + 1)) && tempVoxel->type != 0)
            VoxelRenderer::addVoxel(x, y, z + 1, tempVoxel->color);

        setVoxelSideVisiblity(x - 1, y, z, Direction::EAST, true);
        if((tempVoxel = getVoxel(x - 1, y, z)) && tempVoxel->type != 0)
            VoxelRenderer::addVoxel(x - 1, y, z, tempVoxel->color);

        setVoxelSideVisiblity(x, y - 1, z, Direction::TOP, true);
        if((tempVoxel = getVoxel(x, y - 1, z)) && tempVoxel->type != 0)
            VoxelRenderer::addVoxel(x, y - 1, z, tempVoxel->color);

        setVoxelSideVisiblity(x, y, z - 1, Direction::NORTH, true);
        if((tempVoxel = getVoxel(x, y, z - 1)) && tempVoxel->type != 0)
            VoxelRenderer::addVoxel(x, y, z - 1, tempVoxel->color);
    }
	return true;
}

Voxel* VoxelGrid::getVoxel(int x, int y, int z){
    if (x < 0 || x >= _width) return nullptr;
    if (y < 0 || y >= _height) return nullptr;
    if (z < 0 || z >= _length) return nullptr;

    return &_voxels[z*_layerSize + y*_width + x];
}

void VoxelGrid::setVoxelSideVisiblity(const int x, const int y, const int z, const Direction side, const bool visible) {
    if(x < 0 || x >= _width || y < 0 || y >= _height || z < 0 || z >= _length) return;

    int index = z*_layerSize + y*_width + x;
    
    bool wasVisible = isVoxelSideVisibleNoBoundsCheck(x, y, z, side);
    if(wasVisible && visible)
        return;
    if(!wasVisible && !visible)
        return;
    if(wasVisible && !visible) {
        _voxels[index].sideVisibility &= ~(1 << side);
        return;
    }
    if(!wasVisible && visible) {
        _voxels[index].sideVisibility |= (1 << side);
        return;
    }
}

bool VoxelGrid::isVoxelSideVisible(const int x, const int y, const int z, const Direction side) const {
    if(x < 0 || x >= _width || y < 0 || y >= _height || z < 0 || z >= _length) return false;

    return ((_voxels[z*_layerSize + y*_width + x].sideVisibility & (1 << side)) >> side) == 1;
}

inline bool VoxelGrid::isVoxelSideVisibleNoBoundsCheck(const int x, const int y, const int z, const Direction side) const {
    return ((_voxels[z*_layerSize + y*_width + x].sideVisibility & (1 << side)) >> side) == 1;
}

bool VoxelGrid::isVoxelVisible(const int x, const int y, const int z) {
    if(x < 0 || x >= _width || y < 0 || y >= _height || z < 0 || z >= _length) return false;

    return (bool)_voxels[z*_layerSize + y*_width + x].sideVisibility;
}

void VoxelGrid::drawGrid(Camera *camera) {

    //Opengl Resource for Opengl 3.3+ http://www.opengl-tutorial.org/

    //*************** here is some example draw code. This is temporary, and should not really be used. ****************
    gridShader.bind();

    const glm::vec3 &position = camera->getPosition();

    glm::mat4 modelMatrix(1);
    //this is a fast way to set up the translation. This is equivalent to a translatef
    //We translate by the negative position of the camera. This causes the world to move around the camera, rather 
    //than the camera to move around the world.
    modelMatrix[3][0] = -position.x;
    modelMatrix[3][1] = -position.y;
    modelMatrix[3][2] = -position.z;


    glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

    //send our uniform data, the matrix, the light position, and the texture data
    glUniformMatrix4fv(gridShader.mvpID, 1, GL_FALSE, &MVP[0][0]);

    //Static variables, so they are intitialized onces and remain for the life of the program.
    //In reality, your meshes should be stored in a class somewhere, but this is just an example
    static GLuint vboID = 0;
    static int sizeHolder = 0;
    //initialize the buffer, only happens once
    if (vboID == 0){
        //generate a buffer object for the vboID. after this call, vboID will be a number > 0
        glGenBuffers(1, &vboID);

        //Generate the cube's vertex data

        int i = 0;
       
        sizeHolder = ((_width + 1) + (_length + 1)) * 2 + 16;
        std::vector<GridVertex> verts(sizeHolder);
        int alpha = 255;

        for (int j = 0; j < _width + 1; j++, i += 2){
            verts[i].position.x = _width - j;
            verts[i].position.y = 0;
            verts[i].position.z = 0;

            verts[i + 1].position.x = _width - j;
            verts[i + 1].position.y = 0;
            verts[i + 1].position.z = _length;

            verts[i].color[0] = 255;
            verts[i].color[1] = 0;
            verts[i].color[2] = 0;
            verts[i].color[3] = alpha;

            verts[i + 1].color[0] = 255;
            verts[i + 1].color[1] = 0;
            verts[i + 1].color[2] = 0;
            verts[i + 1].color[3] = alpha;
        }

        for (int k = 0; k < _length + 1; k++, i += 2){
            verts[i].position.x = 0;
            verts[i].position.y = 0;
            verts[i].position.z = _length - k;

            verts[i + 1].position.x = _width;
            verts[i + 1].position.y = 0;
            verts[i + 1].position.z = _length - k;

            verts[i].color[0] = 255;
            verts[i].color[1] = 0;
            verts[i].color[2] = 0;
            verts[i].color[3] = alpha;

            verts[i + 1].color[0] = 255;
            verts[i + 1].color[1] = 0;
            verts[i + 1].color[2] = 0;
            verts[i + 1].color[3] = alpha;
        }

        verts[i].position.x = 0;
        verts[i].position.y = 0;
        verts[i].position.z = 0;

        verts[i + 1].position.x = 0;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = 0;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;

        verts[i].position.x = _width;
        verts[i].position.y = 0;
        verts[i].position.z = 0;

        verts[i + 1].position.x = _width;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = 0;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;

        verts[i].position.x = _width;
        verts[i].position.y = 0;
        verts[i].position.z = _length;

        verts[i + 1].position.x = _width;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = _length;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;
        verts[i].position.x = 0;
        verts[i].position.y = 0;
        verts[i].position.z = _length;

        verts[i + 1].position.x = 0;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = _length;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;
        verts[i].position.x = 0;
        verts[i].position.y = _height;
        verts[i].position.z = 0;

        verts[i + 1].position.x = _width;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = 0;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;
        verts[i].position.x = _width;
        verts[i].position.y = _height;
        verts[i].position.z = 0;

        verts[i + 1].position.x = _width;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = _length;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;
        verts[i].position.x = _width;
        verts[i].position.y = _height;
        verts[i].position.z = _length;

        verts[i + 1].position.x = 0;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = _length;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;
        verts[i].position.x = 0;
        verts[i].position.y = _height;
        verts[i].position.z = _length;

        verts[i + 1].position.x = 0;
        verts[i + 1].position.y = _height;
        verts[i + 1].position.z = 0;

        verts[i].color[0] = 255;
        verts[i].color[1] = 0;
        verts[i].color[2] = 0;
        verts[i].color[3] = alpha;

        verts[i + 1].color[0] = 255;
        verts[i + 1].color[1] = 0;
        verts[i + 1].color[2] = 0;
        verts[i + 1].color[3] = alpha;

        i += 2;
        //bind the buffers into the correct slots
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        //this call is optional, but it makes it faster (not in this case) because it orphans any previous buffer. opengl-tutorial has details
        //fill the buffer with our vertex data. This is basically a memcpy. Static draw means we change the buffer once and draw many times
        glBufferData(GL_ARRAY_BUFFER, sizeHolder * sizeof(GridVertex), verts.data(), GL_STATIC_DRAW);
    } else{ //we already initialized the buffers on another frame
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //set our attribute pointers using our interleaved vertex data. Last parameter is offset into the vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void *)0); //vertexPosition
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GridVertex), (void *)12); //vertexColor


    //Finally, draw our data. The last parameter is the offset into the bound buffer
    glLineWidth(1);
    glDrawArrays(GL_LINES, 0, sizeHolder);

    gridShader.unBind();

}

void VoxelGrid::drawVoxels(Camera *camera) {
    if (_voxelCount == 0){
        return;
    }

    VoxelRenderer::drawVoxels(camera);
}