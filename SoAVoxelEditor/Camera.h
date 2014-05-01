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

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	void updateViewMatrix();
	void updateProjectionMatrix();
private:
	float pitchAngle;
	float yawAngle;

};

