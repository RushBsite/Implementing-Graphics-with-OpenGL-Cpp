#include "glSetup.h"
#include <iostream>

void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

// Colors
GLfloat bg[4] = { 1, 1, 1, 1 };

int main (int argc, char* argv[])
{
	// Orthographics viewing
	perspectiveView = false;

	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bg);
	if (window == NULL) return -1;

	//Callbacks
	glfwSetKeyCallback(window, keyboard);

	// Viewport and perspective setting
	reshape(window, windowW, windowH);

	// Usage
	std::cout << std::endl;
	std::cout << "Keyboard Input: q for quit" << std::endl;

	//Main loop
	while (!glfwWindowShouldClose(window))
	{
		render(window);		//Draw one frame
		glfwSwapBuffers(window); // Swap buffers
		glfwPollEvents();	//Events
	}
	//Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void render(GLFWwindow* window)
{
	//Background color
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(5 * dpiScaling);

	//Draw a rectiangle using GL_LINE_LOOP
	/*glBegin(GL_LINE_LOOP);
	{
		glColor3f(1, 0, 0); glVertex2f(-0.5, -0.5);
		glColor3f(0, 1, 0); glVertex2f(0.5, -0.5);
		glColor3f(0, 0, 1); glVertex2f(0.5, 0.5);
		glColor3f(0, 0, 0); glVertex2f(-0.5, 0.5);
	}*/

	//Draw a rectiangle using GL_LINES
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0); glVertex2f(-0.5, -0.5); 
		glColor3f(0, 1, 0); glVertex2f(0.5, -0.5);  //v0 to v1

		glColor3f(0, 1, 0); glVertex2f(0.5, -0.5); 
		glColor3f(0, 0, 1); glVertex2f(0.5, 0.5); //v1 to v2
		
		glColor3f(0, 0, 1); glVertex2f(0.5, 0.5);
		glColor3f(0, 0, 0); glVertex2f(-0.5, 0.5); //v2 to v3

		glColor3f(0, 0, 0); glVertex2f(-0.5, 0.5);
		glColor3f(1, 0, 0); glVertex2f(-0.5, -0.5); //v3 to v0
	}
	glEnd();
}

void keyboard(GLFWwindow* window, int key, int code, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch(key)
		{
			// Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}
