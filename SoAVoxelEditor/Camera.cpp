#include "Camera.h"
//
// Camera.h
//
// Copyright 2014 Seed Of Andromeda
//

#include "Options.h"
#include "GlobalStructs.h"
#include <SDL.h>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>

//the way I am initializing the position and angles here is stupid. I calculated it by hand and wrote it down.
Camera::Camera() : pitchAngle(0), yawAngle(2*M_PI-M_PI/2), direction(-1.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), right(0.0f, 0.0f, -1.0f), position(2.0f, 0.0f, 0.5f)
{
	updateViewMatrix();
	updateProjectionMatrix();
}


void Camera::update()
{
	const float moveSpeed = 0.04f;
	if (Keys[SDLK_w].pr){
		position += direction*moveSpeed;
	}else if (Keys[SDLK_s].pr){
		position -= direction* moveSpeed;
	}

	if (Keys[SDLK_d].pr){
		position += right*moveSpeed;
	}
	else if (Keys[SDLK_a].pr){
		position -= right* moveSpeed;
	}

	if (Keys[SDLK_SPACE].pr){
		position += glm::vec3(0.0, 1.0, 0.0)*moveSpeed;
	}
	else if (Keys[SDLK_LSHIFT].pr){
		position -= glm::vec3(0.0, 1.0, 0.0)* moveSpeed;
	}

	updateViewMatrix();
}

//this function moves the camera direction when the player is dragging the camera
void Camera::mouseMove(int relx, int rely)
{
	yawAngle += controlsOptions.mouseSensitivity * float(-relx) * 0.005f;
	pitchAngle += controlsOptions.mouseSensitivity * float(-rely) * 0.005f;
	cout << pitchAngle << " " << yawAngle << endl;
	//bound angles
	if (pitchAngle > M_PI / 2.0f){
		pitchAngle = M_PI / 2.0f;
	}
	else if (pitchAngle < -M_PI / 2.0f){
		pitchAngle = -M_PI / 2.0f;
	}

	//wrap angles
	if (yawAngle > 2.0f*M_PI){
		yawAngle = 0.0f;
	}
	else if (yawAngle < 0.0f){
		yawAngle = 2.0f*M_PI - 0.00001f; //for some reason it gets stuck at zero without the - 0.00001f
	}

	direction = glm::vec3(
		cos(pitchAngle) * sin(yawAngle),
		sin(pitchAngle),
		cos(pitchAngle) * cos(yawAngle)
		);

	// Right vector
	right = glm::vec3(
		sin(yawAngle - 3.14f / 2.0f),
		0,
		cos(yawAngle - 3.14f / 2.0f)
		);

	up = glm::cross(right, direction);
}

//update the view matrix every time the camera moves, or every frame
void Camera::updateViewMatrix()
{
	glm::vec3 emptyVec(0.0f, 0.0f, 0.0f);
	// Camera matrix
	viewMatrix = glm::lookAt(
		emptyVec,           // Camera is always at the origin. We translate the world around us, we dont translate the camera
		glm::vec3(direction), // and looks here : at the same position, plus "direction"
		glm::vec3(up)                  // Head is up (set to 0,-1,0 to look upside-down)
		);
}

//update the projection matrix only when the fov changes
void Camera::updateProjectionMatrix()
{
	float znear = 0.01;
	float zfar = 10000.0f;
	projectionMatrix = glm::perspective((float)(graphicsOptions.fov), (float)graphicsOptions.screenWidth / (float)graphicsOptions.screenHeight, znear, zfar);
}