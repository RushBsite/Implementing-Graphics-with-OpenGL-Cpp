#include "glSetup.h"
#include <iostream>
using namespace::std;

#ifdef _WIN32
#define _USE_MATH_DEFINES // To include the definition of M_PI in math.h
#endif

#include <math.h>

void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);
void mouseButton(GLFWwindow* window, int button, int action, int mods);
void mouseMove(GLFWwindow* window, double x, double y);

// Global coordinate frame
float	AXIS_LENGTH = 0.5;
float	AXIS_LINE_WIDTH = 2.0;

//Colors
GLfloat bgColor[4] = { 1, 1, 1, 1, };

//Endpoints of the line segment
float point[3][2] = { {0,0}, {0,0}, {0,0} };
//Max Click count
int c_count = 0;
// Mouse move trigger
bool IsMouseMove = false;

// Mouse input mode
enum class InputMode
{
	NONE = 0,
	CLICK = 1,
	COMPLETE = 2,
};

InputMode inputMode = InputMode::NONE;

// Paly configuration

bool pause = true;

float timeStep = 1.0f / 120; // 120fps
float period = 8.0;

void rotate(float p[2], float delta_theta)
{
	float x = p[0];
	float y = p[1];

	float cos_delta_theta = cos(delta_theta);
	float sin_delta_theta = sin(delta_theta);

	p[0] = cos_delta_theta * x - sin_delta_theta * y;
	p[1] = sin_delta_theta * x + cos_delta_theta * y;

}

int main(int argc, char* argv[])
{
	// Orthographic projection
	perspectiveView = false;
	
	// Fps control: vsync should be 0 for precise time stepping
	vsync = 0; // 0 for immeditate mode ( Tearing possible)

	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	// Callbacks
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, mouseMove);

	//Viewport and persepective setting
	reshape(window, windowW, windowH);

	//Usage

	cout << endl;
	
	cout << "Mouse click:	set the points of the triangle" << endl;
	cout << "keyboard input:	space for play/pause" << endl;
	cout << "keyboard input:	up for increasing period" << endl;
	cout << "keyboard input:	down for decreasing period" << endl;

	//Main loop

	float previous = (float)glfwGetTime();
	float elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		//Time passed during a single loop
		float now = (float)glfwGetTime();
		float delta_t = now - previous;
		previous = now;

		//Time passed after the previous frame
		elapsed += delta_t;

		//cout << "elapsed = " << elasped * 1000 << ms << endl;

		//Deal with the current frame
		if (elapsed > timeStep)
		{
			//Rotate the line segment if not paused
			if (!pause && inputMode == InputMode::COMPLETE)
			{
				//One rotation per period
				float delta_theta = float(2.0 * M_PI) / period * elapsed;

				rotate(point[0], delta_theta);
				rotate(point[1], delta_theta);
				rotate(point[2], delta_theta);

			}
			// cout << "|telapsed = " << elapsed * 1000 << " ms" << endl;
			// cout << "\ttimeStep = " << timeStep * 1000 < " ms "<< endl;
			elapsed = 0; // Reset the elapsed time
		   // cout <<endl;
		   // 
		}
		render(window);
		glfwSwapBuffers(window);
	}
	//Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}

void render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ModelView matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Axes
	glDisable(GL_LINE_STIPPLE);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);

	//Attribute of the line segment
	glColor3f(0, 0, 1);
	glLineWidth(5 * dpiScaling);
	glPointSize(5 * dpiScaling); // increase point size
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // line mode

	//Desired position of the end-effector
	if (inputMode == InputMode::COMPLETE && c_count == 1 && IsMouseMove == true)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(int(3 * dpiScaling), 0xcccc);
		glBegin(GL_LINES);
		glVertex2f(point[0][0], point[0][1]);
		glVertex2f(point[1][0], point[1][1]);
		glEnd();
	}
	else if (inputMode == InputMode::COMPLETE && c_count == 2 && IsMouseMove == true)
	{
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
		glVertex2f(point[1][0], point[1][1]);
		glVertex2f(point[2][0], point[2][1]);
		glEnd();
	}
	else glDisable(GL_LINE_STIPPLE);
	
	if (inputMode > InputMode::NONE && c_count == 1)
	{
		glBegin(GL_POINTS);
		glVertex2f(point[0][0], point[0][1]);
		glEnd();
	}
	else if (inputMode > InputMode::NONE && c_count == 2)
	{
		glDisable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
		glVertex2f(point[0][0], point[0][1]);
		glVertex2f(point[1][0], point[1][1]);
		glEnd();
	}
	else if (inputMode > InputMode::NONE && c_count == 3)
	{
		glBegin(GL_TRIANGLES);
		glVertex2f(point[0][0], point[0][1]);
		glVertex2f(point[1][0], point[1][1]);
		glVertex2f(point[2][0], point[2][1]);
		glEnd();
		
	}
}

void keyboard(GLFWwindow* window, int key, int code, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			//Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
			//play on/off
		case GLFW_KEY_SPACE:  pause = !pause; break;
			//period control
		case GLFW_KEY_UP:	  period += 0.1f; break;
		case GLFW_KEY_DOWN:   period = (float)max(period - 0.1, 0.1); break;
		}
	}
}

void screen2world(float xs, float ys, float& xw, float& yw)
{
	//in the world space. See reshape() in glSetup.cpp
	float aspect = (float)screenW / screenH;
	xw = 2.0f * (xs / (screenW - 1) - 0.5f) * aspect;
	yw = -2.0f * (ys / (screenH - 1) - 0.5f);
}

void mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	// Mouse cursor posititon in the screen coordinate system
	double xs, ys;
	glfwGetCursorPos(window, &xs, &ys);

	//Screen coordinate 2 world coordinate conversion
	float xw, yw;
	screen2world((float)xs, (float)ys, xw, yw);

	IsMouseMove = false;

	// the left button of the mouse is pressed

	switch (c_count)
	{
	case 3:
	case 0:
		// the left button of the mouse is pressed
		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			inputMode = InputMode::CLICK; // click starts
		}

		// The left button of the mouse is released;
		if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			inputMode = InputMode::COMPLETE; //click ends

			point[0][0] = xw; point[0][1] = yw; // 1st point
			c_count = 1; //increase count count 3 -> count 1
		}
		break;
	case 1:
		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			inputMode = InputMode::CLICK; // click starts
		}

		// The left button of the mouse is released;
		if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			inputMode = InputMode::COMPLETE; //click ends

			point[1][0] = xw; point[1][1] = yw; // 2nd point
			c_count ++; //increase count
		}
		break;
	case 2:
		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			inputMode = InputMode::CLICK; // click starts
		}

		// The left button of the mouse is released;
		if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			inputMode = InputMode::COMPLETE; //click ends

			point[2][0] = xw; point[2][1] = yw; // 3rd point
			//cout << "xw , yw" << xw << "" << yw << endl;
			c_count++; //increase count
		}
		break;
	
		
	}
	
	//cout << "c_count : " << c_count << endl;
	
}

void mouseMove(GLFWwindow* window, double x, double y)
{
	IsMouseMove = true;
	if (inputMode == InputMode::COMPLETE && c_count == 1)
	{
		//Screen coordinate 2 world coordinate conversion
		screen2world((float)x, (float)y, point[1][0], point[1][1]);
	}
	else if (inputMode == InputMode::COMPLETE && c_count == 2)
	{
		//Screen coordinate 2 world coordinate conversion
		screen2world((float)x, (float)y, point[2][0], point[2][1]);
	}
}





