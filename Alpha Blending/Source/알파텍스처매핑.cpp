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
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);


bool readMesh(const char* filename);
void deleteMesh();

//Camera
vec3 eye(0, 0, 3);
vec3 up(0, 1, 0);
vec3 center(0, 0, 0);

//Light config
vec4 light(0.0, 0.0, 8, 1);

//Global coordinate frame

//Global coordinate frame
bool axes = true;
float AXIS_LENGTH = 1.25;
float AXIS_LINE_WIDTH = 2;

//Colors
GLfloat bgColor[4] = { 1,1,1,1 };

//Control
int selection = 1;
bool cube = false;
bool depthmask = false;

//Paly config
bool pause = true;

float timeStep = 1.0f / 120;
float period = 4.0;
//Current frame
int frame = 0;


//OpenGl texture unit
GLuint texID[7];

int main(int argc, char* argv[])
{
	vsync = 0;

	fovy = 37.8f;

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
	GLubyte checkerboard[64][64][3];

	for(int i = 0; i< 64; i++)
		for(int j = 0; j < 64; j++)
		{
			int c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0) ) * 255;
			checkerboard[i][j][0] = (GLubyte)c;
			checkerboard[i][j][1] = (GLubyte)c;
			checkerboard[i][j][2] = (GLubyte)c;
			
		}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 64, 64, 0,
		GL_RGB, GL_UNSIGNED_BYTE, checkerboard);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
}

bool loadAlphaTexture(const char* filename, int w, int h)
{
	//Open the raw texture file
	ifstream is(filename, ios::binary);
	if (is.fail())
	{
		cout << "can't open" << filename << endl;
		return false;
	}
	//	Allocate	memory
	GLubyte* raw = new GLubyte[w * h ];
	//	Read all	the texels
	is.read((char*)raw, w * h );
	if (!is)	cout << "Error:	only " << is.gcount() << "bytes could be read!" << endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
		GL_ALPHA, GL_UNSIGNED_BYTE, raw);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	Deallocate	memory
	delete[] raw;

	return true;
}

void init()
{
	glGenTextures(7, texID);

	//Texture
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadCheckerboardTexture();

	//Raw texture
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glPixelTransferf(GL_RED_BIAS, 1.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_logo.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[2]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 1.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_logo.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[3]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 1.0);
	loadAlphaTexture("m02_logo.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[4]);
	glPixelTransferf(GL_RED_BIAS, 1.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_grayscale_ornament.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[5]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 1.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_grayscale_ornament.raw", 512, 512); 

	glBindTexture(GL_TEXTURE_2D, texID[6]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 1.0);
	loadAlphaTexture("m02_grayscale_ornament.raw", 512, 512);

	//prepare mesh
	readMesh("m01_bunny.off");
	
	cout << endl;
	cout << "Keyboard: space for play/ pause" <<endl;
	cout << "Keyboard input : x for axes on/off" <<endl;
	cout << "Keyboard input : c for an opaque cube on/off "<<endl;
	cout << "Keyboard input : d for depth mask on/off"<<endl;
	cout <<endl;
	cout << "Keyboard 1 for a flat transparent bunny"<< endl;
	cout << "Keyboard 2 for a smooth transparent bunny"<<endl;
	cout << "Keyboard 3 for a flat transparent bunny with depth sorting" <<endl;
	cout << "Keyboard 4 for a smooth transparent bunny with depth sorting" <<endl;
	cout << "Keyboard 5 for a alpha-textured cube" <<endl;
}

void quit()
{
	deleteMesh();
}

void setupColoredMaterial(const vec4& color)
{
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 100;

	//Transparency
	mat_ambient[3] *= color[3];
	mat_specular[3] *= color[3];

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(color));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawTexturedCube()
{
	vec3 normal[6];
	vec3 vertex[6][4];
	vec2 texcoord[4];

	//Front
	normal[0] = vec3(0, 0, 1);
	vertex[0][0] = vec3(0, 0, 1);
	vertex[0][1] = vec3(1, 0, 1);
	vertex[0][2] = vec3(1, 1, 1);
	vertex[0][3] = vec3(0, 1, 1);
	//Back
	normal[1] = vec3(0, 0, -1);
	vertex[1][0] = vec3(1, 0, 0); 
	vertex[1][1] = vec3(0, 0, 0); 
	vertex[1][2] = vec3(0, 1, 0); 
	vertex[1][3] = vec3(1, 1, 0); 

	//Right
	normal[3] = vec3(1, 0, 0);
	vertex[3][0] = vec3(1, 0, 1); 
	vertex[3][1] = vec3(1, 0, 0); 
	vertex[3][2] = vec3(1, 1, 0); 
	vertex[3][3] = vec3(1, 1, 1); 

	//Left
	normal[2] = vec3(-1, 0, 0);
	vertex[2][0] = vec3(0, 0, 0); 
	vertex[2][1] = vec3(0, 0, 1); 
	vertex[2][2] = vec3(0, 1, 1); 
	vertex[2][3] = vec3(0, 1, 0); 

	//Top
	normal[4] = vec3(0, 1, 0);
	vertex[4][0] = vec3(0, 1, 1); 
	vertex[4][1] = vec3(1, 1, 1); 
	vertex[4][2] = vec3(1, 1, 0); 
	vertex[4][3] = vec3(0, 1, 0); 

	//Bottom
	normal[5] = vec3(0, -1, 0);
	vertex[5][0] = vec3(0, 0, 0); 
	vertex[5][1] = vec3(1, 0, 0); 
	vertex[5][2] = vec3(1, 0, 1); 
	vertex[5][3] = vec3(0, 0, 1); 

	//Texture coordinate
	texcoord[0] = vec2(0, 1);
	texcoord[1] = vec2(1, 1);
	texcoord[2] = vec2(1, 0);
	texcoord[3] = vec2(0, 0);

	// Cube
	for (int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texID[i + 1]);
		glBegin(GL_QUADS);
		glNormal3fv(value_ptr(normal[i]));
		for (int j = 0; j < 4; j++)
		{
			glTexCoord2fv(value_ptr(texcoord[j]));
			glVertex3fv(value_ptr(vertex[i][j]));
		}
		glEnd();

	}
}



//Read a mesh from a given OFF file
int nVertices = 0, nFaces = 0, nEdges = 0;

vec3* vertex = NULL;
vec3* vnormal = NULL;
vec3* fnormal = NULL;
int* face[3] = { NULL, NULL, NULL };

struct DepthSortData
{
	int i;
	vec4 center;
	float depth;
};

DepthSortData* fdsd = NULL;
DepthSortData* fcbe = NULL;

bool readMesh(const char* filename)
{
	ifstream is(filename);
	if (is.fail()) return false;

	char magicNumber[256];
	is >> magicNumber;

	is >> nVertices >> nFaces >> nEdges;
	cout << "# vertices = " << nVertices << endl;
	cout << "# faces = " << nFaces << endl;
	cout << "# edges = " << nEdges << endl;
	// Vertices
	vertex = new vec3[nVertices];
	for (int i = 0; i < nVertices; i++)
		is >> vertex[i].x >> vertex[i].y >> vertex[i].z;
	// Vertices
	vnormal = new vec3[nVertices];
	for (int i = 0; i < nVertices; i++)
		vnormal[i] = vec3(0, 0, 0);
	// Face normals
	fnormal = new vec3[nFaces];
	
	// Faces
	face[0] = new	int[nFaces];	// Only support triangles
	face[1] = new	int[nFaces];
	face[2] = new	int[nFaces];
	//Depth sort data
	fdsd = new DepthSortData[nFaces];
	
	int n;
	vec3 center;
	for (int i = 0; i < nFaces; i++) {
		is >> n >> face[0][i] >> face[1][i] >> face[2][i];
		if (n != 3) cout << "# vertices of the " << i << "-th faces = " << n << endl;

		//Normal vector of the face
		vec3 v1 = vertex[face[1][i]] - vertex[face[0][i]];
		vec3 v2 = vertex[face[2][i]] - vertex[face[0][i]];
		vec3 v = normalize(cross(v1, v2));

		fnormal[i] = v;

		//Add it to the normal vector of each vertex
		vnormal[face[0][i]] += v;
		vnormal[face[1][i]] += v;
		vnormal[face[2][i]] += v;

		//Depth sorting Data
		center = vertex[face[0][i]] / 3.0f;
		center += vertex[face[1][i]] / 3.0f;
		center += vertex[face[2][i]] / 3.0f;

		fdsd[i].i = i;
		fdsd[i].center = vec4(center, 1.0f);
		
	}
	// Normalization of the normal vectors

	for (int i = 0; i < nVertices; i++)
		vnormal[i] = normalize(vnormal[i]);

	return true;
}

void deleteMesh()
{
	if (vertex) { delete[] vertex; vertex = NULL; }
	if (vnormal) { delete[] vnormal; vnormal = NULL; }
	if (fnormal) { delete[] fnormal; fnormal = NULL; }
	if (face[0]) { delete[] face[0]; face[0] = NULL; }
	if (face[1]) { delete[] face[1]; face[1] = NULL; }
	if (face[2]) { delete[] face[2]; face[2] = NULL; }
	if (fdsd) { delete[] fdsd; fdsd = NULL; }
}

void drawFlatMesh()
{
	//Geometry
	glBegin(GL_TRIANGLES);
	for(int i = 0; i< nFaces; i++)
	{
		glNormal3fv(value_ptr(fnormal[i]));
		for (int j = 0; j < 3; j++)
			glVertex3fv(value_ptr(vertex[face[j][i]]));
	}
	glEnd();
	
}
void drawSmoothMesh()
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++)
		for(int j =0; j< 3; j++)
		{
			glNormal3fv(value_ptr(vnormal[face[j][i]]));
			glVertex3fv(value_ptr(vertex[face[j][i]]));
		}
	glEnd();
}

int compare(const void* a, const void* b)
{
	float depth1 = ((DepthSortData*)a)->depth;
	float depth2 = ((DepthSortData*)b)->depth;

	if (depth1 > depth2) return -1;
	if (depth1 < depth2) return 1;

	return 0;
}

void sortMeshFace()
{
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	mat4 M = make_mat4(modelView);
	for(int i =0; i < nFaces; i++)
	{
		vec4 center_eye = M * fdsd[i].center;

		fdsd[i].depth = -center_eye.z;
	}

	qsort(fdsd, nFaces, sizeof(DepthSortData), compare);
}

void sortCubeFace()
{
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	mat4 K = make_mat4(modelView);

	for(int i =0; i< 6; i++)
	{
		vec4 center_eye = K * fcbe[i].center;
		fcbe[i].depth = -center_eye.z;
	}
	qsort(fcbe, 6, sizeof(DepthSortData), compare);
}

void drawSortedFlatMesh()
{
	sortMeshFace();

	glBegin(GL_TRIANGLES);
	for(int i =0; i< nFaces; i++)
	{
		int iFace = fdsd[i].i;

		glNormal3fv(value_ptr(fnormal[iFace]));
		for (int j = 0; j < 3; j++)
			glVertex3fv(value_ptr(vertex[face[j][iFace]]));
	}
	glEnd();
}

void drawSortedSmoothMesh()
{
	sortMeshFace();

	glBegin(GL_TRIANGLES);
	for(int i = 0; i< nFaces; i++)
		for(int j=0; j< 3; j++)
		{
			int iFace = fdsd[i].i;

			glNormal3fv(value_ptr(vnormal[face[j][iFace]]));
			glVertex3fv(value_ptr(vertex[face[j][iFace]]));
			
		}
	glEnd();
}

void drawSortedTexturedCube()
{
	fcbe = new DepthSortData[6];
	vec3 normal[6];
	vec3 vertex[6][4];
	vec2 texcoord[4];

	//Front
	normal[0] = vec3(0, 0, 1);
	vertex[0][0] = vec3(0, 0, 1);
	vertex[0][1] = vec3(1, 0, 1);
	vertex[0][2] = vec3(1, 1, 1);
	vertex[0][3] = vec3(0, 1, 1);
	//Back
	normal[1] = vec3(0, 0, -1);
	vertex[1][0] = vec3(1, 0, 0);
	vertex[1][1] = vec3(0, 0, 0);
	vertex[1][2] = vec3(0, 1, 0);
	vertex[1][3] = vec3(1, 1, 0);

	//Right
	normal[3] = vec3(1, 0, 0);
	vertex[3][0] = vec3(1, 0, 1);
	vertex[3][1] = vec3(1, 0, 0);
	vertex[3][2] = vec3(1, 1, 0);
	vertex[3][3] = vec3(1, 1, 1);

	//Left
	normal[2] = vec3(-1, 0, 0);
	vertex[2][0] = vec3(0, 0, 0);
	vertex[2][1] = vec3(0, 0, 1);
	vertex[2][2] = vec3(0, 1, 1);
	vertex[2][3] = vec3(0, 1, 0);

	//Top
	normal[4] = vec3(0, 1, 0);
	vertex[4][0] = vec3(0, 1, 1);
	vertex[4][1] = vec3(1, 1, 1);
	vertex[4][2] = vec3(1, 1, 0);
	vertex[4][3] = vec3(0, 1, 0);

	//Bottom
	normal[5] = vec3(0, -1, 0);
	vertex[5][0] = vec3(0, 0, 0);
	vertex[5][1] = vec3(1, 0, 0);
	vertex[5][2] = vec3(1, 0, 1);
	vertex[5][3] = vec3(0, 0, 1);

	//Texture coordinate
	texcoord[0] = vec2(0, 1);
	texcoord[1] = vec2(1, 1);
	texcoord[2] = vec2(1, 0);
	texcoord[3] = vec2(0, 0);

	
	//Depth sorting Data
	vec3 center;
	
	for(int i=0; i<6; i++)
	{
		//Depth sorting Data
		center = vertex[i][0] / 4.0f;
		center += vertex[i][1] / 4.0f;
		center += vertex[i][2] / 4.0f;
		center += vertex[i][3] / 4.0f;
		
		fcbe[i].i = i;
		fcbe[i].center = vec4(center, 1.0f);
	}

	//sortfaces
	sortCubeFace();
	// Cube
	for (int i = 0; i < 6; i++)
	{
		int iFace = fcbe[i].i;
		glBindTexture(GL_TEXTURE_2D, texID[iFace + 1]);
		glBegin(GL_QUADS);

		
		
		glNormal3fv(value_ptr(normal[iFace]));
		for (int j = 0; j < 4; j++)
		{
			glTexCoord2fv(value_ptr(texcoord[j]));
			glVertex3fv(value_ptr(vertex[iFace][j]));
		}
		glEnd();

	}
	
}

void setupLight(const vec4& light) {
	

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 0.1f,0.1f, 0.1f,0.1f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

void drawTexturedQuad()
{
	glBindTexture(GL_TEXTURE_2D, texID[0]);

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);

	glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3f(1, 0, 0);
	glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
	glTexCoord2f(0, 1); glVertex3f(0, 1, 0);

	glEnd();
}

void render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	// depth buffer enableing
	glDepthMask(true);

	//Clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Modelvidw matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	if(axes)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	setupLight(light);

	glDepthMask(true);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glPushMatrix();
	{
		glScalef(2.5, 2.5, 1.0);
		glTranslatef(-0.5, -0.5, -1);

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		setupColoredMaterial(vec4(0.95, 0.95, 0.95, 1));
		drawTexturedQuad();
	}
	glPopMatrix();

	if(cube)
	{
		glPushMatrix();
		glTranslatef(-0.15f, 0.3f, 0.5f);
		glScalef(0.3f, 0.3f, 0.3f);

		vec3 axis(1, 0, 0);
		glRotatef(30, axis.x, axis.y, axis.z);

		glDisable(GL_TEXTURE_2D);

		setupColoredMaterial(vec4(0.5, 0.95, 0.5, 1));
		drawTexturedCube();

		glPopMatrix();
	}

	glDepthMask(depthmask);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	if(selection <= 4)
	{
		glPushMatrix();

		float theta = frame * 4 / period;
		vec3 axis(0, 1, 0);
		glRotatef(theta, axis.x, axis.y, axis.z);

		glTranslatef(0.0f, -0.2f, 0.0f);
		glScalef(0.7f, 0.7f, 0.7f);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		setupColoredMaterial(vec4(0.95, 0.95, 0.95, 0.5));
		switch (selection)
		{
		case 1: drawFlatMesh(); break;
		case 2: drawSmoothMesh(); break;
		case 3: drawSortedFlatMesh(); break;
		case 4: drawSortedSmoothMesh(); break;
		}

		glPopMatrix();
	}

	else
	{
		glPushMatrix();

		float theta = frame * 4 / period;
		vec3 axis(1, 1, 0);

		glRotatef(theta, axis.x, axis.y, axis.z);

		//Centering
		glTranslatef(-0.3f, -0.3f, -0.3f);
		glScalef(0.6f, 0.6f, 0.6f);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Draw

		setupColoredMaterial(vec4(0.95, 0.95, 0.95, 1));
		switch (selection) {
		case 5: drawTexturedCube(); break;
		case 6: drawSortedTexturedCube(); break;
		}
		

		glPopMatrix();
	}
}

void keyboard(GLFWwindow* window, int key, int code, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key)
		{
			// Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
			// Example selection
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		case GLFW_KEY_4: selection = 4; break;
		case GLFW_KEY_5: selection = 5; break;
		case GLFW_KEY_6: selection = 6; break;
			
			// Play on/off
		case GLFW_KEY_SPACE:	pause = !pause; break;

			//Axes on / off

		case GLFW_KEY_X: axes = !axes; break;
			//Opaque cube on/off
		case GLFW_KEY_C: cube = !cube; break;

		case GLFW_KEY_D: depthmask = !depthmask;
			if (depthmask) cout << "depthmask true" << endl;
			else
			{
				cout << "depthmask false" << endl;
			}break;

		}
	}
}
