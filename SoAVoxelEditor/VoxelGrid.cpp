#include "VoxelGrid.h"
#include "Camera.h"
#include "RenderUtil.h"
#include "Shader.h"
#include "VoxelRenderer.h"


VoxelGrid::VoxelGrid(int width, int height, int length){
    voxels.resize(height*width*length);
    for (int i = 0; i < height*width*length; i++){
        voxels[i] = new Voxel;
        voxels[i]->type = '\0';
        voxels[i]->selected = 0;
    }
    h = height;
    w = width;
    l = length;
    layerSize = w * h;
    vTot = 0;
}

//i*layersize + j*width + k
//Y, Z, X basically‏

void VoxelGrid::addVoxel(Voxel* newV, int x, int y, int z){
    Voxel *tempV = getVoxel(x, y, z);
    BlockVertex tv;
    if (tempV == NULL){
        return;
    }
    if (tempV->type == '\0'){
        tempV->type = newV->type;
        tempV->selected = newV->selected;
        vTot++;

        for (int i = 0; i < 24; i++){
            tv = baseMesh.verts[i];
            tv.position.x += x;
            tv.position.y += y;
            tv.position.z += z;
            tv.selected = 0.0;
            currentVerts.push_back(tv);
        }
        changed = true;
    } else{
        printf("Voxel space <%d,%d,%d> is occupied.\n", x, y, z);
    }
}

void VoxelGrid::removeVoxel(int x, int y, int z){
    Voxel *tempV = getVoxel(x, y, z);
    if (tempV == NULL){
        return;
    }
    if (tempV->type == '\0'){
        printf("Nothing to remove at <%d,%d,%d>.\n", x, y, z);
    } else{
        tempV->type = '\0';
        tempV->selected = 0;
        vTot--;
        for (int i = 0; i < currentVerts.size(); i++){
            if (currentVerts[i].position.x - baseMesh.verts[i % 24].position.x == x && currentVerts[i].position.y - baseMesh.verts[i % 24].position.y == y && currentVerts[i].position.z - baseMesh.verts[i % 24].position.z == z){
             
                for (int j = 0; j < 24; j++){
                    currentVerts[i + j] = currentVerts[currentVerts.size() - 24 + j];
                }
                for (int j = 0; j < 24; j++){
                    currentVerts.pop_back();
                }
                break;
            }
        }
        changed = true;
    }
}

Voxel* VoxelGrid::getVoxel(int x, int y, int z){
   
    bool rangeCheck = false;

    if (x < 0 || x >= w){
        rangeCheck = true;
    }
    if (y < 0 || y >= h){
        rangeCheck = true;
    }
    if (z < 0 || z >= l){
        rangeCheck = true;
    }
    if (rangeCheck == true){
        return NULL;
    }

    return voxels[z*layerSize + y*w + x];
}

void VoxelGrid::drawGrid(Camera *camera) {

    //Opengl Resource for Opengl 3.3+ http://www.opengl-tutorial.org/

    //*************** here is some example draw code. This is temporary, and should not really be used. ****************
    gridShader.bind();

    glm::mat4 modelMatrix(1);
    //this is a fast way to set up the translation. This is equivalent to a translatef
    //We translate by the negative position of the camera. This causes the world to move around the camera, rather 
    //than the camera to move around the world.
    modelMatrix[3][0] = -camera->position.x;
    modelMatrix[3][1] = -camera->position.y;
    modelMatrix[3][2] = -camera->position.z;


    glm::mat4 MVP = camera->projectionMatrix * camera->viewMatrix * modelMatrix;

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
       
        sizeHolder = ((w + 1) + (l + 1)) * 2;
        verts = new GridVertex[sizeHolder];
        int alpha = 255;

        for (int j = 0; j < w + 1; j++, i += 2){
            verts[i].position.x = w - j;
            verts[i].position.y = 0;
           
            verts[i].position.z = 0;

            verts[i + 1].position.x = w - j;
            verts[i + 1].position.y = 0;
            verts[i + 1].position.z = l;

            verts[i].color[0] = 255;
            verts[i].color[1] = 0;
            verts[i].color[2] = 0;
            verts[i].color[3] = alpha;

            verts[i + 1].color[0] = 255;
            verts[i + 1].color[1] = 0;
            verts[i + 1].color[2] = 0;
            verts[i + 1].color[3] = alpha;
        }

        for (int k = 0; k < l + 1; k++, i += 2){
            verts[i].position.x = 0;
            verts[i].position.y = 0;
            verts[i].position.z = l - k;

            verts[i + 1].position.x = w;
            verts[i + 1].position.y = 0;
            verts[i + 1].position.z = l - k;

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
    if (vTot == 0){
        return;
    }

    VoxelRenderer::drawVoxels(camera);
}