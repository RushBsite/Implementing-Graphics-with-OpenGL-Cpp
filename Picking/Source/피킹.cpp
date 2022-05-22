#include "glSetup.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <fstream>
using namespace std;

void init();
void quit();
void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);
void mouseButton(GLFWwindow* window, int button, int action, int mods);
void mouseMove(GLFWwindow* window, double x, double y);
void drawBox(float l, float w);
void windowToPlane(float xs, float ys, float& xw, float& yw);
int* findAllHits(int hits, GLuint selectBuffer[64]);

//Camera

vec3 eyeTopView(0, 10, 0); //Top View
vec3 upTopView(0, 0, -1);

vec3 eyeFrontView(0, 0, 10); //Front View
vec3 upFrontView(0, 1, 0);

vec3 eyeBirdView(0, -10, 4); //Bird eye View
vec3 upBirdView(0, 1, 0);
vec3 center(0, 0, 0);

float timeStep = 1.0f / 120; // 120fps
float period = 8.0;

//Light configuration
vec4 light(0.0, 0.0, 0.8, 1); // Initial light position

//Global coordinate frame
bool axes = true;
float AXIS_LENGTH = 1.25;
float AXIS_LINE_WIDTH = 2;
//Colors

GLfloat bgColor[4] = { 1,1,1,1, };//Colors

// control variable
int view = 1;

//Picking
int picked = -1;
int* pickes = NULL;
//drag box coordinate
float point[2][2] = { {0.13,0.13},{-0.13,-0.13} };
float Zbuf;
double BRx, BRy, TLx, TLy;
//Mouse move flag
bool IsMouseMove = false;

// Mouse input mode
enum class InputMode
{
	NONE = 0,
	CLICK = 1,
	DRAG = 2,
};

InputMode inputMode = InputMode::NONE;

int main(int argc, char* argv[])
{

	fovy = 16.1f;

	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	// callbacks
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, mouseMove);

	//Depth test
	glEnable(GL_DEPTH_TEST);

	//normal vec after trasnformation
	glEnable(GL_NORMALIZE);

	//Viewport and perspective setting
	reshape(window, windowW, windowH);

	//Initialization - main loop - finalization

	init();

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

		
		render(window);
		glfwSwapBuffers(window);

	}
	quit();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

// Sphere

GLUquadricObj* sphere = NULL;

void init()
{

	//Prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GL_FALSE);

	cout << endl;
	cout << "Keyboard input: v for the top, front, bird-eye view" << endl;
	cout << "Mouse button down: Select an object" << endl;
	
}

void quit()
{
	gluDeleteQuadric(sphere);
	
}

void setupColoredMaterial(const vec3& color)
{
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[4] = { color[0],color[1],color[2],1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 100.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawSphere(float radius, int slices, int stacks)
{
	gluSphere(sphere, radius, slices, stacks);
}

void setupLight(const vec4& p) {

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(p));
}

void render(GLFWwindow* widnow)
{
	//Picking
	glInitNames();//Initializse the name stack
	glPushName(-1);//Push at least one name in the stack

	//background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vec3 eye(0), up(0);
	switch (view)
	{
	case	0:	eye = eyeTopView;	up = upTopView;	break;
	case	1:	eye = eyeFrontView;	up = upFrontView;	break;
	case	2:	eye = eyeBirdView;	up = upBirdView;	break;
	}

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	if(axes)
	{
		glDisable(GL_LIGHTING);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
		
	}

	if(inputMode == InputMode::CLICK && IsMouseMove == true)
	{
		drawBox(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}
	
	glShadeModel(GL_SMOOTH);
	//Lighting
	//
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	setupLight(light);

	//Draw objects
	//
	// 3x3 spheres or bunnies
	glScalef(0.4f, 0.4f, 0.4f);

	vec3 u[3];
	vec3 v[3];
	u[0] = vec3(0, 1, 0) * 2.0f; u[1] = vec3(0, 0, 0); u[2] = -u[0];
	v[0] = -vec3(1, 0, 0) * 4.0f; v[1] = vec3(0, 0, 0); v[2] = -v[0];

	for (int i = 0; i < 3; i++)
	{
		glPushMatrix();
		glTranslatef(u[i].x, u[i].y, u[i].z);
		for (int j = 0; j < 3; j++)
		{
			glPushMatrix();
			glTranslatef(v[j].x, v[j].y, v[j].z);
			// Material
			if (picked == (3 * i + j)) setupColoredMaterial(vec3(0, 0, 1));
			else
			{
				setupColoredMaterial(vec3(1, 1, 1));
			}

			//Picking
			glLoadName(3 * i + j); //Replace the name for the i-th row, j-th colum
			//Draw a sphere
			drawSphere(0.7f, 64, 64);
			glPopMatrix();
		}
		glPopMatrix();
	}
	
}
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key) { // Quit 
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
			//axes
		case GLFW_KEY_X: axes = !axes; break;
			// Initialization 
		
		case GLFW_KEY_V: view = (view + 1) % 3; break;
			
		}
	}
}

void drawBox(float l, float w)
{
	glLineWidth(w);
	glColor3f(0, 1, 1);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(int(3 * dpiScaling), 0xcccc);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // line mode
	glBegin(GL_QUADS);
	glVertex3f(point[0][0], point[0][1],Zbuf);
	glVertex3f(point[0][0], point[1][1],Zbuf);
	glVertex3f(point[1][0], point[1][1],Zbuf);
	glVertex3f(point[1][0], point[0][1],Zbuf);

	/*glVertex3f(0.5f, 0.5, 0);
	glVertex3f(0.0f, 0.5, 0);
	glVertex3f(0.0f, 0.0, 0);
	glVertex3f(0.5f, 0.0, 0);*/
	
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // line mode
}

int findNearestHits(int hits, GLuint selectBuffer[64])
{
	bool diagnosis = true;

	if (diagnosis&&!IsMouseMove) cout << "hits = " << hits << endl;

	int name = -1;
	float nearest = 2.0;

	int index = 0;

	for (int i =0; i< hits; i++)
	{
		int n = selectBuffer[index + 0]; //#of names
		float z1 = (float)selectBuffer[index + 1] / 0xffffffff; //min depth val
		float z2 = (float)selectBuffer[index + 2] / 0xffffffff; //max depth val

		// choose the nearest one only

		if (z1 < nearest) { nearest = z1; name = selectBuffer[index + 3]; }

		if(diagnosis&&!IsMouseMove)
		{
			cout << "\t# of names = " << n << endl;
			cout << "\tz1 = " << z1 << endl;
			cout << "\tz2 = " << z2 << endl;
			cout << "\tnames: ";
			for (int j = 0; j < n; j++)
				cout << selectBuffer[index + 3 + j] << " ";
			cout << endl;
		}
		index += (3 + n);
	}
	if (diagnosis&&!IsMouseMove) cout << "picked = " << name << endl;

	return name;
}


int selectObject(GLFWwindow* window, double x, double y)
{
	double delX;
	double delY;
	if(!IsMouseMove)
	{
		delX = 2;
		delY = 2;
	}
	else
	{
		delX = BRx - TLx;
		delY = TLy - BRy;
	}
	

	GLuint selectBuffer[64];
	glSelectBuffer(64, selectBuffer);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	//Backkup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	/// a small region of the viewport
	gluPickMatrix(x, viewport[3] - y, delX, delY, viewport);
	//Expolit the projection matrix for normal rendering
	setupProjectionMatrix();

	//enter selection mode
	glRenderMode(GL_SELECT);

	render(window);

	GLint hits = glRenderMode(GL_RENDER);
	int name = findNearestHits(hits, selectBuffer);

	//Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	return name;
}



void unProject(double xCursor, double yCursor, double* wx, double* wy, double* wz)
{
	GLdouble projection[16];
	GLdouble modelView[16];
	GLint viewPort[4];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	GLfloat zCursor, winX, winY;
	winX = (float)xCursor;
	winY = (float)viewPort[3] - (float)yCursor;
	zCursor = 0.0f;

	if (gluUnProject(winX, winY, zCursor, modelView, projection, viewPort, wx, wy, wz) == GLU_FALSE) {
		cout << "failed" << endl;
	}
}


void mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	
	
	if(action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		IsMouseMove = false;
		inputMode = InputMode::CLICK;
		double xs, ys, wx, wy, wz;
		glfwGetCursorPos(window, &xs, &ys);

		TLx = xs * dpiScaling; //top left x
		TLy = ys * dpiScaling; // top left y

		
		unProject(TLx, TLy, &wx, &wy, &wz);

		point[0][0] = wx;
		point[0][1] = wy;
		Zbuf = 7.5;
		picked = selectObject(window, TLx, TLy);
	}

	if(action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		inputMode = InputMode::NONE;
		//picked = selectObject(window, (TLx + BRx) / 2.0f, (TLy + BRy) / 2.0f);
		IsMouseMove = false;
	}
}

void mouseMove(GLFWwindow* window, double x, double y)
{
	
	IsMouseMove = true;
	if(inputMode == InputMode::CLICK)
	{
		
		double wx, wy, wz;
		BRx = x * dpiScaling; //Bottom right x
		BRy = y * dpiScaling; //Bottom right y

		
		unProject(BRx, BRy, &wx, &wy, &wz);

		point[1][0] = wx;
		point[1][1] = wy;
		
	}
}


