#define _USE_MATH_DEFINES		// M_PI
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl3.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>		// must be downloaded
#include <GL/freeglut.h>	// must be downloaded unless you have an Apple
#endif
#include "algebra.h"
#include "GPUProgram.h"
#include "camera.h"
// Resolution of screen
const unsigned int windowWidth = 600, windowHeight = 600;

class Sceen {

	unsigned int vao, vbo;
	GPUProgram sha;
	Camera camera;
	float F(float n, float k)
	{
		return ((n - 1) * (n - 1) + k * k) / ((n + 1) * (n + 1) + k * k);
	}
public:
	void Build();
	// the Draw
	void Draw();

	void Animate(float time);

};
