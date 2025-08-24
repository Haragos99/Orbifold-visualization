#pragma once
#include "algebra.h"
#define M_PI       3.14159265358979323846   // pi
struct Camera {
public:
	vec3 eye, lookat, right, pvup, rvup;
	float fov = 45 * (float)M_PI / 180;

	Camera() : eye(0, 1.7, 1.7), pvup(0, 0, 1), lookat(0, 0, 0)
	{
		set();
	}
	//the position set
	void set()
	{
		vec3 w = eye - lookat;
		float f = length(w);
		right = normalize(cross(pvup, w)) * f * tanf(fov / 2);
		rvup = normalize(cross(w, right)) * f * tanf(fov / 2);
	}
	// the animation
	void Animate(float t)
	{
		float r = sqrtf(eye.x * eye.x + eye.y * eye.y);
		eye = vec3(r * cos(t) + lookat.x, r * sin(t) + lookat.y, eye.z);
		set();
	}

};