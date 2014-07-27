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
Camera::Camera() : _pitchAngle(0), _yawAngle(3*M_PI/2), _direction(-1.0f, 0.0f, 0.0f), _up(0.0f, 1.0f, 0.0f), _right(0.0f, 0.0f, -1.0f), _position(2.0f, 0.0f, 0.5f)
{
	updateViewMatrix();
	updateProjectionMatrix();
}


void Camera::update()
{
	const float moveSpeed = 0.5f;
	if (Keys[SDLK_w].pr){
		_position += _direction*moveSpeed;
	}else if (Keys[SDLK_s].pr){
		_position -= _direction* moveSpeed;
	}

	if (Keys[SDLK_d].pr){
		_position += _right*moveSpeed;
	}
	else if (Keys[SDLK_a].pr){
		_position -= _right*moveSpeed;
	}

	if (Keys[SDLK_SPACE].pr){
		_position += _up * moveSpeed;
	}
	else if (Keys[SDLK_LSHIFT].pr){
		_position -= _up * moveSpeed;
	}

	updateViewMatrix();
}

//this function moves the camera direction when the player is dragging the camera
void Camera::mouseMove(int relx, int rely)
{
	_yawAngle += controlsOptions.mouseSensitivity * float(-relx) * 0.005f;
	_pitchAngle += controlsOptions.mouseSensitivity * float(-rely) * 0.005f;

	//bound angles
	if (_pitchAngle > M_PI / 2.0f){
		_pitchAngle = M_PI / 2.0f;
	}
	else if (_pitchAngle < -M_PI / 2.0f){
		_pitchAngle = -M_PI / 2.0f;
	}

	//wrap angles
	if (_yawAngle > 2.0f*M_PI){
		_yawAngle = 0.0f;
	}
	else if (_yawAngle < 0.0f){
		_yawAngle = 2.0f*M_PI - 0.00001f; //for some reason it gets stuck at zero without the - 0.00001f
	}

	_direction = glm::vec3(
		cos(_pitchAngle) * sin(_yawAngle),
		sin(_pitchAngle),
		cos(_pitchAngle) * cos(_yawAngle)
		);

	// Right vector
	_right = glm::vec3(
		sin(_yawAngle - 3.14f / 2.0f),
		0,
		cos(_yawAngle - 3.14f / 2.0f)
		);

	_up = glm::cross(_right, _direction);
}

void Camera::mouseZoom(float zoomScale){
	zoomScale *= 0.7f;
	_position += _direction * zoomScale;
}

//update the view matrix every time the camera moves, or every frame
void Camera::updateViewMatrix()
{
	glm::vec3 emptyVec(0.0f, 0.0f, 0.0f);
	// Camera matrix
	_viewMatrix = glm::lookAt(
		emptyVec,           // Camera is always at the origin. We translate the world around us, we dont translate the camera
		glm::vec3(_direction), // and looks here : at the same position, plus "direction"
		glm::vec3(_up)                  // Head is up (set to 0,-1,0 to look upside-down)
		);
}

//update the projection matrix only when the fov changes
void Camera::updateProjectionMatrix()
{
	const float znear = 0.01;
    const float zfar = 10000.0f;
	_projectionMatrix = glm::perspective((float)(graphicsOptions.fov), (float)graphicsOptions.screenWidth / (float)graphicsOptions.screenHeight, znear, zfar);
}

glm::vec3 Camera::screenToWorld(glm::vec2 mouse, int width, int height)
{
	const float x = (2.0f * mouse.x) / width - 1.0f;
	const float y = 1.0f - (2.0f * mouse.y) / height;
	glm::vec4 rayClip(x, y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(_projectionMatrix) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);
	glm::vec3 rayWorld = glm::vec3(glm::inverse(_viewMatrix) * rayEye);
	return glm::normalize(rayWorld);
}