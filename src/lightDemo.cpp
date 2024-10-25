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

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/scene.h"

// Use Very Simple Libs
#include "include/geometry.h"
#include "include/VSShaderlib.h"
#include "include/AVTmathLib.h"
#include "include/VertexAttrDef.h"
#include "include/avtFreeType.h"
#include "include/meshUtils.h"
#include "include/Texture_Loader.h"

// our classes
#include "include/camera.h"
#include "include/scenery.h"
#include "include/boat.h"
#include "include/creature.h"
#include "include/globals.h"
#include "include/utils.h"
#include "include/l3dBillboard.h"
#include "include/meshFromAssimp.h"
#include "include/flare.h"

using namespace std;

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2
#define DEG2RAD 3.14/180.0f

#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICULAS  1500

#define CAPTION "AVT Demo: Phong Shading and Text rendered with FreeType"

inline double clamp(const double x, const double min, const double max) {
	return (x < min ? min : (x > max ? max : x));
}

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}

// Create an instance of the Importer class
Assimp::Importer importer;

// the global Assimp scene object
const aiScene* scene;

// scale factor for the Assimp model to fit in the window
float scaleFactor;


char model_dir[50];  //initialized by the user input at the console

//Array of meshes 
vector<vector<struct MyMesh>> myMeshes1; //meshes array for the spider model

//Array of Texture Objects
GLuint* textureIds;  //for the input model

int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text

bool normalMapKey = TRUE;
bool flareEffect = TRUE;

//File with the font
const string font_name = "fonts/arial.ttf";

// array of meshes
vector<struct MyMesh> myMeshes;
vector<struct MyModel> myModels;
vector<struct Creature> creatures;

struct AABB {
	float min[3];
	float max[3];
};

float rotationSensitivity = 0.01f;
float zoomSensitivity = 0.10f;

float terrainSize = 200.0f;
float waterSize = 120.0f;

// boat object
Boat boat = Boat();

MyMesh skybox;

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
GLint tex_loc, tex_loc1, tex_loc2, tex_sphereMap_loc, tex_normalMap_loc, tex_cube_loc;
GLint texMode_uniformId, shadowMode_uniformId;
GLint model_uniformId;

GLint normalMap_loc;
GLint specularMap_loc;
GLint diffMapCount_loc;

FLARE_DEF AVTflare;
float lightScreenPos[3];


GLuint TextureArray[8];
GLuint FlareTextureArray[5];
//GLuint FlareTextureArray[5];

//FLARE_DEF AVTflare;

//float lightScreenPos[3];  //Position of the light in Window Coordinates

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

// fog stuff
std::array<float, 4> fogColor = { 0.5f, 0.5f, 0.5f, 1.0f };
float fogStart = 10.0f;
float fogDensity = 0.01f;
GLuint fogColorLoc;
GLuint fogStartLoc;
GLuint fogDensityLoc;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];
//float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

float dirLightPos[4]{ 600.0f, 1000.0f, 600.0f, 0.0f };
float pointLightsPos[NUM_POINT_LIGHTS][4] = { {0.0f, 4.0f, 0.0f, 1.0f},
					{-35.0f, 20.0f, -25.0f, 1.0f},
					{-100.0f, 20.0f, -10.0f, 1.0f},
					{100.0f, 20.0f, 100.0f, 1.0f},
					{35.0f, 20.0f, 90.0f, 1.0f},
					{80.0f, 20.0f, 80.0f, 1.0f}
};
float spotLightsPos[NUM_SPOT_LIGHTS][4] = {
	{boat.getPosition()[0] + 0.3, boat.getPosition()[1] + 0.2, boat.getPosition()[2] + 0.9, 1.0f},
	{boat.getPosition()[0] - 0.3, boat.getPosition()[1] + 0.2, boat.getPosition()[2] + 0.9, 1.0f}
};

bool spot_trigger = true;
bool point_trigger = true;
bool direct_trigger = true;

// to keep track of which keys are being pressed
bool keyStates[256] = { false };

// time variables
std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
float deltaTime = 0.0f;
float seconds = 0.0f;

bool isPaused = false;
bool gameOver = false;
int lives = 5;
int points = 0;

std::chrono::high_resolution_clock::time_point startTime, currentTime;
float elapsedTime = 0.0f;

// save the state of the boat : collisiding - true, not colliding - false
bool boatColliding = false;
//bool flareEffect = false;

// fireworks particles
int fireworks = 0;
int type = 0;

typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posi��o
	GLfloat vx, vy, vz; // velocidade 
	GLfloat ax, ay, az; // acelera��o
} Particle;

Particle particula[MAX_PARTICULAS];
int dead_num_particles = 0;

void updateParticles()
{
	int i;
	float h;

	/* M�todo de Euler de integra��o de eq. diferenciais ordin�rias
	h representa o step de tempo; dv/dt = a; dx/dt = v; e conhecem-se os valores iniciais de x e v */

	//h = 0.125f;
	h = 0.033;
	if (fireworks) {

		for (i = 0; i < MAX_PARTICULAS; i++)
		{
			particula[i].x += (h * particula[i].vx);
			particula[i].y += (h * particula[i].vy);
			particula[i].z += (h * particula[i].vz);
			particula[i].vx += (h * particula[i].ax);
			particula[i].vy += (h * particula[i].ay);
			particula[i].vz += (h * particula[i].az);
			particula[i].life -= particula[i].fade;
		}
	}
}



unsigned int getTextureId(char* name) {
	int i;

	for (i = 0; i < NTEXTURES; ++i)
	{
		if (strncmp(name, flareTextureNames[i], strlen(name)) == 0)
			return i;
	}
	return -1;
}

void    loadFlareFile(FLARE_DEF* flare, char* filename)
{
	int     n = 0;
	FILE* f;
	char    buf[256];
	int fields;

	memset(flare, 0, sizeof(FLARE_DEF));

	f = fopen(filename, "r");
	if (f)
	{
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%f %f", &flare->fScale, &flare->fMaxSize);

		while (!feof(f))
		{
			char            name[8] = { '\0', };
			double          dDist = 0.0, dSize = 0.0;
			float			color[4];
			int				id;

			fgets(buf, sizeof(buf), f);
			fields = sscanf(buf, "%4s %lf %lf ( %f %f %f %f )", name, &dDist, &dSize, &color[3], &color[0], &color[1], &color[2]);
			if (fields == 7)
			{
				for (int i = 0; i < 4; ++i) color[i] = clamp(color[i] / 255.0f, 0.0f, 1.0f);
				id = getTextureId(name);
				if (id < 0) printf("Texture name not recognized\n");
				else
					flare->element[n].textureId = id;
				flare->element[n].fDistance = (float)dDist;
				flare->element[n].fSize = (float)dSize;
				memcpy(flare->element[n].matDiffuse, color, 4 * sizeof(float));
				++n;
			}
		}

		flare->nPieces = n;
		fclose(f);
	}
	else printf("Flare file opening error\n");
}


void iniParticles(void)
{
	GLfloat v, theta, phi;
	int i;

	for (i = 0; i < MAX_PARTICULAS; i++)
	{
		v = 0.8 * frand() + 0.2;
		phi = frand() * M_PI;
		theta = 2.0 * frand() * M_PI;

		particula[i].x = boat.getPosition()[0];
		particula[i].y = 10.0f;
		particula[i].z = boat.getPosition()[2];
		particula[i].vx = v * cos(theta) * sin(phi);
		particula[i].vy = v * cos(phi);
		particula[i].vz = v * sin(theta) * sin(phi);
		particula[i].ax = 0.05f; /* simular um pouco de vento */
		particula[i].ay = -0.10f; /* simular a acelera��o da gravidade */
		particula[i].az = 0.0f;

		/* tom amarelado que vai ser multiplicado pela textura que varia entre branco e preto */
		particula[i].r = 0.882f;
		particula[i].g = 0.552f;
		particula[i].b = 0.211f;

		particula[i].life = 1.0f;			/* vida inicial */
		particula[i].fade = 0.0025f;	    /* step de decr�scimo da vida para cada itera��o */
	}
}

AABB calculateAABBFromCreatures(Creature creature) {
	AABB aabb;

	aabb.min[0] = creature.x - creature.min_pos_vert[0];
	aabb.min[1] = creature.y - creature.min_pos_vert[1];
	aabb.min[2] = creature.z - creature.min_pos_vert[2];

	aabb.max[0] = creature.x + creature.max_pos_vert[0];
	aabb.max[1] = creature.y + creature.max_pos_vert[1];
	aabb.max[2] = creature.z + creature.max_pos_vert[2];

	return aabb;
}

// --------------------------

AABB calculateAABBFromBoat(Boat boat) {
	AABB aabb;

	aabb.min[0] = boat.getPosition()[0] - boat.getMinPosVert()[0];
	aabb.min[1] = boat.getPosition()[1] - boat.getMinPosVert()[1];
	aabb.min[2] = boat.getPosition()[2] - boat.getMinPosVert()[2];

	aabb.max[0] = boat.getPosition()[0] + boat.getMaxPosVert()[0];
	aabb.max[1] = boat.getPosition()[1] + boat.getMaxPosVert()[1];
	aabb.max[2] = boat.getPosition()[2] + boat.getMaxPosVert()[2];

	return aabb;
}

// --------------------------

AABB calculateAABBFromMesh(MyMesh mesh) {
	AABB aabb;
	aabb.min[0] = mesh.xPosition - mesh.min_pos_vert[0];
	aabb.min[1] = mesh.yPosition - mesh.min_pos_vert[1];
	aabb.min[2] = mesh.zPosition - mesh.min_pos_vert[2];

	aabb.max[0] = mesh.xPosition + mesh.max_pos_vert[0];
	aabb.max[1] = mesh.yPosition + mesh.max_pos_vert[1];
	aabb.max[2] = mesh.zPosition + mesh.max_pos_vert[2];

	return aabb;
}

// --------------------------

bool hasCollision(const AABB& aabb1, const AABB& aabb2) {
	// Check for separation along the x-axis
	if (aabb1.min[0] > aabb2.max[0] || aabb1.max[0] < aabb2.min[0]) {
		return false;
	}

	// Check for separation along the y-axis
	if (aabb1.min[1] > aabb2.max[1] || aabb1.max[1] < aabb2.min[1]) {
		return false;
	}

	// Check for separation along the z-axis
	if (aabb1.min[2] > aabb2.max[2] || aabb1.max[2] < aabb2.min[2]) {
		return false;
	}

	// If there is no separation along any axis, the AABBs collide
	return true;
}

/* Check boat's collision with water creatures - AABB */

void checkCollisionCreatures(Creature creature, Boat boat) {

	if (boatColliding) {
		return;
	}

	AABB boatAABB = calculateAABBFromBoat(boat);
	AABB creatureAABB = calculateAABBFromCreatures(creature);

	// check if the boat collides with the creature
	if (hasCollision(boatAABB, creatureAABB)) {

		if (lives == 1) {
			gameOver = true;
		}
		else if (lives > 1) {
			boatColliding = true;
			boat.stop();
		}
		else {
			printf("\n Error on score: lives number if not allowed.\n");
		}

		lives = lives - 1;
	}
}

/* Check collisions with MyMeshes objects */

void checkCollisionMeshes(MyMesh mesh, Boat boat) {

	AABB boatAABB = calculateAABBFromBoat(boat);
	AABB meshAABB = calculateAABBFromMesh(mesh);

	// check if the boat collides with the mesh
	if (hasCollision(boatAABB, meshAABB) && (mesh.name == "house" || mesh.name == "obstacle")) {
		// move slightly the mesh to avoid collision
		while (hasCollision(boatAABB, meshAABB)) {
			boat.stop();
			mesh.xPosition += 0.5f;
			mesh.zPosition += 0.5f;
			meshAABB = calculateAABBFromMesh(mesh);
		}
	}
}

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

	perspective(53.13f, ratio, 0.1f, 1000.0f);
}


void renderHUD() {

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	glUseProgram(shaderText.getProgramIndex());

	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);

	if (gameOver) {
		RenderText(shaderText, "Game Over", 10.0f, 80.0f, 0.5f, 1.0f, 1.0f, 1.0f);
		RenderText(shaderText, "Press 'R' to restart", 10.0f, 110.0f, 0.5f, 1.0f, 1.0f, 1.0f);
	}
	else if (isPaused) {
		RenderText(shaderText, "Paused", 10.0f, 80.0f, 0.5f, 1.0f, 1.0f, 1.0f);
		RenderText(shaderText, "Press 'P' to resume", 10.0f, 110.0f, 0.5f, 1.0f, 1.0f, 1.0f);
	}

	RenderText(shaderText, "Lives: " + to_string(lives), 10.0f, 20.0f, 0.5f, 1.0f, 1.0f, 1.0f);
	RenderText(shaderText, "Elapsed Time: " + to_string(elapsedTime), 10.0f, 50.0f, 0.5f, 1.0f, 1.0f, 1.0f);

	popMatrix(MODEL);
	popMatrix(PROJECTION);
	popMatrix(VIEW);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}


// ------------------------------------------------------------
//
// Render stuff
//

// Recursive render of the Assimp Scene Graph

void aiRecursive_render(const aiNode* nd, vector<struct MyMesh>& myMeshes, GLuint*& textureIds)
{
	GLint loc;

	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;
	// OpenGL matrices are column major
	m.Transpose();

	// save model matrix and apply node transformation
	pushMatrix(MODEL);

	float aux[16];
	memcpy(aux, &m, sizeof(float) * 16);
	multMatrix(MODEL, aux);


	// draw all meshes assigned to this node
	for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {

		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.emissive");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.emissive);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes[nd->mMeshes[n]].mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, myMeshes[nd->mMeshes[n]].mat.texCount);

		unsigned int  diffMapCount = 0;  //read 2 diffuse textures

		//devido ao fragment shader suporta 2 texturas difusas simultaneas, 1 especular e 1 normal map

		glUniform1i(normalMap_loc, false);   //GLSL normalMap variable initialized to 0
		glUniform1i(specularMap_loc, false);
		glUniform1ui(diffMapCount_loc, 0);

		if (myMeshes[nd->mMeshes[n]].mat.texCount != 0)
			for (unsigned int i = 0; i < myMeshes[nd->mMeshes[n]].mat.texCount; ++i) {

				//Activate a TU with a Texture Object
				GLuint TU = myMeshes[nd->mMeshes[n]].texUnits[i];
				glActiveTexture(GL_TEXTURE5 + TU);
				glBindTexture(GL_TEXTURE_2D, textureIds[TU]);

				if (myMeshes[nd->mMeshes[n]].texTypes[i] == DIFFUSE) {
					if (diffMapCount == 0) {
						diffMapCount++;
						loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitDiff");
						glUniform1i(loc, TU);
						glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else if (diffMapCount == 1) {
						diffMapCount++;
						loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitDiff1");
						glUniform1i(loc, TU);
						glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else printf("Only supports a Material with a maximum of 2 diffuse textures\n");
				}
				else if (myMeshes[nd->mMeshes[n]].texTypes[i] == SPECULAR) {
					loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitSpec");
					glUniform1i(loc, TU);
					glUniform1i(specularMap_loc, true);
				}
				else if (myMeshes[nd->mMeshes[n]].texTypes[i] == NORMALS) { //Normal map
					loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitNormalMap");
					if (normalMapKey)
						glUniform1i(normalMap_loc, normalMapKey);
					glUniform1i(loc, TU);

				}
				else printf("Texture Map not supported\n");
			}

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// bind VAO
		glBindVertexArray(myMeshes[nd->mMeshes[n]].vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		// draw
		glDrawElements(myMeshes[nd->mMeshes[n]].type, myMeshes[nd->mMeshes[n]].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// draw all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		aiRecursive_render(nd->mChildren[n], myMeshes, textureIds);
	}
	popMatrix(MODEL);
}


// ------------------------------------------------------------
//
// Render stuff
//

void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport) {  //lx, ly represent the projected position of light on viewport

	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int screenMaxCoordX = m_viewport[0] + m_viewport[2] - 1;
	int screenMaxCoordY = m_viewport[1] + m_viewport[3] - 1;

	//viewport center
	cx = m_viewport[0] + (int)(0.5f * (float)m_viewport[2]) - 1;
	cy = m_viewport[1] + (int)(0.5f * (float)m_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt(cx * cx + cy * cy);
	flaredist = sqrt((lx - cx) * (lx - cx) + (ly - cy) * (ly - cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(m_viewport[2] * flare->fMaxSize);
	flarescale = (int)(m_viewport[2] * flare->fScale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - lx), m_viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - ly), m_viewport[1], screenMaxCoordY);

	// Render each element. To be used Texture Unit 0

	glUniform1i(texMode_uniformId, 4); // draw modulated textured particles 
	glUniform1i(tex_loc, 0);  //use TU 0

	for (i = 0; i < flare->nPieces; ++i)
	{
		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - flare->element[i].fDistance) * lx + flare->element[i].fDistance * dx);
		py = (int)((1.0f - flare->element[i].fDistance) * ly + flare->element[i].fDistance * dy);
		px = clampi(px, m_viewport[0], screenMaxCoordX);
		py = clampi(py, m_viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * flare->element[i].fSize);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)m_viewport[3] / (float)m_viewport[2] * (float)width);
		memcpy(diffuse, flare->element[i].matDiffuse, 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel

		if (width > 1)
		{
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FlareTextureArray[flare->element[i].textureId]);
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(myMeshes[65].vao);
			glDrawElements(myMeshes[65].type, myMeshes[65].numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void renderTrees() {

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 70, 0, 70);
	aiRecursive_render(scene->mRootNode, myMeshes1[0], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -70, 0, -70);
	aiRecursive_render(scene->mRootNode, myMeshes1[1], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 70, 0, -70);
	aiRecursive_render(scene->mRootNode, myMeshes1[2], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -70, 0, 70);
	aiRecursive_render(scene->mRootNode, myMeshes1[3], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 70, 0, 50);
	aiRecursive_render(scene->mRootNode, myMeshes1[0], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -70, 0, -50);
	aiRecursive_render(scene->mRootNode, myMeshes1[1], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 70, 0, -50);
	aiRecursive_render(scene->mRootNode, myMeshes1[2], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -70, 0, 50);
	aiRecursive_render(scene->mRootNode, myMeshes1[3], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 70, 0, 70);
	aiRecursive_render(scene->mRootNode, myMeshes1[0], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -70, 0, -70);
	aiRecursive_render(scene->mRootNode, myMeshes1[1], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 70, 0, -70);
	aiRecursive_render(scene->mRootNode, myMeshes1[2], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -70, 0, 70);
	aiRecursive_render(scene->mRootNode, myMeshes1[3], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 50, 0, 70);
	aiRecursive_render(scene->mRootNode, myMeshes1[0], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -50, 0, -70);
	aiRecursive_render(scene->mRootNode, myMeshes1[1], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, 50, 0, -70);
	aiRecursive_render(scene->mRootNode, myMeshes1[2], textureIds);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	translate(MODEL, -50, 0, 70);
	aiRecursive_render(scene->mRootNode, myMeshes1[3], textureIds);
	popMatrix(MODEL);
}

void renderTerrain() {

	float pos[3], right[3], up[3];
	int objId = 0;
	GLint loc;

	for (int i = 0; i < 2; i++) {

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
			glUniform1i(texMode_uniformId, 2);
		}
		else if (currMesh.name == "water") {
			rotate(MODEL, -90.0f, 1.0f, 0.0f, 0.0f);
			translate(MODEL, 0.0f, 0.0f, 0.05f);
			glUniform1i(texMode_uniformId, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureArray[0]);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, TextureArray[2]);

			glUniform1i(tex_loc, 0);
			glUniform1i(tex_loc2, 2);


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
	}
}


void renderObjects() {

	float pos[3], right[3], up[3];
	int objId = 0;
	GLint loc;

	glUniform1i(texMode_uniformId, 3);

	renderTrees();

	glUniform1i(texMode_uniformId, 0);

	for (int i = 2; i < myMeshes.size(); i++) {

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
		if (currMesh.name == "house") {
			glUniform1i(texMode_uniformId, 0);
			translate(MODEL, currMesh.xPosition, currMesh.yPosition, currMesh.zPosition);

		}
		else if (currMesh.name == "obstacle") {
			glUniform1i(texMode_uniformId, 6);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureArray[5]);

			translate(MODEL, currMesh.xPosition, currMesh.yPosition, currMesh.zPosition);
		}
		glUniform1i(tex_loc, 0);

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

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	for (int i = 0; i < myModels.size(); i++) {
		MyModel currModel = myModels[i];

		pushMatrix(MODEL);

		if (currModel.name == "boat") {
			GLfloat boatAmbient[] = { 0.3f, 0.15f, 0.05f, 1.0f };
			GLfloat boatDiffuse[] = { 0.6f, 0.5f, 0.3f, 1.0f };
			GLfloat boatSpecular[] = { 0.1f, 0.05f, 0.025f, 1.0f };
			GLfloat boatShininess = 10.0f;

			GLint loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, boatAmbient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, boatDiffuse);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, boatSpecular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, boatShininess);

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

		glUniform1i(texMode_uniformId, 0);
		glBindVertexArray(currModel.VAO);
		glDrawElements(GL_TRIANGLES, currModel.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
	}

	glDepthMask(GL_FALSE);

	for (int i = 0; i < creatures.size(); i++) {
		Creature& currCreature = creatures[i];

		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, currCreature.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, currCreature.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, currCreature.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, currCreature.mat.shininess);

		currCreature.update(deltaTime, creatureSpeedMultiplier, creatureMaxDistance, creatureRadius);
		currCreature.applyShakeAnimation(seconds, creatureShakeAmplitude);

		pushMatrix(MODEL);

		translate(
			MODEL,
			currCreature.x,
			currCreature.y,
			currCreature.z
		);

		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		glUniform1i(texMode_uniformId, 0);
		glBindVertexArray(currCreature.vao);
		glDrawElements(GL_TRIANGLES, currCreature.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
	}

	glUniform1i(tex_loc, 0);
	glUniform1i(texMode_uniformId, 5); // draw textured quads
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[4]);
	for (int j = -5; j < 0; j++) {
		pushMatrix(MODEL);
		translate(MODEL, 65, 0, 60 + j * 20.0);

		pos[0] = 65; pos[1] = 0; pos[2] = 60 + j * 20.0;

		if (type == 2)
			l3dBillboardSphericalBegin(cams[activeCam].camPos, pos);
		else if (type == 3)
			l3dBillboardCylindricalBegin(cams[activeCam].camPos, pos);

		objId = 62;

		//diffuse and ambient color are not used in the tree quads
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes[objId].mat.shininess);

		pushMatrix(MODEL);
		translate(MODEL, 0.0, 3.0, 0.0f);

		// send matrices to OGL
		if (type == 0 || type == 1) {     //Cheating matrix reset billboard techniques
			computeDerivedMatrix(VIEW_MODEL);

			//reset VIEW_MODEL
			if (type == 0) BillboardCheatSphericalBegin();
			else BillboardCheatCylindricalBegin();

			computeDerivedMatrix_PVM(); // calculate PROJ_VIEW_MODEL
		}
		else computeDerivedMatrix(PROJ_VIEW_MODEL);

		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);
		glBindVertexArray(myMeshes[objId].vao);
		glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
		popMatrix(MODEL);
		popMatrix(MODEL);
	}

	for (int j = -5; j < 0; j++) {
		pushMatrix(MODEL);
		translate(MODEL, -65, 0, 60 + j * 20.0);

		pos[0] = -65; pos[1] = 0; pos[2] = 60 + j * 20.0;

		if (type == 2)
			l3dBillboardSphericalBegin(cams[activeCam].camPos, pos);
		else if (type == 3)
			l3dBillboardCylindricalBegin(cams[activeCam].camPos, pos);

		objId = 62;

		//diffuse and ambient color are not used in the tree quads
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes[objId].mat.shininess);

		pushMatrix(MODEL);
		translate(MODEL, 0.0, 3.0, 0.0f);

		// send matrices to OGL
		if (type == 0 || type == 1) {     //Cheating matrix reset billboard techniques
			computeDerivedMatrix(VIEW_MODEL);

			//reset VIEW_MODEL
			if (type == 0) BillboardCheatSphericalBegin();
			else BillboardCheatCylindricalBegin();

			computeDerivedMatrix_PVM(); // calculate PROJ_VIEW_MODEL
		}
		else computeDerivedMatrix(PROJ_VIEW_MODEL);

		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);
		glBindVertexArray(myMeshes[objId].vao);
		glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
		popMatrix(MODEL);
		popMatrix(MODEL);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void invertLights() {
	float res[4];
	/*for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		// Render the reflected geometry
		for (int j = 0; j < 4; j++) {
			spotLightsPos[i][j] *= -1.0;  //mirror the position of light
		}
		multMatrixPoint(VIEW, spotLightsPos[i], res);

		glUniform4fv(sPos_uniformId[i], 1, res);
	}

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		// Render the reflected geometry
		for (int j = 0; j < 4; j++) {
			pointLightsPos[i][j] *= -1.0;  //mirror the position of light
		}
		multMatrixPoint(VIEW, pointLightsPos[i], res);

		glUniform4fv(lPos_uniformId[i], 1, res);
	}*/

	for (int j = 0; j < 4; j++) {
		dirLightPos[j] *= -1.0;  //mirror the position of light
	}
	multMatrixPoint(VIEW, dirLightPos, res);

	glUniform4fv(dPos_uniformId, 1, res);

}

void renderScene(void) {
	float mat[16];
	GLfloat plano_chao[4] = { 0,1,0,0 };
	float particle_color[4];
	float pos[3], right[3], up[3];

	auto currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
	lastTime = currentTime;

	seconds += deltaTime;

	bool isTimeMultipleOfSpeedIncrease = isApproximatelyMultipleOf(seconds, creatureSpeedIncreaseTime, 0.1f);
	if (isTimeMultipleOfSpeedIncrease
		&& creatureSpeedIncreaseAllowed
		&& !(creatureSpeedMultiplier > creatureMaxSpeedMultiplier)) {
		creatureSpeedMultiplier += 0.5f;
	}

	// make sure speed is only increased once whenever [creatureSpeedIncreaseTime] seconds passes
	if (isTimeMultipleOfSpeedIncrease) creatureSpeedIncreaseAllowed = false;
	else creatureSpeedIncreaseAllowed = true;

	GLint loc;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!isPaused && !gameOver) {



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

		fogColorLoc = glGetUniformLocation(shader.getProgramIndex(), "fogColor");
		fogStartLoc = glGetUniformLocation(shader.getProgramIndex(), "fogStart");
		fogDensityLoc = glGetUniformLocation(shader.getProgramIndex(), "fogDensity");

		glUniform4fv(fogColorLoc, 1, fogColor.data());
		glUniform1f(fogStartLoc, fogStart);
		glUniform1f(fogDensityLoc, fogDensity);

		//send the light position in eye coordinates
		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

		float res[4];
		GLfloat pointlight_ambient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat pointlight_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat pointlight_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float counter = 0;

		loc = glGetUniformLocation(shader.getProgramIndex(), "spotlight_mode");
		if (spot_trigger)
			glUniform1i(loc, 1);
		else
			glUniform1i(loc, 0);

		loc = glGetUniformLocation(shader.getProgramIndex(), "pointlight_mode");
		if (point_trigger)
			glUniform1i(loc, 1);
		else
			glUniform1i(loc, 0);

		loc = glGetUniformLocation(shader.getProgramIndex(), "dirlight_mode");
		if (direct_trigger)
			glUniform1i(loc, 1);
		else
			glUniform1i(loc, 0);

		//Associar os Texture Units aos Objects Texture
		//agua.png loaded in TU0; stone.tga loaded in TU1;  lightwood.tga loaded in TU2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureArray[0]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TextureArray[1]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TextureArray[2]);

		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, TextureArray[6]);
		
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[7]);

		//Indicar aos tres samplers do GLSL quais os Texture Units a serem usados
		glUniform1i(tex_loc, 0);
		glUniform1i(tex_loc1, 1);
		glUniform1i(tex_loc2, 2);
		//glUniform1i(tex_sphereMap_loc, 3);
		glUniform1i(tex_cube_loc, 4);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
			multMatrixPoint(VIEW, pointLightsPos[i], res);   //light position in eye coordinates
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
			multMatrixPoint(VIEW, res_aux, res);
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
		GLfloat dirLight_ambient[4] = { 0.3f, 0.24f, 0.14f, 1.0f };
		GLfloat dirLight_diffuse[4] = { 0.7f, 0.45f, 0.30f, 1.0f };
		GLfloat dirLight_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.ambient"), dirLight_ambient[0], dirLight_ambient[1], dirLight_ambient[2], dirLight_ambient[3]);
		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.diffuse"), dirLight_diffuse[0], dirLight_diffuse[1], dirLight_diffuse[2], dirLight_diffuse[3]);
		glUniform4f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.specular"), dirLight_specular[0], dirLight_specular[1], dirLight_specular[2], dirLight_specular[3]);
		glUniform1f(glGetUniformLocation(shader.getProgramIndex(), "dirLightPos.materials.shininess"), 100.0f);

		glLightfv(GL_LIGHT0, GL_AMBIENT, dirLight_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dirLight_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, dirLight_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, dirLightPos);

		for (int i = 0; i < myMeshes.size(); i++) {
			checkCollisionMeshes(myMeshes[i], boat);
		}

		if (boatColliding) {
			boatColliding = false;

			// Explosion needs to be handled here - particles code
			if (!fireworks) {
				fireworks = 1;
				iniParticles();
			}

			if (lives == 0) {
				// end explosion
				// wait for a while

				while (dead_num_particles < MAX_PARTICULAS) {
					updateParticles();
					dead_num_particles++;
				}

				fireworks = 0;
			}

			for (int j = 0; j < 10000000; j++)
			{
				// wait for a while
			}

			// Restart boat position
			boat.setPosition(0.0f, 5.0f, 0.0f);
		}

		for (int i = 0; i < creatures.size(); i++) {
			checkCollisionCreatures(creatures[i], boat);
		}

		
		glUniform1i(texMode_uniformId, 7);

		//it won't write anything to the zbuffer; all subsequently drawn scenery to be in front of the sky box. 
		glDepthMask(GL_FALSE);
		glFrontFace(GL_CW); // set clockwise vertex order to mean the front

		pushMatrix(MODEL);
		pushMatrix(VIEW);  //se quiser anular a translação

		//  Fica mais realista se não anular a translação da câmara 
		// Cancel the translation movement of the camera - de acordo com o tutorial do Antons
		mMatrix[VIEW][12] = 0.0f;
		mMatrix[VIEW][13] = 0.0f;
		mMatrix[VIEW][14] = 0.0f;

		scale(MODEL, 100.0f, 100.0f, 100.0f);
		translate(MODEL, -0.5f, -0.5f, -0.5f);

		// send matrices to OGL
		glUniformMatrix4fv(model_uniformId, 1, GL_FALSE, mMatrix[MODEL]); //Transformação de modelação do cubo unitário para o "Big Cube"
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);

		glBindVertexArray(skybox.vao);
		glDrawElements(skybox.type, skybox.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		popMatrix(MODEL);
		popMatrix(VIEW);

		glFrontFace(GL_CCW); // restore counter clockwise vertex order to mean the front
		glDepthMask(GL_TRUE);

		glUniform1i(texMode_uniformId, 0);
		
		
		
		glEnable(GL_STENCIL_TEST);        // Escrever 1 no stencil buffer onde se for desenhar a reflexão e a sombra
		glStencilFunc(GL_NEVER, 0x1, 0x1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

		renderTerrain();

		// Desenhar apenas onde o stencil buffer esta a 1
		glStencilFunc(GL_EQUAL, 0x1, 0x1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		invertLights();

		pushMatrix(MODEL);
		scale(MODEL, 1.0f, -1.0f, 1.0f);
		glCullFace(GL_FRONT);
		renderObjects();
		glCullFace(GL_BACK);
		popMatrix(MODEL);

		invertLights();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		renderTerrain();

		// Render the Shadows
		glUniform1i(shadowMode_uniformId, 1);

		shadow_matrix(mat, plano_chao, dirLightPos);
		glDisable(GL_DEPTH_TEST); //To force the shadow geometry to be rendered even if behind the floor

		//Dark the color stored in color buffer
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

		pushMatrix(MODEL);
		multMatrix(MODEL, mat);

		renderObjects();
		popMatrix(MODEL);

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		//render the geometry
		glUniform1i(shadowMode_uniformId, 0);
		renderObjects();

		//MIRROR

		//glDepthMask(GL_TRUE);

		scale(MODEL, 0.1f, 0.1f, 0.1f);

		glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glStencilFunc(GL_NEVER, 0x1, 0x1);
		glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

		//// Use an orthographic projection to draw the mirror geometry
		loadIdentity(PROJECTION);
		ortho(-1, 1, -1, 1, -1, 1);
		loadIdentity(VIEW);
		loadIdentity(MODEL);

		glBindVertexArray(myMeshes[64].vao);
		glDrawElements(myMeshes[64].type, myMeshes[64].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		loadIdentity(PROJECTION);
		perspective(80.0f, WinX / WinY, 0.1f, 1000.0f);

		loadIdentity(VIEW);
		float rearCamPos[3];
		float rearCamTarget[3];
		float boatDirection[3] = { boat.getDirection()[0], boat.getDirection()[1], boat.getDirection()[2] };

		for (int i = 0; i < 3; i++) {
			rearCamPos[i] = boat.getPosition()[i] - boatDirection[i] * 10.0f;
			rearCamTarget[i] = boat.getPosition()[i] - boatDirection[i] * 100.0f;
		}

		lookAt(
			rearCamPos[0],
			rearCamPos[1],
			rearCamPos[2],
			rearCamTarget[0],
			rearCamTarget[1],
			rearCamTarget[2],
			0, 1, 0
		);

		glStencilFunc(GL_EQUAL, 0x1, 0x1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// accept cull_face e depois voltar atras
		// sacale (MODEL, 1, 1, -1)
		renderTerrain();
		renderObjects();

		if (fireworks) {

			updateParticles();

			// draw fireworks particles
			int objId = 6;  //quad for particle

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, TextureArray[3]); //particle.tga associated to TU0 

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);  //Depth Buffer Read Only

			glUniform1i(texMode_uniformId, 2); // draw modulated textured particles 

			for (int i = 0; i < MAX_PARTICULAS; i++)
			{
				if (particula[i].life > 0.0f) /* s� desenha as que ainda est�o vivas */
				{

					/* A vida da part�cula representa o canal alpha da cor. Como o blend est� activo a cor final � a soma da cor rgb do fragmento multiplicada pelo
					alpha com a cor do pixel destino */

					particle_color[0] = particula[i].r;
					particle_color[1] = particula[i].g;
					particle_color[2] = particula[i].b;
					particle_color[3] = particula[i].life;

					// send the material - diffuse color modulated with texture
					loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
					glUniform4fv(loc, 1, particle_color);

					pushMatrix(MODEL);
					translate(MODEL, particula[i].x, particula[i].y, particula[i].z);

					// send matrices to OGL
					computeDerivedMatrix(PROJ_VIEW_MODEL);
					glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
					glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
					computeNormalMatrix3x3();
					glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

					glBindVertexArray(myMeshes[objId].vao);
					glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
					popMatrix(MODEL);
				}
				else dead_num_particles++;
			}

			glDepthMask(GL_TRUE); //make depth buffer again writeable

			if (dead_num_particles == MAX_PARTICULAS) {
				fireworks = 0;
				dead_num_particles = 0;
				printf("All particles dead\n");
			}

		}

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
		}
		else {
			ortho(ratio * -25, ratio * 25, -25, 25, 0.1, 100);
		}

		pushMatrix(VIEW);
		loadIdentity(VIEW);
		popMatrix(PROJECTION);
		popMatrix(VIEW);
		popMatrix(MODEL);

		if (flareEffect) {

			int flarePos[2];
			int m_viewport[4];
			glGetIntegerv(GL_VIEWPORT, m_viewport);

			pushMatrix(MODEL);
			loadIdentity(MODEL);
			computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function

			if (!project(dirLightPos, lightScreenPos, m_viewport))
				printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
			flarePos[0] = clampi((int)lightScreenPos[0], m_viewport[0], m_viewport[0] + m_viewport[2] - 1);
			flarePos[1] = clampi((int)lightScreenPos[1], m_viewport[1], m_viewport[1] + m_viewport[3] - 1);
			popMatrix(MODEL);

			//viewer looking down at  negative z direction
			pushMatrix(PROJECTION);
			loadIdentity(PROJECTION);
			pushMatrix(VIEW);
			loadIdentity(VIEW);
			ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
			render_flare(&AVTflare, flarePos[0], flarePos[1], m_viewport);
			popMatrix(PROJECTION);
			popMatrix(VIEW);
		}
	}

	// render HUD
	renderHUD();

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

	switch (key) {
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

	case 'p':
	case 'P':
		if (isPaused) {
			printf("Game resumed!\n");
			startTime = std::chrono::high_resolution_clock::now();
		}
		else {
			printf("Game paused!\n");
			auto currentTime = std::chrono::high_resolution_clock::now();
			elapsedTime += std::chrono::duration<float>(currentTime - startTime).count();
		}

		isPaused = !isPaused;
		break;

	case 'r':
	case 'R':
		if (gameOver) {
			lives = 5;
			points = 0;
			elapsedTime = 0.0f;
			startTime = std::chrono::high_resolution_clock::now();
			gameOver = false;
			printf("Game restarted!\n");
		}
		else {
			lives = 5;
			points = 0;
			elapsedTime = 0.0f;
			startTime = std::chrono::high_resolution_clock::now();
			printf("Game restarted!\n");
		}
		break;

	case 'f':
		if (flareEffect) {
			flareEffect = false;
			printf("Flare Effect Deactivated\n");
		}
		else {
			flareEffect = true;
			printf("Flare Effect Activated\n");
		}
		break;
	case 'F':
		if (!fireworks) {
			fireworks = 1;
			iniParticles();
		}
		break;

	case '5': type++; if (type == 5) type = 0;
		switch (type) {
		case 0: printf("Cheating Spherical (matrix reset)\n"); break;
		case 1: printf("Cheating Cylindrical (matrix reset)\n"); break;
		case 2: printf("True Spherical\n"); break;
		case 3: printf("True Cylindrical\n"); break;
		case 4: printf("No billboarding\n"); break;
		}
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
	if (state == GLUT_DOWN) {
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
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight_phong.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight_phong.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "tex_coord");

	glLinkProgram(shader.getProgramIndex());
	printf("InfoLog for Model Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	if (!shader.isProgramValid()) {
		printf("GLSL Model Program Not Valid!\n");
		printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());
		//exit(1);
	}

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode");
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
	shadowMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "shadowMode");
	model_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_Model");

	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	tex_normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalSphereMap");
	tex_sphereMap_loc = glGetUniformLocation(shader.getProgramIndex(), "sphereMap");
	tex_cube_loc = glGetUniformLocation(shader.getProgramIndex(), "cubeMap");

	fogColorLoc = glGetUniformLocation(shader.getProgramIndex(), "fogColor");
	fogStartLoc = glGetUniformLocation(shader.getProgramIndex(), "fogStart");
	fogDensityLoc = glGetUniformLocation(shader.getProgramIndex(), "fogDensity");

	normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalMap");
	specularMap_loc = glGetUniformLocation(shader.getProgramIndex(), "specularMap");
	diffMapCount_loc = glGetUniformLocation(shader.getProgramIndex(), "diffMapCount");

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


void createTrees() {

	std::string filepath = "tree/Tree2.obj";

	//import 3D file into Assimp scene graph
	if (!Import3DFromFile(filepath, importer, scene, scaleFactor))
		return;

	for (int i = 0; i < 5; i++) {
		//creation of Mymesh array with VAO Geometry and Material and array of Texture Objs for the model input by the user
		vector<struct MyMesh> tree = createMeshFromAssimp(scene, textureIds);

		myMeshes1.push_back(tree);

	}

	strcpy(model_dir, "tree/");
};

MyMesh createBillboard() {
	// create quad for billboard objID = 62

	//tree specular color
	float tree_spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float tree_shininess = 10.0f;
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	MyMesh amesh = createQuad(10.0f, 10.0f);
	memcpy(amesh.mat.specular, tree_spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = tree_shininess;
	amesh.mat.texCount = 0;


	return amesh;
}

void createSkyBox() {

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	float amb1[] = { 0.3f, 0.0f, 0.0f, 1.0f };
	float diff1[] = { 0.8f, 0.1f, 0.1f, 1.0f };
	float spec1[] = { 0.9f, 0.9f, 0.9f, 1.0f };


	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;

	int texcount = 0;

	// create geometry and VAO of the cube, objId=2;
	skybox = createCube();
	memcpy(skybox.mat.ambient, amb, 4 * sizeof(float));
	memcpy(skybox.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(skybox.mat.specular, spec, 4 * sizeof(float));
	memcpy(skybox.mat.emissive, emissive, 4 * sizeof(float));
	skybox.mat.shininess = shininess;
	skybox.mat.texCount = texcount;
	
}

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

	//Texture Object definition
	glGenTextures(8, TextureArray);
	Texture2D_Loader(TextureArray, "agua.png", 0);
	Texture2D_Loader(TextureArray, "relva1.png", 1);
	Texture2D_Loader(TextureArray, "water.png", 2);
	Texture2D_Loader(TextureArray, "particle.tga", 3);
	Texture2D_Loader(TextureArray, "tree.tga", 4);
	Texture2D_Loader(TextureArray, "pedra.png", 5);
	Texture2D_Loader(TextureArray, "sphereMap.jpg", 6);

	//Sky Box Texture Object
	const char* filenames[] = { "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg" };

	TextureCubeMap_Loader(TextureArray, filenames, 7);

	//Flare elements textures
	glGenTextures(5, FlareTextureArray);
	Texture2D_Loader(FlareTextureArray, "crcl.tga", 0);
	Texture2D_Loader(FlareTextureArray, "flar.tga", 1);
	Texture2D_Loader(FlareTextureArray, "hxgn.tga", 2);
	Texture2D_Loader(FlareTextureArray, "ring.tga", 3);
	Texture2D_Loader(FlareTextureArray, "sun.tga", 4);


	// top view cameras

	cams[0].camPos[0] = 0.01f;
	cams[0].camPos[1] = 200.0f;
	cams[0].camPos[2] = 0.01f;
	cams[0].type = ORTHOGONAL;

	cams[1].camPos[0] = 0.01f;
	cams[1].camPos[1] = 200.0f;
	cams[1].camPos[2] = 0.01f;

	createTrees();

	MyModel boatModel = boat.createMesh();
	myModels.push_back(boatModel);


	MyMesh terrainMesh = createTerrainMesh(terrainSize); // 0
	myMeshes.push_back(terrainMesh);

	MyMesh waterMesh = createWaterMesh(waterSize); // 1
	myMeshes.push_back(waterMesh);

	vector<MyMesh> houseMeshes = createHouseMeshes(50, terrainSize, waterSize); // 2 - 51
	myMeshes.insert(myMeshes.end(), houseMeshes.begin(), houseMeshes.end());

	vector<MyMesh> obstacleMeshes = createObstacleMeshes(10, terrainSize, waterSize); // 52 - 61
	myMeshes.insert(myMeshes.end(), obstacleMeshes.begin(), obstacleMeshes.end());

	// create quad for billboard objID = 62
	amesh = createBillboard();

	myMeshes.push_back(amesh);

	// create points for particles = 63
	amesh = createCube();
	myMeshes.push_back(amesh);

	amesh = createCube(); //64 //TENHO DE ALTERAR ISTO PARA ALTERAR A POSICAO DA CAMERA TRASEIRA
	myMeshes.push_back(amesh);

	vector<Creature> creatureMeshes = createCreatureMeshes(numCreatures, creatureRadius, creatureInitialSpeed);
	creatures.insert(creatures.end(), creatureMeshes.begin(), creatureMeshes.end());

	cams[2].followBoat(boat.getPosition(), boat.getDirection(), activeCam != 2, tracking == 1);
	cams[2].computeCameraAngles();

	//loadFlareFile(&AVTflare, "flare.txt");

	// create geometry and VAO of the quad for flare elements
	amesh = createQuad(1, 1);
	myMeshes.push_back(amesh); // ==65

	createSkyBox();  

	//Load flare from file
	loadFlareFile(&AVTflare, "flare.txt");

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		glEnable(GL_LIGHT1 + i);
	}
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		glEnable(GL_LIGHT2 + i);
	}

	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char** argv) {

	lastTime = std::chrono::high_resolution_clock::now();
	startTime = std::chrono::high_resolution_clock::now();

	//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100, 100);
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

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}



