#include <GL/glew.h>
#include "glSetup.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <fstream>
using namespace std;

void init();
void animate();
void render(GLFWwindow* window);
void keyboard(GLFWwindow*, int key, int code, int action, int mods);

//camera
vec3 eye1(0, 0, 3);  // square
vec3 at1(0, 0, 0);
vec3 eye5(3.5, 3, 3.5); //cube
vec3 at5(0, 0, 0);
vec3 eye6(0, 5, 15); // floor
vec3 at6(0, 0, 0);
vec3 upD(0, 1, 0);
vec3 forwardD;

//light

vec4 light1(0.0, 0.0, 1.0, 0);
vec4 light5(1.0, 1.0, 1.0, 0);
vec4 light6(0, 5, 15, 0);

//Global coordinate frame
bool axes = true;
float AXIS_LENGTH = 2;
float AXIS_LINE_WIDTH = 2;

//Colors
GLfloat bgColor[4] = { 1,1,1,1 };

//Paly configuration
bool pause = true;

float timeStep = 1.0f / 120;
float period = 4.0;

//Current frame
int frame = 0;

int example = 1;

//Texture square upside down
bool upsideDown = true;

//size of floor for the mipmapping ex
float floorSize = 400;

//OpenGl texture unit
GLuint texID[4];

//Texture parameters
bool texture = true;  //texture on / off
float textureNumRepeats = 1;

// MSAA
bool aaEnabled = false;

int main(int argc, char* argv[])
{
	farDist = 200.0;

	vsync = 0;

	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	cerr << "status : GLEW " << glewGetString(GLEW_VERSION) << endl;

	//Initializeing glew for glGenerateMipmap()
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		cerr << "error : " << glewGetErrorString(error) << endl;
		return 0;

	}

	// callbacks
	glfwSetKeyCallback(window, keyboard);

	//Depth test
	glEnable(GL_DEPTH_TEST);

	// Normal vectors ae normalized after transformation
	glEnable(GL_NORMALIZE);

	reshape(window, windowW, windowH);

	//main loop
	init();

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

		if (elapsed > timeStep)
		{
			if (!pause) frame += 1;
			elapsed = 0;
		}


		render(window);
		glfwSwapBuffers(window);


	}
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void loadCheckerboardTexture()
{
	int method = 1;

	const int w = 64; const int h = 64;
	GLubyte checkerboard[w][h];

	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			checkerboard[i][j] = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

	if (method == 1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, checkerboard);
	}
	else {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, w, h,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, checkerboard);
	}
	// Repeat for floor
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Mipmapping for floor
	if (method == 1) glGenerateMipmap(GL_TEXTURE_2D);
}

GLubyte demonHeadImage[3 * 128 * 128] =
{
	#include "m02_demon_image.h"
};

void loadDemonHeadTexture()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 128, 128, 0,
		GL_RGB, GL_UNSIGNED_BYTE, demonHeadImage);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

bool loadRawTexture(const char* filename, int w, int h, int n)
{
	//Open the raw texture file
	ifstream is(filename, ios::binary);
	if (is.fail())
	{
		cout << "can't open" << filename << endl;
		return false;
	}
	//	Allocate	memory
	GLubyte* raw = new GLubyte[w * h * n];

	//	Read all	the texels
	is.read((char*)raw, w * h * n);
	if (!is)	cout << "Error:	only " << is.gcount() << "bytes could be read!" << endl;

	//	Only 3 and	1
	if (n == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, raw);
	}
	else if (n == 1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, w, h, 0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, raw);
	}
	else {
		// Not supporting 2 channels
	}


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	Deallocate	memory
	delete[] raw;

	return true;

}

void setTextureFilter(int method)
{
	cout << "setTextureFilter(" << method << ")" << endl;
	int glMethod = GL_NEAREST;
	if (method <= 2)	// Magnification
	{
		switch (method)
		{
		case 1: glMethod = GL_NEAREST; break;
		case 2: glMethod = GL_LINEAR; break;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMethod);
	}
	else {	// Minification
		switch (method)
		{
		case	3:	glMethod = GL_NEAREST;	break;
		case	4:	glMethod = GL_LINEAR;	break;
		case	5:	glMethod = GL_NEAREST_MIPMAP_NEAREST;	break;
		case	6:	glMethod = GL_LINEAR_MIPMAP_NEAREST;	break;
		case	7:	glMethod = GL_NEAREST_MIPMAP_LINEAR;	break;
		case	8:	glMethod = GL_LINEAR_MIPMAP_LINEAR;		break;

		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMethod);

	}

}

void computeMovementDirections()
{
	vec3 rightD;

	// Forward and backward direction for navigation
	forwardD = normalize(at6 - eye6);	// Gaze direction
	rightD = normalize(cross(forwardD, upD));
	forwardD = normalize(cross(upD, rightD));	// Forward direction to move

}

void
moveForward(float d)
{
	eye6 += d * forwardD;
	at6 += d * forwardD;
}

void
turnForwardDirection(float degree)
{
	mat4 M(1.0f);
	M = translate(M, eye6);
	M = rotate(M, radians(degree), upD);
	M = translate(M, -eye6);

	at6 = M * vec4(at6, 1.0);

	// Forward/backward direction
	computeMovementDirections();
}

void init()
{
	computeMovementDirections();
	// Generate 4 textures and bind appropriate images
	glGenTextures(4, texID);

	// Checkerboard texture
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadCheckerboardTexture();

	setTextureFilter(1);	// Nearest for magnification
	setTextureFilter(3);	// Nearest for minification

		// Demon texture
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	loadDemonHeadTexture();

	// Raw texture
	glBindTexture(GL_TEXTURE_2D, texID[2]);
	loadRawTexture("m02_marble.raw", 512, 512, 3);

	glBindTexture(GL_TEXTURE_2D, texID[3]);
	loadRawTexture("m02_logo.raw", 512, 512, 1);

	// Keyboard cout << endl;
	cout << "Keyboard input: space for play/pause" << endl;
	cout << "Keyboard	input:	x	for	axes on/off" << endl;
	cout << "Keyboard	input:	a	for	MSAA on/off" << endl;
	cout << "Keyboard	input:	t	for	texture on/off" << endl;
	cout << endl;
	cout << "Keyboard	input:	b	for	the	checkerboard example" << endl;
	cout << "Keyboard	input:	m	for	the	marble example" << endl;
	cout << "Keyboard	input:	l	for	the logo example" << endl;
	cout << "Keyboard	input:	d	for	the demon head example" << endl;
	cout << "Keyboard	input:	f	for	the	floor example" << endl;
	cout << "Keyboard	input:	c	for	the	cube example" << endl;
	cout << endl;
	cout << "Keyboard	input:	1 for	magnification -	nearest" << endl;
	cout << "Keyboard	input:	2 for	magnification -	linear" << endl;
	cout << "Keyboard	input:	3	for	minification	-	nearest" << endl;
	cout << "Keyboard	input:	4	for	minification	-	linear" << endl;
	cout << "Keyboard	input:	5	for	minification	-	nearest mipmap nearest" << endl;
	cout << "Keyboard	input:	6	for	minification	-	linear mipmap nearest" << endl;
	cout << "Keyboard	input:	7	for	minification	-	nearest mipmap linear" << endl;
	cout << "Keyboard	input:	8	for	minification	-	linear mipmap linear" << endl;
	cout << "Keyboard	input:	up, down, left, right	for viewing" << endl;
	cout << "Keyboard	input:	SHIFT	+ up, down for inc/decreasing # repeats" << endl;


}

void setupLight(const vec4& light) {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}


void setupMaterial()
{
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 25;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawTexturedSquare(float m = 1)
{
	//Square
	glBegin(GL_QUADS);

	glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);

	//upside down
	if (upsideDown)
	{
		glTexCoord2f(0, m - 0); glVertex3f(0, 0, 0);
		glTexCoord2f(m, m - 0); glVertex3f(1, 0, 0);
		glTexCoord2f(m, m - m); glVertex3f(1, 1, 0);
		glTexCoord2f(0, m - m); glVertex3f(0, 1, 0);
	}
	else
	{
		glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2f(m, 0); glVertex3f(1, 0, 0);
		glTexCoord2f(m, m); glVertex3f(1, 1, 0);
		glTexCoord2f(0, m); glVertex3f(0, 1, 0);
	}

	glEnd();
}

void
drawTexturedFloor(float size, float m)
{
	// Scaling
	glScalef(size, 1, size);

	// Square
	glBegin(GL_QUADS);

	// front
	glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);

	// Upside down
	glTexCoord2f(0, m);	glVertex3f(-0.5, 0, -0.5);
	glTexCoord2f(m, m);	glVertex3f(-0.5, 0, 0.5);
	glTexCoord2f(m, 0);	glVertex3f(0.5, 0, 0.5);
	glTexCoord2f(0, 0);	glVertex3f(0.5, 0, -0.5);

	glEnd();
}

void drawColoredTexturedCube()
{
	vec3	normal[6];
	vec3	color[6][4];
	vec3	vertex[6][4];
	vec2	texcoord[4];

	//Front
	normal[0] = vec3(0, 0, 1);
	color[0][0] = vec3(0, 0, 1); vertex[0][0] = vec3(0, 0, 1);
	color[0][1] = vec3(1, 0, 1); vertex[0][1] = vec3(1, 0, 1);
	color[0][2] = vec3(1, 1, 1); vertex[0][2] = vec3(1, 1, 1);
	color[0][3] = vec3(0, 1, 1); vertex[0][3] = vec3(0, 1, 1);

	//Back
	normal[1] = vec3(0, 0, -1);
	color[1][0] = vec3(1, 0, 0); vertex[1][0] = vec3(1, 0, 0);
	color[1][1] = vec3(0, 0, 0); vertex[1][1] = vec3(0, 0, 0);
	color[1][2] = vec3(0, 1, 0); vertex[1][2] = vec3(0, 1, 0);
	color[1][3] = vec3(1, 1, 0); vertex[1][3] = vec3(1, 1, 0);

	//Right
	normal[3] = vec3(1, 0, 0);
	color[3][0] = vec3(1, 0, 1); vertex[3][0] = vec3(1, 0, 1);
	color[3][1] = vec3(1, 0, 0); vertex[3][1] = vec3(1, 0, 0);
	color[3][2] = vec3(1, 1, 0); vertex[3][2] = vec3(1, 1, 0);
	color[3][3] = vec3(1, 1, 1); vertex[3][3] = vec3(1, 1, 1);

	//Left
	normal[2] = vec3(-1, 0, 0);
	color[2][0] = vec3(0, 0, 0); vertex[2][0] = vec3(0, 0, 0);
	color[2][1] = vec3(0, 0, 1); vertex[2][1] = vec3(0, 0, 1);
	color[2][2] = vec3(0, 1, 1); vertex[2][2] = vec3(0, 1, 1);
	color[2][3] = vec3(0, 1, 0); vertex[2][3] = vec3(0, 1, 0);

	//Top
	normal[4] = vec3(0, 1, 0);
	color[4][0] = vec3(0, 1, 1); vertex[4][0] = vec3(0, 1, 1);
	color[4][1] = vec3(1, 1, 1); vertex[4][1] = vec3(1, 1, 1);
	color[4][2] = vec3(1, 1, 0); vertex[4][2] = vec3(1, 1, 0);
	color[4][3] = vec3(0, 1, 0); vertex[4][3] = vec3(0, 1, 0);

	//Bottom
	normal[5] = vec3(0, -1, 0);
	color[5][0] = vec3(0, 0, 0); vertex[5][0] = vec3(0, 0, 0);
	color[5][1] = vec3(1, 0, 0); vertex[5][1] = vec3(1, 0, 0);
	color[5][2] = vec3(1, 0, 1); vertex[5][2] = vec3(1, 0, 1);
	color[5][3] = vec3(0, 0, 1); vertex[5][3] = vec3(0, 0, 1);

	//Texture coordinate
	texcoord[0] = vec2(0, 1);
	texcoord[1] = vec2(1, 1);
	texcoord[2] = vec2(1, 0);
	texcoord[3] = vec2(0, 0);

	// Cube
	for (int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texID[i % 4]);
		glBegin(GL_QUADS); glNormal3fv(value_ptr(normal[i])); for (int j = 0; j < 4; j++)
		{
			glColor3fv(value_ptr(color[i][j]));

			glTexCoord2fv(value_ptr(texcoord[j]));
			glVertex3fv(value_ptr(vertex[i][j]));
		}
		glEnd();

	}
}


void render(GLFWwindow* widnow)
{

	//background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	//Eye
	vec3 eye(0.0);
	vec3 at(0.0);
	switch (example)
	{
	case 1:
	case 2:
	case 3:
	case 4: eye = eye1; at = at1; break;
	case 5: eye = eye5; at = at5; break;
	case 6: eye = eye6; at = at6; break;
	}
	gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], upD[0], upD[1], upD[2]);

	//MSAA

	if (aaEnabled) glEnable(GL_MULTISAMPLE);
	else
	{
		glDisable(GL_MULTISAMPLE);
	}

	// Axes
	if (axes)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	//Lighting
	vec4 light(0.0);
	switch (example)
	{
	case 1:
	case 2:
	case 3:
	case 4: light = light1;  break;
	case 5: light = light5;  break;
	case 6: light = light6;  break;
	}

	setupLight(light);

	//Material
	setupMaterial();

	//Texture on/off
	if (texture) glEnable(GL_TEXTURE_2D);
	else	     glDisable(GL_TEXTURE_2D);

	//Draw
	//
	switch (example)
	{
	case 1://Checkerboard
	{
		glBindTexture(GL_TEXTURE_2D, texID[0]);
		drawTexturedSquare(textureNumRepeats);
	}
	break;
	case 2: // Demon
	{
		glBindTexture(GL_TEXTURE_2D, texID[1]);
		drawTexturedSquare(textureNumRepeats);
	}
	break;
	case 3: // Marble
	{
		glBindTexture(GL_TEXTURE_2D, texID[2]);	//	Marble	texture
		drawTexturedSquare(textureNumRepeats);
	}
	break;
	case 4: // Logo
	{
		glBindTexture(GL_TEXTURE_2D, texID[3]);	//	Logo	texture
		drawTexturedSquare(textureNumRepeats);
	}
	break;
	case 5: // Cube
	{
		float	theta1 = frame * 4 / period;
		vec3 axis(0, 1, 0);
		glRotatef(theta1, axis.x, axis.y, axis.z);
		drawColoredTexturedCube();
	}
	break;
	case 6: //Floor
	{
		glBindTexture(GL_TEXTURE_2D, texID[0]);
		drawTexturedFloor(floorSize, textureNumRepeats);
	}
	break;
	}

}
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key) { // Quit 
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
			// Play on/off				
		case GLFW_KEY_SPACE:	pause = !pause; break;

			// Axes on/off				
		case GLFW_KEY_X:	axes = !axes;	break;

			// checkBoard/Demon/Cube/Floor			
		case GLFW_KEY_B:	example = 1; textureNumRepeats = 1;	break;
		case GLFW_KEY_D:	example = 2; textureNumRepeats = 1;	break;
		case GLFW_KEY_M:	example = 3; textureNumRepeats = 1;	break;
		case GLFW_KEY_L:	example = 4; textureNumRepeats = 1;	break;
		case GLFW_KEY_C:	example = 5; textureNumRepeats = 1;	break;
		case GLFW_KEY_F:	example = 6; textureNumRepeats = 32; break;

			// Turn texture coordinates upside down		
		case GLFW_KEY_U:	upsideDown = !upsideDown; break;

			// Texture on/off				
		case GLFW_KEY_T:	texture = !texture; break;

			// Texture filter				
		case GLFW_KEY_1:	setTextureFilter(1);	break;
		case GLFW_KEY_2:	setTextureFilter(2);	break;
		case GLFW_KEY_3:	setTextureFilter(3);	break;
		case GLFW_KEY_4:	setTextureFilter(4);	break;
		case GLFW_KEY_5:	setTextureFilter(5);	break;
		case GLFW_KEY_6:	setTextureFilter(6);	break;
		case GLFW_KEY_7:	setTextureFilter(7);	break;
		case GLFW_KEY_8:	setTextureFilter(8);	break;

			//MSAA on/off
		case GLFW_KEY_A:	aaEnabled = !aaEnabled; break;
		}
	}
	// With the shfit key
	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && (mods & GLFW_MOD_SHIFT))
	{
		switch (key) {
		case GLFW_KEY_UP:	textureNumRepeats += 1; break;
		case GLFW_KEY_DOWN:	textureNumRepeats -= 1; break;
		}

	}

	// Without the shfit key
	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && !(mods & GLFW_MOD_SHIFT))
	{
		// Navigation for floor example
		if (example == 6)
		{
			switch (key)
			{
				// Navigation: move forward/backward
			case GLFW_KEY_UP: moveForward(0.5f); break;
			case GLFW_KEY_DOWN: moveForward(-0.5f); break;
				// Navigation: turn left/right
			case GLFW_KEY_LEFT:	turnForwardDirection(5.0f); break;
			case GLFW_KEY_RIGHT:	turnForwardDirection(-5.0f); break;
			}
		}
	}
}



