
#include "framework.h"
#include "GPUProgram.h"
#include <fstream>
#include <string>
#include <iostream>
	// Function to read GLSL shader source code from a file
	std::string readShaderFromFile(const std::string& filePath) {
		std::ifstream file(filePath);

		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << filePath << std::endl;
			return "";
		}

		std::string shaderSource;
		std::string line;

		while (std::getline(file, line)) {
			shaderSource += line + "\n";
		}

		file.close();
		return shaderSource;
	}

//Camera
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


Camera camera;
GPUProgram sha;
// for calculat the gold reflection
float F(float n, float k)
{
	return ((n - 1) * (n - 1) + k * k) / ((n + 1) * (n + 1) + k * k);
}

// This will generate the virtual world
class Picture {

	unsigned int vao, vbo;
	std::string fragmentshader;
	std::string vertexshader;
public:
	void Build()
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
		vertexshader = readShaderFromFile("C:\\Users\\Felhasználó\\OneDrive\\Documents\\GrafikaHaziCsomag\\Programs\\Skeleton\\Skeleton\\vertexSource.glsl");
		fragmentshader = readShaderFromFile("C:\\Users\\Felhasználó\\OneDrive\\Documents\\GrafikaHaziCsomag\\Programs\\Skeleton\\Skeleton\\fragmentSource.glsl");
		sha.create(vertexshader.c_str(), fragmentshader.c_str(), "fragmentColor");
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
	void Draw()
	{
		//Give the camera parametrs to GPU
		sha.setUniform(camera.eye, "wEye");
		sha.setUniform(camera.lookat, "wLookAt");
		sha.setUniform(camera.right, "wRight");
		sha.setUniform(camera.rvup, "wUp");
		// Draw the picture
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

};




Picture* picture;
// Initialization, create an OpenGL context
void onInitialization()
{
	glViewport(0, 0, windowWidth, windowHeight);
	picture = new Picture();
	picture->Build();

}

// Window has become invalid: Redraw
void onDisplay()
{
	static int nFrames = 0;
	nFrames++;
	static long tStart = glutGet(GLUT_ELAPSED_TIME);
	long tEnd = glutGet(GLUT_ELAPSED_TIME);
	printf("%d msec\r", (tEnd - tStart) / nFrames);
	glClearColor(1.0f, 0.5f, 0.8f, 1.0f);							// background color 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
	picture->Draw();
	glutSwapBuffers();									// exchange the two buffers
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {

}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {

}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	// the animtaion
	float f = glutGet(GLUT_ELAPSED_TIME);
	float time = f / 1000.0f;
	camera.Animate(time);
	glutPostRedisplay();
}