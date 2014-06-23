#pragma once
//
// Camera.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <glm\glm.hpp>

//simple camera controller. Could be a lot better
class Camera
{
public:
	Camera();
	void update();
	void mouseMove(int relx, int rely);
	void mouseZoom(float zoomScale);
	glm::vec3 screenToWorld(glm::vec2 mouse, int width, int height);
	bool boundsCheck();

	void updateViewMatrix();
	void updateProjectionMatrix();

    //getters
    glm::vec3 getPosition() const { return _position; }

private:
    glm::vec3 _position;
    glm::vec3 _direction;
    glm::vec3 _up;
    glm::vec3 _right;
    glm::mat4 _viewMatrix;
    glm::mat4 _projectionMatrix;

	float _pitchAngle;
	float _yawAngle;

};

