//=============================================================================================
// Collection of classes from lecture slides.
// Framework for assignments. Valid from 2019.
// Do not change it if you want to submit a homework.
//=============================================================================================
#include "framework.h"

// Initialization
void onInitialization();

// Window has become invalid: Redraw
void onDisplay();

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY);

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY);

// Move mouse with key pressed
void onMouseMotion(int pX, int pY);

// Mouse click event
void onMouse(int button, int state, int pX, int pY);

// Idle event indicating that some time elapsed: do animation here
void onIdle();


