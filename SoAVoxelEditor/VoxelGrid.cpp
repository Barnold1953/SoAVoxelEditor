#include "VoxelGrid.h"
#include "Camera.h"
#include "RenderUtil.h"
#include "Shader.h"
#include "Voxel.h"
#include "VoxelRenderer.h"


VoxelGrid::VoxelGrid(int width, int height, int length){
    _voxels.resize(height*width*length);
    for (int i = 0; i < height*width*length; i++){
        _voxels[i] = new Voxel;
        _voxels[i]->type = '\0';
        _voxels[i]->selected = 0;
    }
    _height = height;
    _width = width;
    _length = length;
    _layerSize = _width * _height;
    _vTot = 0;
}

//i*layersize + j*width + k
//Y, Z, X basically‏

bool VoxelGrid::addVoxel(Voxel* newV, int x, int y, int z){
    Voxel *tempV = getVoxel(x, y, z);
    
    if (tempV == NULL){
        return 0;
    }

   
    if (tempV->type == '\0'){
        _vTot++;
        tempV->type = newV->type;
        tempV->selected = newV->selected;
        VoxelRenderer::addVoxel(x, y, z);
		return 1;
    } else{
        printf("Voxel space <%d,%d,%d> is occupied.\n", x, y, z);
		return 0;
    }
}

bool VoxelGrid::removeVoxel(int x, int y, int z){
    Voxel *tempV = getVoxel(x, y, z);
    if (tempV == NULL){
        return 0;
    }
    if (tempV->type == '\0'){
        printf("Nothing to remove at <%d,%d,%d>.\n", x, y, z);
		return 0;
    } else{
        _vTot--;
        tempV->type = '\0';
        tempV->selected = 0;
        
        VoxelRenderer::removeVoxel(x, y, z);
    }
	return 1;
}

Voxel* VoxelGrid::getVoxel(int x, int y, int z){
   
    bool rangeCheck = false;

    if (x < 0 || x >= _width){
        rangeCheck = true;
    }
    if (y < 0 || y >= _height){
        rangeCheck = true;
    }
    if (z < 0 || z >= _length){
        rangeCheck = true;
    }
    if (rangeCheck == true){
        return NULL;
    }

    return _voxels[z*_layerSize + y*_width + x];
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

        GridVertex* verts;
        int i = 0;
       
        sizeHolder = ((_width + 1) + (_length + 1)) * 2;
        verts = new GridVertex[sizeHolder];
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

        //bind the buffers into the correct slots
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        //this call is optional, but it makes it faster (not in this case) because it orphans any previous buffer. opengl-tutorial has details
        //fill the buffer with our vertex data. This is basically a memcpy. Static draw means we change the buffer once and draw many times
        glBufferData(GL_ARRAY_BUFFER, sizeHolder * sizeof(GridVertex), verts, GL_STATIC_DRAW);
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
    if (_vTot == 0){
        return;
    }

    VoxelRenderer::drawVoxels(camera);
}