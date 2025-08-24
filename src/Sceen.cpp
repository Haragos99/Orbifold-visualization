#include "framework.h"

void Sceen::Build()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// the virtual world cordinate
	float vertexCoords[] = { -1,-1,1,-1,1,1,-1,1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	sha.create("vertexSource.glsl", "fragmentSource.glsl", "fragmentColor");
	// Dodecaeders points 
	std::vector<vec3> v{
		vec3(0,0.618f,1.618f),vec3(0,-0.618f,1.618f),vec3(0,-0.618f,-1.618f),vec3(0,0.618f,-1.618f),vec3(1.618f,0,0.618f),vec3(-1.618f,0,0.618f),
		vec3(-1.618f,0,-0.618f),vec3(1.618f,0,-0.618f),vec3(0.618f,1.618f,0),vec3(-0.618f,1.618f,0),vec3(-0.618f,-1.618f,0),vec3(0.618f,-1.618f,0),
		vec3(1,1,1),vec3(-1,1,1),vec3(-1,-1,1),vec3(1,-1,1),vec3(1,-1,-1),vec3(1,1,-1),vec3(-1,1,-1),vec3(-1,-1,-1)
	};
	// put to the fragment
	for (int i = 0; i < v.size(); i++)
	{
		sha.setUniform(v[i], "v[" + std::to_string(i) + "]");
	}
	// index of the array
	std::vector<int> planes{
		1,2,16, 1,13,9, 1,14,6, 2,15,11, 3,4,18, 3,17,12, 3,20,7, 19,10,9 ,16,12,17, 5,8,18, 14,10,19 ,6,7,20
	};
	// put to the fragment
	for (int i = 0; i < planes.size(); i++)
	{
		sha.setUniform(planes[i], "planes[" + std::to_string(i) + "]");
	}
	// The gold refraction
	sha.setUniform(vec3(F(0.17, 3.1), F(0.35, 2.7), F(1.5, 1.9)), "F0");
	// for the rotation
	sha.setUniform(cosf(72), "c");
	sha.setUniform(sinf(72), "s");
}
// the Draw
void Sceen::Draw()
{
	//Give the camera parametrs to GPU
	sha.setUniform(camera.eye, "wEye");
	sha.setUniform(camera.lookat, "wLookAt");
	sha.setUniform(camera.right, "wRight");
	sha.setUniform(camera.rvup, "wUp");
	// Draw the picture
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Sceen::Animate(float time)
{
	camera.Animate(time);
}