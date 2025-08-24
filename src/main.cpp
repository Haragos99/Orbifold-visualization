
#include "framework.h"

Sceen sceen;
// Initialization, create an OpenGL context
void onInitialization()
{
	glViewport(0, 0, windowWidth, windowHeight);
	sceen = Sceen();
	sceen.Build();

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
	sceen.Draw();
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
	sceen.Animate(time);
	glutPostRedisplay();
}