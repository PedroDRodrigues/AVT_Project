//
// AVT: Phong Shading and Text rendered with FreeType library
// The text rendering was based on https://learnopengl.com/In-Practice/Text-Rendering
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
// 
// Author: Jo�o Madeiras Pereira
//

#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <chrono>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>

// Use Very Simple Libs
#include "include/geometry.h"
#include "include/VSShaderlib.h"
#include "include/AVTmathLib.h"
#include "include/VertexAttrDef.h"
#include "include/avtFreeType.h"
#include "include/meshUtils.h"

// our classes
#include "include/camera.h"
#include "include/scenery.h"
#include "include/boat.h"

using namespace std;

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2
#define DEG2RAD 3.14/180.0f

#define CAPTION "AVT Demo: Phong Shading and Text rendered with FreeType"
int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text

//File with the font
const string font_name = "fonts/arial.ttf";

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId[NUM_POINT_LIGHTS];
GLint sPos_uniformId[NUM_SPOT_LIGHTS];
GLint sDir_uniformId[NUM_SPOT_LIGHTS];
GLint sCut_uniformId[NUM_SPOT_LIGHTS];
GLint dPos_uniformId;
GLint tex_loc, tex_loc1, tex_loc2;

// Mouse Tracking Variables
int startX, startY, tracking = 0;
bool mouseMovingWhilePressed = false;
int lastMouseX = -1, lastMouseY = -1;
int timeoutDuration = 50;

// Camera Spherical Coordinates
float camAlpha = 39.0f, camBeta = 51.0f;
float r = 100.0f;

//camera declarations
Camera cams[3];
int activeCam = 2;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

float dirLightPos[4]{ 1.0f, 1000.0f, 1.0f, 0.0f };
float pointLightsPos[NUM_POINT_LIGHTS][4]= { {-35.0f, 4.0f, -35.0f, 1.0f},
					{-35.0f, 20.0f, -0.5f, 1.0f},
					{-1.0f, 20.0f, -1.0f, 1.0f},
					{1.0f, 20.0f, 1.0f, 1.0f},
					{35.0f, 20.0f, 0.5f, 1.0f},
					{1.5f, 20.0f, 1.5f, 1.0f}
};
float spotLightsPos[NUM_SPOT_LIGHTS][4] = { 
	{0.3, 0.2, 0.9, 1.0f},
	{- 0.3, 0.2, 0.9, 1.0f}
};

bool spot_trigger = true;
bool point_trigger = true;
bool direct_trigger = true;

// array of meshes
vector<struct MyMesh> myMeshes;
vector<struct MyModel> myModels;

float rotationSensitivity = 0.01f;
float zoomSensitivity = 0.10f;

float terrainSize = 100.0f;
float waterSize = 70.0f;

// boat object
Boat boat = Boat();

// to keep track of which keys are being pressed
bool keyStates[256] = { false };

// time variables
std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
float deltaTime = 0.0f;

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
    FrameCount = 0;
    glutTimerFunc(1000, timer, 0);
}

void refresh(int value)
{
	//renderScene();
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, refresh, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {
	float ratio;

	// Prevent a divide by zero, when window is too short
	if (h == 0)
		h = 1;

	// set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// set the projection matrix
	ratio = (1.0f * w) / h;
	loadIdentity(PROJECTION);

	//switch (activeCam) {
	//case 0:
	//	perspective(53.13f, ratio, 0.1f, 1000.0f);
	//	break;
	//case 1:
	//	ortho(-10.0f, 10.0f, -10.0f / ratio, 10.0f / ratio, 0.1f, 1000.0f);
	//	break;
	//}
	perspective(53.13f, ratio, 0.1f, 1000.0f); // VERIFICAR SE NÃO LEVA Switch(activeCam)
}

// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {

	auto currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
	lastTime = currentTime;

	GLint loc;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);

	// set the camera using a function similar to gluLookAt
	lookAt(
		cams[activeCam].camPos[0],
		cams[activeCam].camPos[1],
		cams[activeCam].camPos[2],
		cams[activeCam].camTarget[0],
		cams[activeCam].camTarget[1],
		cams[activeCam].camTarget[2],
		0, 1, 0
	);

	// use our shader

	glUseProgram(shader.getProgramIndex());

	//send the light position in eye coordinates
	//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

	float res[4];
	GLfloat pointlight_ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat pointlight_diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat pointlight_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	float counter = 0;

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {		
		multMatrixPoint(VIEW, pointLightsPos[i], res);   //lightPos definido em World Coord so is converted to eye space
		glUniform4fv(lPos_uniformId[i], 1, res);

		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("pointLightsPos[" + to_string(i) + "].materials.ambient").c_str()), (pointlight_ambient[0] + counter) * 0.1f, (pointlight_ambient[1] + counter) * 0.1f, (pointlight_ambient[2] + counter) * 0.1f, pointlight_ambient[3]);
		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("pointLightsPos[" + to_string(i) + "].materials.diffuse").c_str()), (pointlight_diffuse[0] + counter), (pointlight_diffuse[1] + counter), (pointlight_diffuse[2] + counter), pointlight_diffuse[3]);
		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("pointLightsPos[" + to_string(i) + "].materials.specular").c_str()), (pointlight_specular[0] + counter), (pointlight_specular[1] + counter), (pointlight_specular[2] + counter), pointlight_specular[3]);
		glUniform1f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("pointLightsPos[" + to_string(i) + "].materials.shininess").c_str()), 100.0f);
		counter += 0.1f;
		glLightfv(GL_LIGHT1 + i, GL_AMBIENT, pointlight_ambient);
		glLightfv(GL_LIGHT1 + i, GL_DIFFUSE, pointlight_diffuse);
		glLightfv(GL_LIGHT1 + i, GL_SPECULAR, pointlight_specular);
		glLightfv(GL_LIGHT1 + i, GL_POSITION, pointLightsPos[i]);
	}

	GLfloat spotLights_ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spotLights_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spotLights_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		multMatrixPoint(VIEW, spotLightsPos[i], res);
		glUniform4fv(sPos_uniformId[i], 1, res);

		float res_aux[4] = { boat.getPosition()[0], boat.getPosition()[1], boat.getPosition()[2], 1.0f };
		multMatrixPoint(VIEW, /*boat.getDirection()*/ res_aux, res); // DANGER
		glUniform4fv(sDir_uniformId[i], 1, res);
		glUniform1f(sCut_uniformId[i], 0.2f);

		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("spotLightsPos[" + to_string(i) + "].materials.ambient").c_str()), spotLights_ambient[0], spotLights_ambient[1], spotLights_ambient[2], spotLights_ambient[3]);
		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("spotLightsPos[" + to_string(i) + "].materials.diffuse").c_str()), spotLights_diffuse[0], spotLights_diffuse[1], spotLights_diffuse[2], spotLights_diffuse[3]);
		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("spotLightsPos[" + to_string(i) + "].materials.specular").c_str()), spotLights_specular[0], spotLights_specular[1], spotLights_specular[2], spotLights_specular[3]);
		glUniform1f(glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("spotLightsPos[" + to_string(i) + "].materials.shininess").c_str()), 100.0f);

		glLightfv(GL_LIGHT2 + i, GL_AMBIENT, spotLights_ambient);
		glLightfv(GL_LIGHT2 + i, GL_DIFFUSE, spotLights_diffuse);
		glLightfv(GL_LIGHT2 + i, GL_SPECULAR, spotLights_specular);
		glLightfv(GL_LIGHT2 + i, GL_POSITION, spotLightsPos[i]);
	}

	multMatrixPoint(VIEW, dirLightPos, res);
	glUniform4fv(dPos_uniformId, 1, res);
	GLfloat dirLight_ambient[4] = {0.3f, 0.24f, 0.14f, 1.0f};
	GLfloat dirLight_diffuse[4] = {0.7f, 0.42f, 0.26f, 1.0f};
	GLfloat dirLight_specular[4]= {0.5f, 0.5f, 0.5f, 1.0f};
	glUniform4f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.ambient"), dirLight_ambient[0], dirLight_ambient[1], dirLight_ambient[2], dirLight_ambient[3]);
	glUniform4f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.diffuse"), dirLight_diffuse[0], dirLight_diffuse[1], dirLight_diffuse[2], dirLight_diffuse[3]);
	glUniform4f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.specular"), dirLight_specular[0], dirLight_specular[1], dirLight_specular[2], dirLight_specular[3]);
	glUniform1f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.shininess"), 100.0f);

	glLightfv(GL_LIGHT0, GL_AMBIENT, dirLight_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dirLight_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, dirLight_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, dirLightPos);	
	
	int objId = 0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh

	for (int i = 0; i < myMeshes.size(); i++) {

		MyMesh currMesh = myMeshes[i];

		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, currMesh.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, currMesh.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, currMesh.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, currMesh.mat.shininess);

		pushMatrix(MODEL);
		if (currMesh.name == "terrain") {
			rotate(MODEL, -90.0f, 1.0f, 0.0f, 0.0f);
		}
		else if (currMesh.name == "water") {
			rotate(MODEL, -90.0f, 1.0f, 0.0f, 0.0f);
			translate(MODEL, 0.0f, 0.0f, 0.01f);
		}
		else if (currMesh.name == "house") {
			translate(MODEL, currMesh.xPosition, 0.0f, currMesh.yPosition);
		}

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(currMesh.vao);

		glDrawElements(currMesh.type, currMesh.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
		//objId++;
	}

	for (int i = 0; i < myModels.size(); i++) {
		MyModel currModel = myModels[i];

		pushMatrix(MODEL);

		if (currModel.name == "boat") {
			boat.render(MODEL);
			boat.update(deltaTime);
			cams[activeCam].followBoat(boat.getPosition(), boat.getDirection(), activeCam != 2, mouseMovingWhilePressed);
			cams[2].computeCameraAngles();
		}

		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		glBindVertexArray(currModel.VAO);
		glDrawElements(GL_TRIANGLES, currModel.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
	}

	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	//the glyph contains transparent background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	// viewer at origin looking down at  negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);

	int ratio = m_viewport[2] - m_viewport[0] / (m_viewport[3] - m_viewport[1]);
	if (cams[activeCam].type == PERSPECTIVE) {
		perspective(53.13f, ratio, 1, 100);
	} else {
		ortho(ratio * -25, ratio * 25, -25, 25, 0.1, 100);
	}

	pushMatrix(VIEW);
	loadIdentity(VIEW);
	RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	RenderText(shaderText, "AVT Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{

	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	int ratio = m_viewport[2] - m_viewport[0] / (m_viewport[3] - m_viewport[1]);

	keyStates[key] = true;

	switch(key) {
		case '1': // orthogonal top
			printf("%s\n", "orthogonal top");
			activeCam = 0;
			break;
		case '2': // perspective top
			printf("%s\n", "perspective top");
			activeCam = 1;

			break;
		case '3': // perspective follow
			printf("%s\n", "perspective follow");
			activeCam = 2;
			break;

		case 'C': 
			printf("Camera Spherical Coordinates (%f, %f, %f)\n", camAlpha, camBeta, r);
			
			if (point_trigger == false) {
				for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
					glEnable(GL_LIGHT1 + i);
				}
			
				point_trigger = true;
			}
		
			if (point_trigger == true) {
				for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
					glDisable(GL_LIGHT1 + i);
				}
				
				point_trigger = false;
			}
			break;

		case 'H':
			// toggle spot lights
			if (spot_trigger == true) {
				for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
					glDisable(GL_LIGHT2 + i);
				}
				spot_trigger = false;
			}
			else {
				for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
					glEnable(GL_LIGHT2 + i);
				}
				spot_trigger = true;
			}
			break;

		case 'N':
			// toggle directional light - NIGHT MODE / DAY MODE
			if (direct_trigger == true) {
				glDisable(GL_LIGHT0);
				direct_trigger = false;
			}
			else {
				glEnable(GL_LIGHT0);
				direct_trigger = true;
			}
			break;

		case 'm': glEnable(GL_MULTISAMPLE); break;
		case 'n': glDisable(GL_MULTISAMPLE); break;

		case 'a':
		case 'A':
			if (keyStates['s']) boat.paddleBackwardLeft();
			else boat.paddleLeft();
			break;

		case 'd':
		case 'D':
			if (keyStates['s']) boat.paddleBackwardRight();
			else boat.paddleRight();
			break;

		case 'o':
		case 'O':
			boat.toggleTurboMode();
			break;

		case 27:
			glutLeaveMainLoop();
			break;
	}
}

void releaseKey(unsigned char key, int xx, int yy) {
	keyStates[key] = false;
}

// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	if (activeCam != 2) {
		return;
	}

	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;

		if (button == GLUT_LEFT_BUTTON) {
			tracking = 1;
		}
		//else if (button == GLUT_RIGHT_BUTTON)
		//	tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		//if (tracking == 1) {
		//	alpha -= (xx - startX);
		//	beta += (yy - startY);
		//}
		//else if (tracking == 2) {
		//	r += (yy - startY) * 0.01f;
		//	if (r < 0.1f)
		//		r = 0.1f;
		//}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{
	mouseMovingWhilePressed = true;
	lastMouseX = xx;
	lastMouseY = yy;

	if (activeCam != 2) {
		return;
	}

	if (tracking == 1) {
		int deltaX = -(xx - startX);
		int deltaY = yy - startY;

		float sensitivity = 0.05f;


		cams[activeCam].yaw += deltaX * sensitivity;
		cams[activeCam].pitch += deltaY * sensitivity;

		if (cams[activeCam].pitch > 89.0f) cams[activeCam].pitch = 89.0f;
		if (cams[activeCam].pitch < -89.0f) cams[activeCam].pitch = -89.0f;

		std::array<float, 3> direction;
		direction[0] = cos(DEG2RAD * cams[activeCam].yaw) * cos(DEG2RAD * cams[activeCam].pitch);
		direction[1] = sin(DEG2RAD * cams[activeCam].pitch);
		direction[2] = sin(DEG2RAD * cams[activeCam].yaw) * cos(DEG2RAD * cams[activeCam].pitch);

		cams[activeCam].updateTarget(direction[0], direction[1], direction[2]);

		startX = xx;
		startY = yy;
	}
}

void checkMouseStopped(int value) {
	static int lastCheckX = -1, lastCheckY = -1;

	if (lastMouseX == lastCheckX && lastMouseY == lastCheckY && !tracking) {
		mouseMovingWhilePressed = false;
	}

	lastCheckX = lastMouseX;
	lastCheckY = lastMouseY;

	glutTimerFunc(timeoutDuration, checkMouseStopped, 0);
}


// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight_gouraud.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight_gouraud.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());
	printf("InfoLog for Model Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	if (!shader.isProgramValid()) {
		printf("GLSL Model Program Not Valid!\n");
		printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());
		exit(1);
	}

	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	//lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		lPos_uniformId[i] = glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("pointLightsPos[" + to_string(i) + "].position").c_str());
	}
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		sPos_uniformId[i] = glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("spotLightsPos[" + to_string(i) + "].position").c_str());
		sDir_uniformId[i] = glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("spotLightsPos[" + to_string(i) + "].direction").c_str());
		sCut_uniformId[i] = glGetUniformLocation(shader.getProgramIndex(), (const GLchar*)("spotLightsPos[" + to_string(i) + "].cutoff").c_str());
	}
	dPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "dirLight.direction");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	
	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());


	if (!shaderText.isProgramValid()) {
		printf("GLSL Text Program Not Valid!\n");
		exit(1);
	}
	
	return(shader.isProgramLinked() && shaderText.isProgramLinked());
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void init()
{
	MyMesh amesh;

	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	/// Initialization of freetype library with font_name file
	freeType_init(font_name);

	// set the camera position based on its spherical coordinates
	//cams[activeCam].updatePosition(camAlpha, camBeta, r);

	// top view cameras

	cams[0].camPos[0] = 0.01f;
	cams[0].camPos[1] = 100.0f;
	cams[0].camPos[2] = 0.01f;
	cams[0].type = ORTHOGONAL;

	cams[1].camPos[0] = 0.01f;
	cams[1].camPos[1] = 100.0f;
	cams[1].camPos[2] = 0.01f;

	createTerrainMesh(terrainSize);
	createWaterMesh(waterSize);
	createHouseMeshes(50, terrainSize, waterSize);

	boat.createMesh();

	cams[2].followBoat(boat.getPosition(), boat.getDirection(), activeCam != 2, tracking == 1);
	cams[2].computeCameraAngles();

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

	//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (4, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


	//  Callback Registration
	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);


	glutTimerFunc(0, timer, 0);
	glutTimerFunc(0, refresh, 0);    //use it to to get 60 FPS whatever
	//glutIdleFunc(renderScene);  // Use it for maximum performance

	//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(releaseKey);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutTimerFunc(timeoutDuration, checkMouseStopped, 0);
	//glutMouseWheelFunc ( mouseWheel ) ;
	

	//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}



