#pragma once

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include "Transform.h"

class Camera {

public:

	void update(){
		glm::vec2 seek = glm::vec2(seekPoint.x, seekPoint.y);

		//update position w/ seek
		glm::vec2 dir = (seek - pos);
		dir = dir * (1.0f/dir.length());

		acc = dir * followForce;

		vel += acc;
		if (vel.length() > maxSpeed)
			vel = (vel * (1.0f / vel.length())) * maxSpeed;

		pos += vel;

		//update zoom
		if (abs(targetZoom - zoom) > 0.03f) {
			if (targetZoom > zoom)
				zoomGain += followForce * 0.03f;
			else if (targetZoom < zoom)
				zoomGain += followForce * -0.03f;
			else {
				zoomGain += 0;
			}

			if (abs(zoomGain) > maxSpeed * 0.03f)
				zoomGain = (zoomGain / abs(zoomGain)) * maxSpeed * 0.03f;

			zoom += zoomGain;
		}

		//check bounds
		if (abs(pos.x) > camBounds.x) {
			pos.x = (pos.x / abs(pos.x)) * camBounds.x;
			vel.x = 0;
			acc.x = 0;
		}
		if (abs(pos.y) > camBounds.y) {
			pos.y = (pos.y / abs(pos.y)) * camBounds.y;
			vel.y = 0;
			acc.y = 0;
		}
		if (zoom < zoomBounds.x) {
			zoom = zoomBounds.x;
			zoomGain = 0;
		}
		else if (zoom > zoomBounds.y) {
			zoom = zoomBounds.y;
			zoomGain = 0;
		}

		//update cam transform
		CameraTransform = Transform::Identity();
		CameraTransform.RotateX(-10.0f);

		//apply rumble
		if (rumble) {
			float shakeX = ((rand() % 11) - 5) * 0.05f;
			float shakeY = ((rand() % 11) - 5) * 0.05f;
			CameraTransform.Translate(glm::vec3(pos.x + shakeX, pos.y + shakeY + zoom, zoom + 5));
		}
		else {
			CameraTransform.Translate(glm::vec3(pos.x, pos.y + zoom - 1, zoom + 5));
		}
	}

	void reset() {
		//pos = glm::vec2(0, 2);
		vel = glm::vec2(0, 0);
		acc = glm::vec2(0, 0);
		zoom = zoom++;
		rumble = false;

		CameraTransform = Transform::Identity();
		CameraTransform.SetTranslation(glm::vec3(pos.x, pos.y + zoom - 1, zoom + 5));
	}


	Transform CameraTransform = Transform::Identity();
	Transform CameraProjection = Transform::Identity();

	glm::vec3 seekPoint;
	float targetZoom;
	bool rumble = false;

	glm::vec2 pos = glm::vec2(0, 0);


private:

	float zoom = 10;
	float zoomGain = 0;
	glm::vec2 vel = glm::vec2(0, 0);
	glm::vec2 acc = glm::vec2(0, 0);

	glm::vec2 camBounds = glm::vec2(25, 15);//x = left/right, y = up/down
	glm::vec2 zoomBounds = glm::vec2(10, 19);
	float followForce = 0.1f;
	float maxSpeed = 1.0f;


};