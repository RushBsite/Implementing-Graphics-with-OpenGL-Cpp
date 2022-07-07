#include "glSetup.h"

#include <string.h>
#include <iostream>
using namespace std;

bool	perspectiveView = true;

float	screenScale = 0.5f;
int		screenW = 0, screenH = 0;
int		windowW, windowH;
float	aspect;
float	dpiScaling = 0;

int		vsync = 1;

float	fovy = 46.4f;	//fovy of 28mm lens in degree
//float	fovy = 37.8f;	//fovy of 35mm lens in degree
//float	fovy = 27.0f;	//fovy of 50mm lens in degree
//float	fovy = 16.1f;	//fovy of 85mm lens in degree
//float	fovy = 11.4f;	//fovy of 120mm lens in degree
//float	fovy = 6.9f;	//fovy of 200mm lens in degree

float	nearDist = 1.0f;
float	farDist = 20.0f;

void errorCallback(int error, const char* description)
{
	cerr << "####" << description << endl;
}

void setupProjectionMatrix()
{
	if (perspectiveView)	gluPerspective(fovy, aspect, nearDist, farDist);
	else
	{
		glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -nearDist, farDist);
	}
	
}

void reshape(GLFWwindow* window, int w, int h)
{
	aspect = (float)w / h;

	//set the viewport
	windowW = w;
	windowH = h;
	glViewport(0, 0, w, h);

	// INitialize the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//set up a projection matrix
	setupProjectionMatrix();

	// the screen size is required for mouse interaction.
	glfwGetWindowSize(window, &screenW, &screenH);
	cerr << "reshape(" << w << ", " << h << ")";
	cerr << " with screen" << screenW << " x " << screenH << endl;
}

GLFWwindow* initializeOpenGL(int argc, char* argv[], GLfloat bg[4])
{
	glfwSetErrorCallback(errorCallback);

	//Init GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);

	//enable OpenGL 2.1 in OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_SAMPLES, 4); //MSAA

	//Create the window
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW, monitorH;
	glfwGetMonitorPhysicalSize(monitor, &monitorW, &monitorH);
	cerr << "Status: Monitor " << monitorW << "mm x " << monitorH << "mm" << endl;

	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	if (screenW == 0)	screenW = int(videoMode->width * screenScale);
	if (screenH == 0)	screenH = int(videoMode->height * screenScale);

	GLFWwindow* window = glfwCreateWindow(screenW, screenH, argv[0], NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		cerr << "Failed in glfwCreateWindow()" << endl;
		return NULL;
	}

	//Context
	glfwMakeContextCurrent(window);

	//Clear the background ASAP
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	glfwSwapBuffers(window);

	//check the size of the window
	glfwGetWindowSize(window, &screenW, &screenH);
	cerr << "Status: Screen" << screenW << " x " << screenH << endl;

	glfwGetFramebufferSize(window, &windowW, &windowH);
	cerr << "Status: Framebuffer" << windowW << " x " << windowH << endl;

	//DPI scaling
	if (dpiScaling == 0)	dpiScaling = float(windowW) / screenW;

	//CallBacks
	glfwSetFramebufferSizeCallback(window, reshape);

	//Get the OpenGL version and renerer
	cout << "Status: Renderer" << glGetString(GL_RENDERER) << endl;
	cout << "Status: Ventor" << glGetString(GL_VENDOR) << endl;
	cout << "Status: OpenGL" << glGetString(GL_VERSION) << endl;

	//Vertical sync.
	glfwSwapInterval(vsync);

	return window;
}

void drawAxes(float l, float w)
{
	glLineWidth(w);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(l, 0, 0); // x - axis
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, l, 0); // y - axis
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, l); // z - axis
	glEnd();

}


