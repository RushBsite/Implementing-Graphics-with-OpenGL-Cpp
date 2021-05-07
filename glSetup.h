#ifndef __GL_SETUP_H_
#define __GL_SETUP_H_

#if defined(__APPLE__)	&&	defined(__MACH__)
	#include <OpenGL/glu.h>
#else
	#include <windows.h>
	#include <gl/GLU.h>
#endif


#include <GLFW/glfw3.h>

extern float	screenScale;		//Portion of the screen when not using Full screen
extern int		screenW, screenH;	//screenScale portion of the screen
extern int		windowW, windowH;
extern float	aspect;
extern float	dpiScaling;

extern int		vsync;

extern bool		perspectiveView;
extern float	fovy;
extern float	nearDist;
extern float	farDist;

GLFWwindow* initializeOpenGL(int argc, char* argv[], GLfloat bg[4]);
void		reshape(GLFWwindow* winddow, int w, int h);
void		setupProjectionMatrix();

void		drawAxes(float l, float w);

#endif


 