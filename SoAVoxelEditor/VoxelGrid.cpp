#include "VoxelGrid.h"
#include "Camera.h"
#include "RenderUtil.h"


VoxelGrid::VoxelGrid(int width, int height, int length){
    voxels.resize(height*width*length);
    for (int i = 0; i < height*width*length; i++){
        voxels[i] = new voxel;
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

void VoxelGrid::addVoxel(voxel* newV, int x, int y, int z){
    voxel *tempV = getVoxel(x, y, z);
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
        //printf("Voxel at <%d,%d,%d> added.\n", x, y, z);
    } else{
        printf("Voxel space <%d,%d,%d> is occupied.\n", x, y, z);
    }
}

void VoxelGrid::removeVoxel(int x, int y, int z){
    voxel *tempV = getVoxel(x, y, z);
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
                //if (currentVerts[i].offset.x == x && currentVerts[i].offset.y == y && currentVerts[i].offset.z == z){
                for (int j = 0; j < 24; j++){
                    currentVerts[i + j] = currentVerts[currentVerts.size() - 24 + j];
                }
                for (int j = 0; j < 24; j++){
                    currentVerts.pop_back();
                }
                //currentVerts.erase(currentVerts.begin() + i, currentVerts.begin() + i + 24);
                break;
            }
        }
        changed = true;
        //printf("Voxel at <%d,%d,%d> removed.\n", x, y, z);
    }
}

voxel* VoxelGrid::getVoxel(int x, int y, int z){
    //printf("getVoxel coordinates <%d,%d,%d>\n", x, y, z);
    bool rangeCheck = false;

    if (x < 0 || x >= w){
        //printf("Value: %d for x is out of range\n", x);
        rangeCheck = true;
    }
    if (y < 0 || y >= h){
        //printf("Value: %d for y is out of range\n", y);
        rangeCheck = true;
    }
    if (z < 0 || z >= l){
        //printf("Value: %d for z is out of range\n", z);
        rangeCheck = true;
    }
    if (rangeCheck == true){
        return NULL;
    }

    return voxels[z*layerSize + y*w + x];
}

void VoxelGrid::drawGrid(Camera *camera) {
    //glClearDepth(1.0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Opengl Resource for Opengl 3.3+ http://www.opengl-tutorial.org/

    //*************** here is some example draw code. This is temporary, and should not really be used. ****************
    gridShader.bind();

    glm::mat4 modelMatrix(1);
    //this is a fast way to set up the translation. This is equivalent to a translatef
    //We translate by the negative position of the camera. This causes the world to move around the camera, rather 
    //than the camera to move around the world.
    modelMatrix[3][0] = -mainCamera->position.x;
    modelMatrix[3][1] = -mainCamera->position.y;
    modelMatrix[3][2] = -mainCamera->position.z;


    glm::mat4 MVP = mainCamera->projectionMatrix * mainCamera->viewMatrix * modelMatrix;

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
        int i = 0, w, l;
        w = gameGrid->w;
        l = gameGrid->l;
        sizeHolder = ((w + 1) + (l + 1)) * 2;
        verts = new GridVertex[sizeHolder];
        int alpha = 255;

        for (int j = 0; j < w + 1; j++, i += 2){
            verts[i].position.x = w - j;
            verts[i].position.y = 0;
            //verts[i].position.z = -(l / 2);
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
        //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), NULL);
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
    if (voxelGrid->vTot == 0){
        return;
    }

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

    //glm::mat4 MVP = mainCamera->projectionMatrix * mainCamera->viewMatrix * modelMatrix;
    glm::mat4 MVP = camera->projectionMatrix * camera->viewMatrix * modelMatrix;

    //send our uniform data, the matrix, the light position, and the texture data
    glUniformMatrix4fv(blockShader.mvpID, 1, GL_FALSE, &MVP[0][0]);
    //glm::vec3 lightPos(0.1f, 0.5f, 0.8f);
    glm::vec3 lightPos = camera->position;
    lightPos = glm::normalize(lightPos);
    glUniform3f(blockShader.lightPosID, lightPos.x, lightPos.y, lightPos.z);

    //t key toggles between selected/not selected texture
    //if (Keys[SDLK_t].pr == 0){



    //this shouldn't still work!!!!
    glBindTexture(GL_TEXTURE_2D, cubeTexts['b']->data);
    RenderUtil::checkGlError();
    glUniform1i(blockShader.textPosID, 0);
    glBindTexture(GL_TEXTURE_2D, cubeSelectedTexts['b']->data);
    RenderUtil::checkGlError();
    glUniform1i(blockShader.textSelPosID, 1);



    /*if (gameGrid->getVoxel(0,0,0) != NULL){
    if (gameGrid->getVoxel(0,0,0)->selected == false){
    glBindTexture(GL_TEXTURE_2D, cubeTexts['b']->data);
    checkGlError();
    glUniform1i(blockShader.textPosID, 0);
    }
    else{
    glBindTexture(GL_TEXTURE_2D, cubeSelectedTexts['b']->data);
    checkGlError();
    glUniform1i(blockShader.textPosID, 0);
    }
    }
    else{
    glBindTexture(GL_TEXTURE_2D, cubeTexts['b']->data);
    checkGlError();
    glUniform1i(blockShader.textPosID, 0);
    }*/

    //glUniform1f(blockShader.selected, 1);

    //Static variables, so they are intitialized onces and remain for the life of the program.
    //In reality, your meshes should be stored in a class somewhere, but this is just an example
    static GLuint vboID = 0;
    static GLuint elementsID = 0;

    //initialize the buffer, only happens once
    //if (vboID == 0){
    if (changed == 1){
        //generate a buffer object for the vboID. after this call, vboID will be a number > 0
        glGenBuffers(1, &vboID);
        //generate buffer object for the indices
        glGenBuffers(1, &elementsID);


        //Generate the cube's vertex data
        BlockVertex verts[24];
        for (int i = 0; i < 24; i++){
            verts[i].position.x = cubeVertices[i * 3];
            verts[i].position.y = cubeVertices[i * 3 + 1];
            verts[i].position.z = cubeVertices[i * 3 + 2];

            verts[i].normal.x = cubeNormals[i * 3];
            verts[i].normal.y = cubeNormals[i * 3 + 1];
            verts[i].normal.z = cubeNormals[i * 3 + 2];

            verts[i].color[0] = 0;
            verts[i].color[1] = 255;
            verts[i].color[2] = 0;
            verts[i].color[3] = 255;

            verts[i].text.x = cubeTextCoords[i * 2];
            verts[i].text.y = cubeTextCoords[i * 2 + 1];
            verts[i].selected = 0.0;
        }
        //
        //the indexes for drawing the cube, it just follows the pattern (i, i+1, i+2, i+2, i+3, i) for i += 4
        GLuint drawIndices[36] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };
        //GLuint drawIndices[72] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20, 24, 25, 26, 26, 27, 24, 28, 29, 30, 30, 31, 28, 32, 33, 34, 34, 35, 32, 36, 37, 38, 38, 39, 36, 40, 41, 42, 42, 43, 40, 44, 45, 46, 46, 47, 44 };
        //bind the buffers into the correct slots
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        //this call is optional, but it makes it faster (not in this case) because it orphans any previous buffer. opengl-tutorial has details
        //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), NULL);
        //fill the buffer with our vertex data. This is basically a memcpy. Static draw means we change the buffer once and draw many times
        //glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, currentVerts.size() * sizeof(BlockVertex), &currentVerts[0], GL_STATIC_DRAW);

        //now do the same thing for the elements
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
        //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(drawIndices), NULL);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(drawIndices), drawIndices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * gameGrid->vTot * sizeof(GLuint), currentIndices, GL_STATIC_DRAW);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentIndices.size() * sizeof(GLuint), &currentIndices[0], GL_STATIC_DRAW);
        changed = false;
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
    //glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)48); //textureType

    //Finally, draw our data. The last parameter is the offset into the bound buffer
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    //glDrawElements(GL_TRIANGLES, currentIndices.size(), GL_UNSIGNED_INT, NULL);
    glDrawElements(GL_TRIANGLES, 36 * gameGrid->h * gameGrid->l * gameGrid->w, GL_UNSIGNED_INT, NULL);

    blockShader.unBind();

    if (drawDebugLine){
        RenderUtil::drawLine(mainCamera, debugP1, debugP2, 255, 0, 255, 5);
    }

    //SDL_GL_SwapWindow(mainWindow);
}