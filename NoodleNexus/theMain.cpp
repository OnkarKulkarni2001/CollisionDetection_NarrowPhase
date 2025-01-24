//#define GLAD_GL_IMPLEMENTATION
//#include <glad/glad.h>
//
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "GLCommon.h"

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/quaternion.hpp> 

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>     // "input output" stream
#include <fstream>      // "file" stream
#include <sstream>      // "string" stream ("string builder" in Java c#, etc.)
#include <string>
#include <vector>

//void ReadPlyModelFromFile(std::string plyFileName);
#include "PlyFileLoaders.h"
#include "Basic_Shader_Manager/cShaderManager.h"
#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include "sharedThings.h"       // Fly camera
#include "cPhysics.h"
#include "cLightManager.h"
#include <windows.h>    // Includes ALL of windows... MessageBox
#include "cLightHelper/cLightHelper.h"
#include "cBasicTextureManager/cBasicTextureManager.h"

#include <ctime>
#include <cstdlib>
//
//const unsigned int MAX_NUMBER_OF_MESHES = 1000;
//unsigned int g_NumberOfMeshesToDraw;
//sMesh* g_myMeshes[MAX_NUMBER_OF_MESHES] = { 0 };    // All zeros

std::vector<sMesh*> g_vecProjectiles;
std::vector<glm::vec3> g_vecProjectileDirections;

std::vector<sMesh*> g_vecMeshesToDraw;
cPhysics* g_pPhysicEngine = NULL;
// This loads the 3D models for drawing, etc.
cVAOManager* g_pMeshManager = NULL;

cBasicTextureManager* g_pTextures = NULL;

//cLightManager* g_pLightManager = NULL;

void AddModelsToScene(cVAOManager* pMeshManager, GLuint shaderProgram);

void DrawMesh(sMesh* pCurMesh, GLuint program);

void DrawMesh(sMesh* pCurMesh, glm::mat4 matModel, GLuint program);

glm::vec3 cameraStart = glm::vec3(130.0f, 3.0f, 60.0f);
glm::vec3 cameraStartRotation = glm::vec3(0.0, 180.0, 0);

const int MAP_WIDTH = 23;
const int MAP_HEIGHT = 27;
float deltaTime = 0.0f;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

bool isControlDown(GLFWwindow* window);

#include "cTankBuilder.h"
void SetUpTankGame(void);
void TankStepFrame(double timeStep);
std::vector< iTank* > g_vecTheTanks;

//--------------------------------------------------------------------------------------------------------------------------

class RandomStringGenerator
{
public:
	// Constructor to initialize the random seed (only once)
	RandomStringGenerator()
	{
		std::srand(static_cast<unsigned int>(deltaTime));
	}

	// Set the number of strings and initialize the vector
	void setNumberOfStrings(int count)
	{
		strings.clear();
		strings.reserve(count);
	}

	// Add a string to the list
	void addString(const std::string& str)
	{
		strings.push_back(str);
	}

	// Get a random string from the list
	std::string getRandomString() const
	{
		if (strings.empty())
		{
			return ""; // Return an empty string if no strings were added
		}
		int randomIndex = std::rand() % strings.size();
		return strings[randomIndex];
	}

private:
	std::vector<std::string> strings;
};

//--------------------------------------------------------------------------------------------------------------------------

// END OF: TANK GAME

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	const float CAMERA_MOVE_SPEED = 0.1f;

	if (mods == GLFW_MOD_SHIFT)
	{
		if (key == GLFW_KEY_F9 && action == GLFW_PRESS)
		{
			// Save state to file
//            MyAmazingStateThing->saveToFile("MySaveFile.sexy");
		}
		if (key == GLFW_KEY_F10 && action == GLFW_PRESS)
		{
			// Save state to file
			// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
//            MessageBox(NULL, L"Hello!", L"The title", MB_OK);
			if (MessageBox(NULL, L"Kill all humans?", L"Bender asks", MB_YESNO) == IDYES)
			{
				std::cout << "You are heartless" << std::endl;
			}
			else
			{
				std::cout << "Humans still live..." << std::endl;
			}
		}
	}//if (mods == GLFW_MOD_SHIFT)

 //   if (mods == GLFW_KEY_LEFT_CONTROL)
	if (isControlDown(window))
	{
		if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		{
			// check if you are out of bounds
			if (::g_selectedLightIndex > 0)
			{

				::g_selectedLightIndex--;
			}
			//// 0 to 10
			//if (::g_selectedLightIndex < 0)
			//{
			//    ::g_selectedLightIndex = 0;
			//}

		}
		if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		{
			::g_selectedLightIndex++;
			if (::g_selectedLightIndex >= 10)
			{
				::g_selectedLightIndex = 9;
			}
		}
		if (key == GLFW_KEY_9 && action == GLFW_PRESS)
		{
			::g_bShowDebugSpheres = true;
		}
		if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		{
			::g_bShowDebugSpheres = false;
		}
	}//if (mods == GLFW_KEY_LEFT_CONTROL)


	return;
}

void ConsoleStuff(void);

// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float getRandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

glm::vec3 getRandom_vec3(glm::vec3 min, glm::vec3 max)
{
	return glm::vec3(
		getRandomFloat(min.x, max.x),
		getRandomFloat(min.y, max.y),
		getRandomFloat(min.z, max.z));
}

// Returns NULL if NOT found
sMesh* pFindMeshByFriendlyName(std::string theNameToFind)
{
	for (unsigned int index = 0; index != ::g_vecMeshesToDraw.size(); index++)
	{
		if (::g_vecMeshesToDraw[index]->uniqueFriendlyName == theNameToFind)
		{
			return ::g_vecMeshesToDraw[index];
		}
	}
	// Didn't find it
	return NULL;
}

glm::mat4 GiveModelMatrix(sMesh* pCurMesh)
{
	glm::mat4 matModel = glm::mat4(1.0f);

	// Translation (movement, position, placement...)
	glm::mat4 matTranslate
		= glm::translate(glm::mat4(1.0f),
			glm::vec3(pCurMesh->positionXYZ.x,
				pCurMesh->positionXYZ.y,
				pCurMesh->positionXYZ.z));

	// Rotation...
	// Caculate 3 Euler acix matrices...
	glm::mat4 matRotateX =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(pCurMesh->rotationEulerXYZ.x), // Angle in radians
			glm::vec3(1.0f, 0.0, 0.0f));

	glm::mat4 matRotateY =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(pCurMesh->rotationEulerXYZ.y), // Angle in radians
			glm::vec3(0.0f, 1.0, 0.0f));

	glm::mat4 matRotateZ =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(pCurMesh->rotationEulerXYZ.z), // Angle in radians
			glm::vec3(0.0f, 0.0, 1.0f));


	// Scale
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		glm::vec3(pCurMesh->uniformScale,
			pCurMesh->uniformScale,
			pCurMesh->uniformScale));


	// Calculate the final model/world matrix
	matModel *= matTranslate;     // matModel = matModel * matTranslate;
	matModel *= matRotateX;
	matModel *= matRotateY;
	matModel *= matRotateZ;
	matModel *= matScale;

	return matModel;
}

void AddSceneLights()
{
	// Set up one of the lights in the scene
	::g_pLightManager->theLights[0].position = glm::vec4(-15.0f, 30.0f, 0.0f, 1.0f);
	::g_pLightManager->theLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pLightManager->theLights[0].atten.y = 0.001f;
	::g_pLightManager->theLights[0].atten.z = 0.0001f;

	::g_pLightManager->theLights[0].param1.x = 0.0f;    // Point light (see shader)
	::g_pLightManager->theLights[0].param2.x = 1.0f;    // Turn on (see shader)


	// Set up one of the lights in the scene
	::g_pLightManager->theLights[1].position = glm::vec4(0.0f, 20.0f, 0.0f, 1.0f);
	::g_pLightManager->theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pLightManager->theLights[1].atten.y = 0.01f;
	::g_pLightManager->theLights[1].atten.z = 0.001f;

	::g_pLightManager->theLights[1].param1.x = 1.0f;    // Spot light (see shader)
	::g_pLightManager->theLights[1].direction = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	::g_pLightManager->theLights[1].param1.y = 5.0f;   //  y = inner angle
	::g_pLightManager->theLights[1].param1.z = 10.0f;  //  z = outer angle

	::g_pLightManager->theLights[1].param2.x = 1.0f;    // Turn on (see shader)
}

void SpawnProjectileTrail(cVAOManager* meshManager, GLuint program, glm::vec3 projectilePosition)
{
	// Get and print a random string
	std::string trail = "assets/Ply(converted)/projectiletrail.ply";

	//Import Floor Ply Data and add it to VAO
	sModelDrawInfo trailModel;
	::g_pMeshManager->LoadModelIntoVAO(trail, trailModel, program, false);

	//Make a mesh for the floor model
	sMesh* pTrailModel = new sMesh();
	pTrailModel->modelFileName = trail;
	pTrailModel->positionXYZ = projectilePosition;
	pTrailModel->rotationEulerXYZ.y = -90.0f;
	pTrailModel->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pTrailModel->bIsWireframe = false;
	pTrailModel->bDoNotLight = false;
	pTrailModel->bOverrideObjectColour = true;
	pTrailModel->uniformScale = 0.01f;
	pTrailModel->uniqueFriendlyName = "trail";
	::g_vecMeshesToDraw.push_back(pTrailModel);

	/*::g_pPhysicEngine->addTriangleMesh(
		trail,
		pTrailModel->positionXYZ,
		pTrailModel->rotationEulerXYZ,
		pTrailModel->uniformScale);*/
}

void Fire(glm::vec3 targetLocation, float speed, float deltaTime)
{
	sMesh* pProjectile = pFindMeshByFriendlyName("projectile");

	glm::vec3 startPosition = pProjectile->positionXYZ;
	glm::vec3 currentPosition = pProjectile->positionXYZ;
	float distance = glm::distance(startPosition, targetLocation);
	float distanceTraveled = 0.0f;

	distanceTraveled += speed * deltaTime;

	float t = glm::clamp(distanceTraveled / distance, 0.0f, 1.0f);

	currentPosition = glm::mix(startPosition, targetLocation, t);

	pProjectile->positionXYZ = currentPosition;

	if (t >= 1.0f) {
		// TODO: Implement what happens after it collides
	}
}

std::string GiveRandomCeilingProp() {
	RandomStringGenerator generator;

	generator.setNumberOfStrings(5);

	generator.addString("assets/Ply(converted)/SM_Env_Ceiling_Light_05_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Ceiling_Light_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Ceiling_Light_02_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Ceiling_Light_03_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Ceiling_Light_04_xyz_n_rgba_uv.ply");

	return generator.getRandomString();
}

void AddCeilingProps(cVAOManager* meshManager, GLuint program, glm::vec3 position)
{

	std::string randomProp = GiveRandomCeilingProp();

	sModelDrawInfo prop;
	::g_pMeshManager->LoadModelIntoVAO(randomProp, prop, program, true);

	sMesh* pProp = new sMesh();
	pProp->modelFileName = randomProp;
	pProp->positionXYZ = position;
	pProp->rotationEulerXYZ.y = -90.0f;
	pProp->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pProp->bIsWireframe = false;
	pProp->bDoNotLight = false;
	pProp->bOverrideObjectColour = true;
	pProp->uniqueFriendlyName = "ceilingprop";
	::g_vecMeshesToDraw.push_back(pProp);

	::g_pPhysicEngine->addTriangleMesh(
		randomProp,
		pProp->positionXYZ,
		pProp->rotationEulerXYZ,
		pProp->uniformScale);

}

std::string GiveRandomConstructionProp() {
	RandomStringGenerator generator;

	generator.setNumberOfStrings(8);

	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_08_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_02_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_03_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_04_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_05_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_06_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Construction_Block_07_xyz_n_rgba_uv.ply");

	return generator.getRandomString();
}	

void AddConstructionProps(cVAOManager* meshManager, GLuint program, glm::vec3 position)
{

	std::string randomProp = GiveRandomConstructionProp();

	sModelDrawInfo prop;
	::g_pMeshManager->LoadModelIntoVAO(randomProp, prop, program, true);

	sMesh* pProp = new sMesh();
	pProp->modelFileName = randomProp;
	pProp->positionXYZ = position;
	pProp->rotationEulerXYZ.y = -90.0f;
	pProp->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pProp->bIsWireframe = false;
	pProp->bDoNotLight = false;
	pProp->bOverrideObjectColour = true;
	pProp->uniqueFriendlyName = "constructionprop";
	pProp->uniformScale = 0.1f;
	::g_vecMeshesToDraw.push_back(pProp);

	::g_pPhysicEngine->addTriangleMesh(
		randomProp,
		pProp->positionXYZ,
		pProp->rotationEulerXYZ,
		pProp->uniformScale);

}

std::string GiveRandomProp() {
	RandomStringGenerator generator;

	generator.setNumberOfStrings(29);

	generator.addString("assets/Ply(converted)/SM_Item_Book_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Book_02_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Book_03_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Book_04_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Book_05_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Book_06_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Camera_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Cups_01_xyz_n_rgba_uv.ply");
	//generator.addString("assets/Ply(converted)/SM_Item_Drill_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Drink_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Extinguisher_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Flag_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Food_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Food_02_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Food_03_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Food_04_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Food_05_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Hammer_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Health_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Lamp_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Saw_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Scoop_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Tether_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Item_Wrench_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Prop_3DPrinter_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Prop_AirBalloon_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Prop_Battery_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Prop_Beaker_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Prop_Bed_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Prop_Bed_02_xyz_n_rgba_uv.ply");

	return generator.getRandomString();
}

void AddProps(cVAOManager* meshManager, GLuint program, glm::vec3 position)
{

	std::string randomProp = GiveRandomProp();

	sModelDrawInfo prop;
	::g_pMeshManager->LoadModelIntoVAO(randomProp, prop, program, true);

	sMesh* pProp = new sMesh();
	pProp->modelFileName = randomProp;
	pProp->positionXYZ = position;
	pProp->rotationEulerXYZ.y = -90.0f;
	pProp->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pProp->bIsWireframe = false;
	pProp->bDoNotLight = false;
	pProp->bOverrideObjectColour = true;
	pProp->uniqueFriendlyName = "prop";
	::g_vecMeshesToDraw.push_back(pProp);

	::g_pPhysicEngine->addTriangleMesh(
		randomProp,
		pProp->positionXYZ,
		pProp->rotationEulerXYZ,
		pProp->uniformScale);

}

void AddProps(cVAOManager* meshManager, GLuint program, glm::vec3 position, std::string propFile)
{


	sModelDrawInfo prop;
	::g_pMeshManager->LoadModelIntoVAO(propFile, prop, program, true);

	sMesh* pProp = new sMesh();
	pProp->modelFileName = propFile;
	pProp->positionXYZ = position;
	pProp->rotationEulerXYZ = glm::vec3(0.0f, -90.0f, 0.0f);
	pProp->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pProp->bIsWireframe = false;
	pProp->bDoNotLight = false;
	pProp->bOverrideObjectColour = true;
	pProp->uniqueFriendlyName = "prop";
	::g_vecMeshesToDraw.push_back(pProp);

	::g_pPhysicEngine->addTriangleMesh(
		propFile,
		pProp->positionXYZ,
		pProp->rotationEulerXYZ,
		pProp->uniformScale);

}

void AddFloor(cVAOManager* meshManager, GLuint program, glm::vec3 position)
{

	RandomStringGenerator generator;

	// Set the number of strings (for example, 4 strings)
	generator.setNumberOfStrings(3);

	// Add each string
	generator.addString("assets/Ply(converted)/SM_Env_Floor_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Floor_02_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Floor_03_xyz_n_rgba_uv.ply");
	//generator.addString("assets/Ply(converted)/SM_Env_Floor_04_xyz_n_rgba_uv.ply");			// This floor doesn't show

	// Get and print a random string
	std::string randomFloor = generator.getRandomString();

	//Import Floor Ply Data and add it to VAO
	sModelDrawInfo floorPanel;
	::g_pMeshManager->LoadModelIntoVAO(randomFloor, floorPanel, program, true);

	//Make a mesh for the floor model
	sMesh* pFloorPanel = new sMesh();
	pFloorPanel->modelFileName = randomFloor;
	pFloorPanel->positionXYZ = position;
	pFloorPanel->rotationEulerXYZ = glm::vec3 (0.0f, -90.0f, 0.0f);
	pFloorPanel->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pFloorPanel->bIsWireframe = false;
	pFloorPanel->bDoNotLight = false;
	pFloorPanel->bOverrideObjectColour = true;
	pFloorPanel->uniqueFriendlyName = "FloorPanel";
	::g_vecMeshesToDraw.push_back(pFloorPanel);

	::g_pPhysicEngine->addTriangleMesh(
		randomFloor,
		pFloorPanel->positionXYZ,
		pFloorPanel->rotationEulerXYZ,
		pFloorPanel->uniformScale);

	//---------------------------------------------------------------------------------------------------------------------

	//Import Ceiling Ply Data and add it to VAO
	//RandomStringGenerator generator;

	// Set the number of strings (for example, 4 strings)
	generator.setNumberOfStrings(2);

	// Add each string
	generator.addString("assets/Ply(converted)/SM_Env_Ceiling_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Ceiling_02_xyz_n_rgba_uv.ply");
	//generator.addString("assets/Ply(converted)/SM_Env_Ceiling_03_xyz_n_rgba_uv.ply");				// This ceiling tries to overlap

	// Get and print a random string
	std::string randomCeiling = generator.getRandomString();

	sModelDrawInfo ceilingPanel;
	::g_pMeshManager->LoadModelIntoVAO(randomCeiling, ceilingPanel, program, true);

	//Make a mesh for the ceiling Model
	sMesh* pCeilingPanel = new sMesh();
	pCeilingPanel->modelFileName = randomCeiling;
	pCeilingPanel->positionXYZ = glm::vec3(position.x-5.0f, position.y + 5.0f, position.z);
	pCeilingPanel->rotationEulerXYZ = glm::vec3(0.0f, -90.0f, 0.0f);
	pCeilingPanel->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pCeilingPanel->bIsWireframe = false;
	pCeilingPanel->bDoNotLight = false;
	pCeilingPanel->bOverrideObjectColour = true;
	pCeilingPanel->uniqueFriendlyName = "CeilingPanel";
	::g_vecMeshesToDraw.push_back(pCeilingPanel);

	::g_pPhysicEngine->addTriangleMesh(
		randomCeiling,
		pCeilingPanel->positionXYZ,
		pCeilingPanel->rotationEulerXYZ,
		pCeilingPanel->uniformScale);

}

void AddWall(cVAOManager* meshManager, GLuint program, glm::vec3 location, float rotation)
{
	RandomStringGenerator generator;

	// Set the number of strings (for example, 4 strings)
	generator.setNumberOfStrings(9);

	// Add each string
	generator.addString("assets/Ply(converted)/SM_Env_Wall_01_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_02_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_03_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_04_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_05_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_06_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_07_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_08_xyz_n_rgba_uv.ply");
	generator.addString("assets/Ply(converted)/SM_Env_Wall_09_xyz_n_rgba_uv.ply");

	// Get and print a random string
	std::string randomWall = generator.getRandomString();

	//Import Ply Data and add it to VAO
	sModelDrawInfo wallPanel;
	::g_pMeshManager->LoadModelIntoVAO(randomWall, wallPanel, program, true);
	std::cout << wallPanel.numberOfVertices << " vertices loaded" << std::endl;

	//Make a mesh for the model
	sMesh* pWallPanel = new sMesh();
	pWallPanel->modelFileName = randomWall;
	pWallPanel->positionXYZ = location;
	pWallPanel->rotationEulerXYZ = glm::vec3(0.0f, rotation, 0.0f);
	pWallPanel->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pWallPanel->bIsWireframe = false;
	pWallPanel->bDoNotLight = false;
	pWallPanel->bOverrideObjectColour = true;
	pWallPanel->uniqueFriendlyName = "FloorPanel";
	::g_vecMeshesToDraw.push_back(pWallPanel);

	::g_pPhysicEngine->addTriangleMesh(
		randomWall,
		pWallPanel->positionXYZ,
		pWallPanel->rotationEulerXYZ,
		pWallPanel->uniformScale);
}

float LineTraceForward(GLuint program, float maxDistance)
{
	cPhysics::sLine LASERbeam;
	glm::vec3 LASERbeam_Offset = glm::vec3(0.0f, -2.0f, 0.0f);
	float distance;

	if (::g_bShowLASERBeam)
	{
		// The fly camera is always "looking at" something 1.0 unit away
		glm::vec3 cameraDirection = ::g_pFlyCamera->getTargetRelativeToCamera();     //0,0.1,0.9

		LASERbeam.startXYZ = ::g_pFlyCamera->getEyeLocation();

		// Move the LASER below the camera
		LASERbeam.startXYZ += LASERbeam_Offset;
		glm::vec3 LASER_ball_location = LASERbeam.startXYZ;

		// Is the LASER less than 500 units long?
		// (is the last LAZER ball we drew beyond 500 units form the camera?)
		while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < maxDistance)
		{
			// Move the next ball 0.1 times the normalized camera direction
			LASER_ball_location += (cameraDirection * 0.10f);
			DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
		}
		LASERbeam.endXYZ = LASER_ball_location;
	}

	float shortestDistance = maxDistance;

	//Draw the end of this LASER beam
	DrawDebugSphere(LASERbeam.endXYZ, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

	// Now draw a different coloured ball wherever we get a collision with a triangle
	std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
	::g_pPhysicEngine->rayCast(LASERbeam.startXYZ, LASERbeam.endXYZ, vec_RayTriangle_Collisions, false);

	glm::vec4 triColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	float triangleSize = 0.25f;

	for (std::vector<cPhysics::sCollision_RayTriangleInMesh>::iterator itTriList = vec_RayTriangle_Collisions.begin();
		itTriList != vec_RayTriangle_Collisions.end(); itTriList++)
	{
		for (std::vector<cPhysics::sTriangle>::iterator itTri = itTriList->vecTriangles.begin();
			itTri != itTriList->vecTriangles.end(); itTri++)
		{
			DrawDebugSphere(itTri->intersectionPoint, triColour, triangleSize, program);
			triColour.r -= 0.1f;
			triColour.g -= 0.1f;
			triColour.b += 0.2f;
			triangleSize *= 1.25f;
			distance = glm::distance(itTri->intersectionPoint, ::g_pFlyCamera->getEyeLocation());
			if (distance < shortestDistance)
			{
				shortestDistance = distance;
			}
		}

	}
	return shortestDistance;
}

float LineTraceCollision(glm::vec3 cameraDirection, GLuint program, float maxDistance)
{
	cPhysics::sLine LASERbeam;
	glm::vec3 LASERbeam_Offset = glm::vec3(0.0);
	float distance{};

	if (::g_bShowLASERBeam)
	{
		LASERbeam.startXYZ = ::g_pFlyCamera->getEyeLocation();

		// Move the LASER below the camera
		LASERbeam.startXYZ += LASERbeam_Offset;
		glm::vec3 LASER_ball_location = LASERbeam.startXYZ;

		// Is the LASER less than 500 units long?
		// (is the last LAZER ball we drew beyond 500 units form the camera?)
		while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < maxDistance)
		{
			// Move the next ball 0.1 times the normalized camera direction
			LASER_ball_location += (cameraDirection * 0.10f);
			//DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
		}
		LASERbeam.endXYZ = LASER_ball_location;
	}

	float shortestDistance = maxDistance;

	//Draw the end of this LASER beam
	//DrawDebugSphere(LASERbeam.endXYZ, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

	// Now draw a different coloured ball wherever we get a collision with a triangle
	std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
	::g_pPhysicEngine->rayCast(LASERbeam.startXYZ, LASERbeam.endXYZ, vec_RayTriangle_Collisions, false);

	glm::vec4 triColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	float triangleSize = 0.25f;

	for (std::vector<cPhysics::sCollision_RayTriangleInMesh>::iterator itTriList = vec_RayTriangle_Collisions.begin();
		itTriList != vec_RayTriangle_Collisions.end(); itTriList++)
	{
		for (std::vector<cPhysics::sTriangle>::iterator itTri = itTriList->vecTriangles.begin();
			itTri != itTriList->vecTriangles.end(); itTri++)
		{
			//DrawDebugSphere(itTri->intersectionPoint, triColour, triangleSize, program);
			triColour.r -= 0.1f;
			triColour.g -= 0.1f;
			triColour.b += 0.2f;
			triangleSize *= 1.25f;
			distance = glm::distance(itTri->intersectionPoint, ::g_pFlyCamera->getEyeLocation());
			if (distance < shortestDistance)
			{
				shortestDistance = distance;
			}
		}
	}
	return shortestDistance;
}

void GenerateDamageSphere(GLuint program, glm::vec3 location, double deltaTime)
{
	sModelDrawInfo damageSphere;
	::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_xyz_N_uv.ply",
		damageSphere, program, false);
	std::cout << damageSphere.numberOfVertices << " vertices loaded" << std::endl;

	//Make a mesh for the model
	sMesh* pDamageSphere = new sMesh();
	pDamageSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
	pDamageSphere->positionXYZ = location;
	pDamageSphere->uniformScale = 0.1f;
	pDamageSphere->objectColourRGBA = glm::vec4(1.0, 0.0, 0.0, 1.0);
	pDamageSphere->bIsWireframe = false;
	pDamageSphere->bDoNotLight = true;
	pDamageSphere->bOverrideObjectColour = true;
	pDamageSphere->uniqueFriendlyName = "DamageSphere";
	::g_vecMeshesToDraw.push_back(pDamageSphere);
}

void GenerateDentSphere(GLuint program, glm::vec3 location, double deltaTime)
{
	sModelDrawInfo dentSphere;
	::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_xyz_N_uv.ply",
		dentSphere, program, false);
	std::cout << dentSphere.numberOfVertices << " vertices loaded" << std::endl;

	//Make a mesh for the model
	sMesh* pDentSphere = new sMesh();
	pDentSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
	pDentSphere->positionXYZ = location;
	pDentSphere->uniformScale = 0.1f;
	pDentSphere->objectColourRGBA = glm::vec4(0.1f, 0.1f, 0.1f, 1);
	pDentSphere->bIsWireframe = false;
	pDentSphere->bDoNotLight = true;
	pDentSphere->bOverrideObjectColour = true;
	pDentSphere->uniqueFriendlyName = "DentSphere";
	::g_vecMeshesToDraw.push_back(pDentSphere);
}

void UpdateDamageSpheres(double deltaTime, GLuint program)
{
	for (sMesh* pMesh : ::g_vecMeshesToDraw)
	{
		if (pMesh->uniqueFriendlyName == "DamageSphere")
		{
			if (pMesh->uniformScale < 1.0f)
			{
				pMesh->uniformScale += static_cast<float>(deltaTime);
				if (pMesh->uniformScale > 1.0f)
				{
					pMesh->uniformScale = 1.0f; // Clamp to max size
					pMesh->bIsVisible = false;
					GenerateDentSphere(program, pMesh->positionXYZ, deltaTime);
				}
			}
		}
	}
}

glm::vec3 FireLaser(GLuint program, float maxDistance, float duration, float deltaTime)
{
	cPhysics::sLine LASERbeam;
	glm::vec3 LASERbeam_Offset = glm::vec3(0.0, -1.0f, 0.0f);

	duration -= deltaTime * 0.5f;

	if (::g_bShowLASERBeam)
	{
		LASERbeam.startXYZ = ::g_pFlyCamera->getEyeLocation();

		// Move the LASER below the camera
		LASERbeam.startXYZ += LASERbeam_Offset;
		glm::vec3 LASER_ball_location = LASERbeam.startXYZ;

		// Is the LASER less than 500 units long?
		// (is the last LAZER ball we drew beyond 500 units form the camera?)
		while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < maxDistance)
		{
			// Move the next ball 0.1 times the normalized camera direction
			LASER_ball_location += (::g_pFlyCamera->getTargetRelativeToCamera() * 0.10f);
			DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
		}
		LASERbeam.endXYZ = LASER_ball_location;
	}
	glm::vec3  hitLocation = glm::vec3(10000.0f);

	//Draw the end of this LASER beam
	DrawDebugSphere(LASERbeam.endXYZ, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

	// Now draw a different coloured ball wherever we get a collision with a triangle
	std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
	::g_pPhysicEngine->rayCast(LASERbeam.startXYZ, LASERbeam.endXYZ, vec_RayTriangle_Collisions, false);

	glm::vec4 triColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	float triangleSize = 0.25f;

	for (std::vector<cPhysics::sCollision_RayTriangleInMesh>::iterator itTriList = vec_RayTriangle_Collisions.begin();
		itTriList != vec_RayTriangle_Collisions.end(); itTriList++)
	{
		for (std::vector<cPhysics::sTriangle>::iterator itTri = itTriList->vecTriangles.begin();
			itTri != itTriList->vecTriangles.end(); itTri++)
		{
			DrawDebugSphere(itTri->intersectionPoint, triColour, triangleSize, program);
			triColour.r -= 0.1f;
			triColour.g -= 0.1f;
			triColour.b += 0.2f;
			triangleSize *= 1.25f;
			if (glm::distance(itTri->intersectionPoint, ::g_pFlyCamera->getEyeLocation()) < glm::distance(hitLocation, ::g_pFlyCamera->getEyeLocation()))
			{
				hitLocation = itTri->intersectionPoint;
			}
		}
	}
	return hitLocation;
}

glm::vec3 GetProjectileHitLocation(GLuint program, float maxDistance, float duration, float deltaTime) {
	cPhysics::sLine LASERbeam;
	glm::vec3 LASERbeam_Offset = glm::vec3(0.0, -1.0f, 0.0f);

	duration -= deltaTime * 0.5f;

	if (::g_bShowLASERBeam)
	{
		LASERbeam.startXYZ = ::g_pFlyCamera->getEyeLocation();

		// Move the LASER below the camera
		LASERbeam.startXYZ += LASERbeam_Offset;
		glm::vec3 LASER_ball_location = LASERbeam.startXYZ;

		// Is the LASER less than 500 units long?
		// (is the last LAZER ball we drew beyond 500 units form the camera?)
		while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < maxDistance)
		{
			// Move the next ball 0.1 times the normalized camera direction
			LASER_ball_location += (::g_pFlyCamera->getTargetRelativeToCamera() * 0.10f);
			//DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
		}
		LASERbeam.endXYZ = LASER_ball_location;
	}
	glm::vec3  hitLocation = glm::vec3(10000.0f);

	//Draw the end of this LASER beam
	//DrawDebugSphere(LASERbeam.endXYZ, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

	// Now draw a different coloured ball wherever we get a collision with a triangle
	std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
	::g_pPhysicEngine->rayCast(LASERbeam.startXYZ, LASERbeam.endXYZ, vec_RayTriangle_Collisions, false);

	glm::vec4 triColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	float triangleSize = 0.25f;

	for (std::vector<cPhysics::sCollision_RayTriangleInMesh>::iterator itTriList = vec_RayTriangle_Collisions.begin();
		itTriList != vec_RayTriangle_Collisions.end(); itTriList++)
	{
		for (std::vector<cPhysics::sTriangle>::iterator itTri = itTriList->vecTriangles.begin();
			itTri != itTriList->vecTriangles.end(); itTri++)
		{
			//DrawDebugSphere(itTri->intersectionPoint, triColour, triangleSize, program);
			triColour.r -= 0.1f;
			triColour.g -= 0.1f;
			triColour.b += 0.2f;
			triangleSize *= 1.25f;
			if (glm::distance(itTri->intersectionPoint, ::g_pFlyCamera->getEyeLocation()) < glm::distance(hitLocation, ::g_pFlyCamera->getEyeLocation()))
			{
				hitLocation = itTri->intersectionPoint;
			}
		}
	}
	return hitLocation;
}

float LineTraceProjectileCollision(glm::vec3 startPosition, glm::vec3 direction, GLuint program, float maxDistance)
{
	cPhysics::sLine LASERbeam;
	glm::vec3 LASERbeam_Offset = glm::vec3(0.0);
	float distance{};

	if (::g_bShowLASERBeam)
	{
		LASERbeam.startXYZ = ::g_pFlyCamera->getEyeLocation();

		// Move the LASER below the camera
		LASERbeam.startXYZ += LASERbeam_Offset;
		glm::vec3 LASER_ball_location = LASERbeam.startXYZ;

		// Is the LASER less than 500 units long?
		// (is the last LAZER ball we drew beyond 500 units form the camera?)
		while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < maxDistance)
		{
			// Move the next ball 0.1 times the normalized camera direction
			LASER_ball_location += (direction * 0.10f);
			//DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
		}
		LASERbeam.endXYZ = LASER_ball_location;
	}

	float shortestDistance = maxDistance;

	//Draw the end of this LASER beam
	//DrawDebugSphere(LASERbeam.endXYZ, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

	// Now draw a different coloured ball wherever we get a collision with a triangle
	std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
	::g_pPhysicEngine->rayCast(LASERbeam.startXYZ, LASERbeam.endXYZ, vec_RayTriangle_Collisions, false);

	glm::vec4 triColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	float triangleSize = 0.25f;

	for (std::vector<cPhysics::sCollision_RayTriangleInMesh>::iterator itTriList = vec_RayTriangle_Collisions.begin();
		itTriList != vec_RayTriangle_Collisions.end(); itTriList++)
	{
		for (std::vector<cPhysics::sTriangle>::iterator itTri = itTriList->vecTriangles.begin();
			itTri != itTriList->vecTriangles.end(); itTri++)
		{
			triColour.r -= 0.1f;
			triColour.g -= 0.1f;
			triColour.b += 0.2f;
			triangleSize *= 1.25f;
			distance = glm::distance(itTri->intersectionPoint, ::g_pFlyCamera->getEyeLocation());
			if (distance < shortestDistance)
			{
				shortestDistance = distance;
			}
		}
	}
	return shortestDistance;
}

glm::vec3 SpawnProjectile(cVAOManager* meshManager, GLuint program, glm::vec3 camPosition, glm::vec3 targetLocation)
{
	//sModelDrawInfo projectile;

	//std::string projectileFile = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
	//::g_pMeshManager->LoadModelIntoVAO(projectileFile, projectile, program, false);

	sMesh* pProjectile = new sMesh();
	pProjectile->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
	pProjectile->positionXYZ = camPosition + 5.0f * ::g_pFlyCamera->getTargetRelativeToCamera();
	pProjectile->uniformScale = 0.1f;
	pProjectile->objectColourRGBA = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	pProjectile->bIsWireframe = false;
	pProjectile->bDoNotLight = true;
	pProjectile->bOverrideObjectColour = true;
	pProjectile->uniqueFriendlyName = "projectile";

	::g_vecProjectiles.push_back(pProjectile);

	glm::vec3 fireDirection = glm::normalize(targetLocation - camPosition);
	::g_vecProjectileDirections.push_back(fireDirection);

	::g_pPhysicEngine->addTriangleMesh(
		"assets/models/Sphere_radius_1_xyz_N_uv.ply",
		pProjectile->positionXYZ,
		pProjectile->rotationEulerXYZ,
		pProjectile->uniformScale);

	return fireDirection;
}

void readMapFile(const std::string& filename, int map[MAP_HEIGHT][MAP_WIDTH], cVAOManager* meshManager, GLuint program, float distance, float collisionThreshold)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}
	std::string line;
	int y = 0;
	while (std::getline(file, line) && y < MAP_HEIGHT)
	{
		std::istringstream iss(line);
		std::string value;
		int x = 0;

		while (iss >> value && x < MAP_WIDTH)
		{
			if (value == "x")
			{
				map[y][x] = 1;  // Wall
			}
			else if (value == "0")
			{
				map[y][x] = 0;  // Empty space
			}
			x++;
		}
		y++;
	}
	file.close();

	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			if (map[y][x] == 0)		//Chechk Floor
			{
				AddFloor(meshManager, program, glm::vec3(x * distance, 0.0, y * distance));
				if (map[y][x - 1] == 1)	//		check if left object is X or floor, if x place wall
				{
					AddWall(::g_pMeshManager, program, glm::vec3((x * distance) - distance, 0.0, (y * distance) - distance), 90.0f);
				}
				if (map[y][x + 1] == 1)		//check if right object is X or floor, if x place wall
				{
					AddWall(::g_pMeshManager, program, glm::vec3((x * distance), 0.0, (y * distance)), -90.0f);
					//std::cout << "Top Row Detected\n";
				}
				if (y == 0 && map[y][x] == 0)	//check if top side is edge
				{
					AddWall(::g_pMeshManager, program, glm::vec3((x * distance), 0.0, (y * distance) - distance), 0.0f);
					/*std::cout << "Top Row Detected\n";*/
				}
				if (map[y][x] == 0 && map[y - 1][x] == 1)		//check if top side is X, then place a wall
				{
					AddWall(::g_pMeshManager, program, glm::vec3((x * distance), 0.0, (y * distance) - distance), 0.0f);
				}
				if (map[y][x] == 0 && map[y + 1][x] == 1)		//check if bottom side is X, then place a wall
				{
					AddWall(::g_pMeshManager, program, glm::vec3((x * distance) - distance, 0.0, (y * distance)), 180.0f);
				}
				if (x == 0 && map[y][x] == 0)		//check if left edge
				{
					AddWall(::g_pMeshManager, program, glm::vec3((x * distance) - distance, 0.0, (y * distance) - distance), 90.0f);
				}
				if (x == MAP_WIDTH - 1 && map[y][x] == 0)		//Check if right Edge
				{
					//AddWall(::g_pMeshManager, program, glm::vec3((x * distance), 0.0, (y * distance) - distance), -90.0f);
				}
				if (y == MAP_HEIGHT - 1 && map[y][x] == 0)		//Check if Bottom Edge
				{
					AddWall(::g_pMeshManager, program, glm::vec3((x * distance) - distance, 0.0, (y * distance)), 180.0f);
				}
			}
		}
	}
}

void readPropMapFile(const std::string& filename, int map[MAP_HEIGHT][MAP_WIDTH], cVAOManager* meshManager, GLuint program, float distance, float collisionThreshold)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}
	std::string line;
	int y = 0;
	while (std::getline(file, line) && y < MAP_HEIGHT)
	{
		std::istringstream iss(line);
		std::string value;
		int x = 0;

		while (iss >> value && x < MAP_WIDTH)
		{
			if (value == "x")
			{
				map[y][x] = 1;  // Wall
			}
			else if (value == "cp")
			{
				map[y][x] = 2;
			}
			else if (value == "p")
			{
				map[y][x] = 3;
			}
			else if (value == "wp")
			{
				map[y][x] = 4;
			}
			else if (value == "0")
			{
				map[y][x] = 0;  // Empty space
				//AddFloor(meshManager, program, glm::vec3(x * distance, 0.0, y * distance));
				//AddProps(g_pMeshManager, program, glm::vec3(x * distance, 0.0, y * distance));
			}
			x++;
		}
		y++;
	}
	file.close();

	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			if (map[y][x] == 3) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance + offsetZ));
				AddProps(g_pMeshManager, program, glm::vec3(x * distance, 0.0, y * distance), "assets/Ply(converted)/SM_Env_PlantWall_02_xyz_n_rgba_uv.ply");
			}
			if (map[y][x] == 2) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddCeilingProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 5.0, y * distance + offsetZ));
			}
			if (map[y][x] == 4) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddConstructionProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance));
			}
			if (map[y][x] == 5) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance), "assets/Ply(converted)/SM_Item_Lamp_01_xyz_n_rgba_uv.ply");
			}
			if (map[y][x] == 6) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance), "assets/Ply(converted)/SM_Prop_3DPrinter_01_xyz_n_rgba_uv.ply");
			}
			if (map[y][x] == 7) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance), "assets/Ply(converted)/SM_Item_Food_03_xyz_n_rgba_uv.ply");
			}
			if (map[y][x] == 8) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance + offsetZ), "assets/Ply(converted)/SM_Item_Extinguisher_01_xyz_n_rgba_uv.ply");
			}
			if (map[y][x] == 9) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance), "assets/Ply(converted)/SM_Prop_Bed_01_xyz_n_rgba_uv.ply");
			}
			if (map[y][x] == 10) {
				float offsetX = getRandomFloat(-1.0f, 1.0f);
				float offsetZ = getRandomFloat(-1.0f, 1.0f);
				AddProps(g_pMeshManager, program, glm::vec3(x * distance + offsetX, 0.0, y * distance), "assets/Ply(converted)/SM_Item_Scoop_01_xyz_n_rgba_uv.ply");
			}
		}
	}
}

sMesh* CrosshairSphere(glm::vec3 position, GLuint program, sMesh* pCrosshairSphere)
{
	// Created the debug sphere, yet?
		pCrosshairSphere = new sMesh();
		//        pDebugSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
		pCrosshairSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
		pCrosshairSphere->positionXYZ = position;
		pCrosshairSphere->bIsWireframe = false;
		//pCrosshairSphere->objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		pCrosshairSphere->uniqueFriendlyName = "Debug_Sphere";
		//pCrosshairSphere->uniformScale = 10.0f;
		pCrosshairSphere->bDoNotLight = true;
		// At this point the debug sphere is created

		pCrosshairSphere->bIsVisible = true;
		pCrosshairSphere->positionXYZ = position;
		pCrosshairSphere->bOverrideObjectColour = true;
		pCrosshairSphere->objectColourRGBA = glm::vec4(0.0, 1.0, 0.0,1.0);
		pCrosshairSphere->uniformScale = 1.01f;

		//DrawMesh(pCrosshairSphere, program);

		pCrosshairSphere->bIsVisible = true;

	return pCrosshairSphere;
}


int main(void)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Callback for keyboard, but for "typing"
	// Like it captures the press and release and repeat
	glfwSetKeyCallback(window, key_callback);

	// 
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetWindowFocusCallback(window, cursor_enter_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// NOTE: OpenGL error checks have been omitted for brevity

	cShaderManager* pShaderManager = new cShaderManager();

	cShaderManager::cShader vertexShader;
	vertexShader.fileName = "assets/shaders/vertex01.glsl";

	cShaderManager::cShader fragmentShader;
	fragmentShader.fileName = "assets/shaders/fragment01.glsl";

	if (!pShaderManager->createProgramFromFile("shader01",
		vertexShader, fragmentShader))
	{
		std::cout << "Error: " << pShaderManager->getLastError() << std::endl;
	}
	else
	{
		std::cout << "Shader built OK" << std::endl;
	}

	const GLuint program = pShaderManager->getIDFromFriendlyName("shader01");

	glUseProgram(program);
	// Loading the TYPES of models I can draw...

//    cVAOManager* pMeshManager = new cVAOManager();
	::g_pMeshManager = new cVAOManager();

	::g_pPhysicEngine = new cPhysics();
	// For triangle meshes, let the physics object "know" about the VAO manager
	::g_pPhysicEngine->setVAOManager(::g_pMeshManager);

	::g_pTextures = new cBasicTextureManager();

	::g_pTextures->SetBasePath("assets/textures");
	::g_pTextures->Create2DTextureFromBMPFile("SpaceInteriors_Emmision.bmp");
	::g_pTextures->Create2DTextureFromBMPFile("SpaceInteriors_Texture.bmp");
	::g_pTextures->Create2DTextureFromBMPFile("WorldMap.bmp");

	// This also adds physics objects to the phsyics system
	AddModelsToScene(::g_pMeshManager, program);

	::g_pFlyCamera = new cBasicFlyCamera();
	::g_pFlyCamera->setEyeLocation(cameraStart);
	::g_pFlyCamera->rotateLeftRight_Yaw_NoScaling(180.0);
	// Rotate the camera 180 degrees
	::g_pFlyCamera->rotateLeftRight_Yaw_NoScaling(glm::radians(90.0f));

	glUseProgram(program);

	// Enable depth buffering (z buffering)
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
	glEnable(GL_DEPTH_TEST);

	double currentFrameTime = glfwGetTime();
	double lastFrameTime = glfwGetTime();

	std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed with current time

	// Set up the lights
	::g_pLightManager = new cLightManager();
	// Called only once
	::g_pLightManager->loadUniformLocations(program);

	AddSceneLights();

	//int num = 50;
	float floorDistance = 5.0f;
	float floorThreshold = 1.5f;

	int map[MAP_HEIGHT][MAP_WIDTH] = { 0 };
	std::string filename = "../NoodleNexus/Map.txt";
	readMapFile(filename, map, ::g_pMeshManager, program, floorDistance, floorThreshold);
	readPropMapFile("../NoodleNexus/PropMap.txt", map, ::g_pMeshManager, program, floorDistance, floorThreshold);

	// Set the texture sampler to one of the 3 textures we loaded
//    GLuint badBunnyTexNum = ::g_pTextures->getTextureIDFromName("bad_bunny_1920x1080.bmp");
//    GLuint badBunnyTexNum = ::g_pTextures->getTextureIDFromName("dua-lipa-promo.bmp");
	GLuint badBunnyTexNum = ::g_pTextures->getTextureIDFromName("SpaceInteriors_Texture.bmp");

	// Bund to texture unit #3 (just because. for no particular reason)
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, badBunnyTexNum);
	// glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only

	//uniform sampler2D texture01;
	GLint texture01_UL = glGetUniformLocation(program, "texture01");
	// Connects the sampler to the texture unit
	glUniform1i(texture01_UL, 3);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here

	/*cLightHelper TheLightHelper;*/
	float frontDistance = 0.0f;
	float backDistance = 0.0f;
	float rightDistance = 0.0f;
	float leftDistance = 0.0f;
	float botDistance;
	float collisionThreshold = 3.0f;
	float downDistance = 0.0f;
	glm::vec3 fireDirection;
	sMesh* pProjectile;
	bool bIsFKeyPressed = false;
	glm::vec3 laserHitLocation;
	float projectileDistance;
	glm::vec3 projectileDirection;

	//glm::vec3 crosshairPosition = glm::vec3(0.0f);
	//sMesh pCrosshairSphere;

	//pCrosshairSphere = *CrosshairSphere(crosshairPosition, program, &pCrosshairSphere);
	//DrawMesh(&pCrosshairSphere, program);

	float laserDuration = 2.0f; // 2 seconds, adjust as needed
	bool laserActive = false;

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//        glm::mat4 m, p, v, mvp;
		glm::mat4 matProjection = glm::mat4(1.0f);

		matProjection = glm::perspective(0.6f,           // FOV
			ratio,          // Aspect ratio of screen
			0.1f,           // Near plane
			1000.0f);       // Far plane

		// View or "camera"
		glm::mat4 matView = glm::mat4(1.0f);

		//        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, 4.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

		matView = glm::lookAt(::g_pFlyCamera->getEyeLocation(),
			::g_pFlyCamera->getTargetLocation(),
			upVector);
		//        matView = glm::lookAt( cameraEye,
		//                               cameraTarget,
		//                               upVector);

		UpdateDamageSpheres(deltaTime, program);

		const GLint matView_UL = glGetUniformLocation(program, "matView");
		glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);

		const GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
		glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			if (!bIsFKeyPressed)
			{
				projectileDirection = SpawnProjectile(g_pMeshManager, program, g_pFlyCamera->getEyeLocation(), g_pFlyCamera->getTargetLocation());

				bIsFKeyPressed = true;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			if (!bIsFKeyPressed)
			{
				laserHitLocation = FireLaser(program, 500.0f, 5.0f, deltaTime);
				GenerateDamageSphere(program, laserHitLocation, deltaTime);

				bIsFKeyPressed = true;
			}
		}
		else
		{
			bIsFKeyPressed = false;
		}
		for (int i = 0; i != g_vecProjectiles.size(); i++)
		{

			g_vecProjectiles[i]->positionXYZ += g_vecProjectileDirections[i] * 10.0f * deltaTime;

			sMesh* pCurMesh = ::g_vecProjectiles[i];
			glm::vec3 projectileHitLocation = GetProjectileHitLocation(program, 500.0f, 1.0f, deltaTime);
			projectileDistance = LineTraceProjectileCollision(pCurMesh->positionXYZ, g_vecProjectileDirections[i], program, 500.0f);
			std::cout << projectileDistance << std::endl;
			if (projectileDistance <= 1.0f)
			{
				std::cout << "Destroy Projectile\n";
				//GenerateDamageSphere(program, ::g_vecProjectiles[i]->positionXYZ, deltaTime);

			}
			//crosshairPosition = ::g_pFlyCamera->getTargetRelativeToCamera() + glm::vec3(0,0,5);
			//CrosshairSphere(crosshairPosition, program, &pCrosshairSphere);

			/*DrawMesh(pCurMesh, program);*/
			DrawMesh(pCurMesh, program);

			//sMesh* targetMesh = pFindMeshByFriendlyName("trail");

			//// Locate the target mesh in the vector
			//std::vector<sMesh*>::iterator it = std::find(g_vecMeshesToDraw.begin(), g_vecMeshesToDraw.end(), targetMesh);

			//// Check if the mesh was found before attempting to erase
			//if (it != g_vecMeshesToDraw.end()) {
			//	g_vecMeshesToDraw.erase(it);
			//}
		}

		//        // *******************************************************************

				// Update the light info in the shader
				// (Called every frame)
		::g_pLightManager->updateShaderWithLightInfo();
		// *******************************************************************


		//    ____                       _                      
		//   |  _ \ _ __ __ ___      __ | |    ___   ___  _ __  
		//   | | | | '__/ _` \ \ /\ / / | |   / _ \ / _ \| '_ \ 
		//   | |_| | | | (_| |\ V  V /  | |__| (_) | (_) | |_) |
		//   |____/|_|  \__,_| \_/\_/   |_____\___/ \___/| .__/ 
		//                                               |_|    
		// Draw all the objects
		//for (unsigned int meshIndex = 0; meshIndex != ::g_NumberOfMeshesToDraw; meshIndex++)
		for (unsigned int meshIndex = 0; meshIndex != ::g_vecMeshesToDraw.size(); meshIndex++)
		{
			//            sMesh* pCurMesh = ::g_myMeshes[meshIndex];
			sMesh* pCurMesh = ::g_vecMeshesToDraw[meshIndex];

			DrawMesh(pCurMesh, program);

		}//for (unsigned int meshIndex..

		//LineTraceForward(program, 500.0f);

		frontDistance = LineTraceCollision(::g_pFlyCamera->getTargetRelativeToCamera(), program, 100.0f);
		rightDistance = LineTraceCollision(::g_pFlyCamera->getRightVector() * (-1.0f), program, 150.0f);
		leftDistance = LineTraceCollision(::g_pFlyCamera->getRightVector(), program, 150.0f);
		backDistance = LineTraceCollision(::g_pFlyCamera->getTargetRelativeToCamera() * (-1.0f), program, 150.0f);
		downDistance = LineTraceCollision(glm::vec3(0.0f, -1.0f, 0.0f), program, 150.0f);

		g_pFlyCamera->setEyeLocation(g_pFlyCamera->getEyeLocation().x, g_pFlyCamera->getEyeLocation().y - deltaTime, g_pFlyCamera->getEyeLocation().z);

		if (downDistance <= 150.0f) {
			g_pFlyCamera->setEyeLocation(g_pFlyCamera->getEyeLocation().x, 3.0f, g_pFlyCamera->getEyeLocation().z);
		}

		//std::cout << "Front Distance : " << frontDistance << std::endl;
		//std::cout << "Back Distance : " << backDistance << std::endl;
		//std::cout << "Left Distance : " << leftDistance << std::endl;
		//std::cout << "Right Distance : " << rightDistance << std::endl;

		// Calculate elapsed time
		// We'll enhance this
		currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;


		// Physic update and test 
		::g_pPhysicEngine->StepTick(deltaTime);




		//if (laserActive)
		//{
		//	glm::vec3 hitLocation = FireLaser(program, maxDistance, laserDuration, deltaTime);

		//	// Check if duration has ended and deactivate laser if so
		//	if (laserDuration <= 0.0f)
		//	{
		//		laserActive = false;
		//	}
		//}






		// Handle any collisions
		if (::g_pPhysicEngine->vec_SphereAABB_Collisions.size() > 0)
		{
			// Yes, there were collisions

			for (unsigned int index = 0; index != ::g_pPhysicEngine->vec_SphereAABB_Collisions.size(); index++)
			{
				cPhysics::sCollision_SphereAABB thisCollisionEvent = ::g_pPhysicEngine->vec_SphereAABB_Collisions[index];

				if (thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y < 0.0f)
				{
					// Yes, it's heading down
					// So reverse the direction of velocity
					//thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y = fabs(thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y);
				}

			}//for (unsigned int index

		}//if (::g_pPhysicEngine->vec_SphereAABB_Collisions


		// Handle async IO stuff
		handleKeyboardAsync(window, rightDistance, leftDistance, frontDistance, backDistance, collisionThreshold);
		handleMouseAsync(window);

		glfwSwapBuffers(window);
		glfwPollEvents();


		//std::cout << "Camera: "
		std::stringstream ssTitle;
		ssTitle << "Camera: "
			<< ::g_pFlyCamera->getEyeLocation().x << ", "
			<< ::g_pFlyCamera->getEyeLocation().y << ", "
			<< ::g_pFlyCamera->getEyeLocation().z
			<< "   ";
		ssTitle << "light[" << g_selectedLightIndex << "] "
			<< ::g_pLightManager->theLights[g_selectedLightIndex].position.x << ", "
			<< ::g_pLightManager->theLights[g_selectedLightIndex].position.y << ", "
			<< ::g_pLightManager->theLights[g_selectedLightIndex].position.z
			<< "   "
			<< "linear: " << ::g_pLightManager->theLights[0].atten.y
			<< "   "
			<< "quad: " << ::g_pLightManager->theLights[0].atten.z;


		//        glfwSetWindowTitle(window, "Hey!");
		glfwSetWindowTitle(window, ssTitle.str().c_str());


	}// End of the draw loop


	// Delete everything
	delete ::g_pFlyCamera;
	delete ::g_pPhysicEngine;

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void AddModelsToScene(cVAOManager* pMeshManager, GLuint program)
{

	//Import Ply Data and add it to VAO
	sModelDrawInfo sphere;
	::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_xyz_N_uv.ply",
		sphere, program, false);
	std::cout << sphere.numberOfVertices << " vertices loaded" << std::endl;


	sModelDrawInfo hanger;
	::g_pMeshManager->LoadModelIntoVAO("assets/additionalModels/SM_Bld_Hanger_01_xyz_n_uv_NO_DOORS_or_WINDOWS.ply", hanger, program, false);

	//Make a mesh for the ceiling Model
	sMesh* pHanger = new sMesh();
	pHanger->modelFileName = "assets/additionalModels/SM_Bld_Hanger_01_xyz_n_uv_NO_DOORS_or_WINDOWS.ply";
	pHanger->positionXYZ = glm::vec3(126.0, 0.0f, (MAP_HEIGHT / 2) * 5.0f);
	pHanger->uniformScale = 0.01f;
	pHanger->rotationEulerXYZ = glm::vec3(0.0f, 90.0f, 0.0f);
	pHanger->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pHanger->bIsWireframe = false;
	pHanger->bDoNotLight = false;
	pHanger->bOverrideObjectColour = true;
	pHanger->uniqueFriendlyName = "Hanger";
	::g_vecMeshesToDraw.push_back(pHanger);

	::g_pPhysicEngine->addTriangleMesh(
		"assets/additionalModels/SM_Bld_Hanger_01_xyz_n_uv_NO_DOORS_or_WINDOWS.ply",
		pHanger->positionXYZ,
		pHanger->rotationEulerXYZ,
		pHanger->uniformScale);


	sModelDrawInfo hangerPlatform;
	::g_pMeshManager->LoadModelIntoVAO("assets/additionalModels/SM_Bld_HangerPlatform_01_xyz_n_uv.ply", hanger, program, false);

	//Make a mesh for the ceiling Model
	sMesh* pHangerPlatform = new sMesh();
	pHangerPlatform->modelFileName = "assets/additionalModels/SM_Bld_HangerPlatform_01_xyz_n_uv.ply";
	pHangerPlatform->positionXYZ = pHanger->positionXYZ;
	pHangerPlatform->uniformScale = pHanger->uniformScale;
	pHangerPlatform->rotationEulerXYZ = pHanger->rotationEulerXYZ;
	pHangerPlatform->objectColourRGBA = glm::vec4(getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), getRandomFloat(0.0f, 0.1f), 1);
	pHangerPlatform->bIsWireframe = false;
	pHangerPlatform->bDoNotLight = false;
	pHangerPlatform->bOverrideObjectColour = true;
	pHangerPlatform->uniqueFriendlyName = "HangerPlatform";
	::g_vecMeshesToDraw.push_back(pHangerPlatform);

	::g_pPhysicEngine->addTriangleMesh(
		"assets/additionalModels/SM_Bld_HangerPlatform_01_xyz_n_uv.ply",
		pHangerPlatform->positionXYZ,
		pHangerPlatform->rotationEulerXYZ,
		pHangerPlatform->uniformScale);

}

//using namespace std;

void ConsoleStuff(void)
{
	// "o" for output
//    std::ofstream myFile("someData.txt");
	// Write something
	//myFile << "Hello" << std::endl;
	//myFile << "there";
	//myFile.close();

	// Now read this file
//    std::ifstream myFile2("someData.txt");
//    std::string someString;
//    myFile2 >> someString;
//    std::cout << someString << std::endl;
//
	//std::string aword;
	//while (aword != "END_OF_FILE")
	//{
	//    myFile2 >> aword;
	//    std::cout << aword << std::endl;
	//};

	//std::string aword;
	//while (myFile2 >> aword)
	//{
	//    std::cout << aword << std::endl;
	//};

	std::ifstream myFile2("assets/models/bun_zipper_res3.ply");
	if (myFile2.is_open())
	{

		std::string aword;
		while (myFile2 >> aword)
		{
			std::cout << aword << std::endl;
		};
	}
	else
	{
		std::cout << "Can't find file" << std::endl;
	}


	// iostream
	std::cout << "Type a number:" << std::endl;

	int x = 0;
	std::cin >> x;

	std::cout << "You typed: " << x << std::endl;

	std::cout << "Type your name:" << std::endl;
	std::string name;
	std::cin >> name;

	std::cout << "Hello " << name << std::endl;
	return;
}

cTankBuilder* pTheTankBuilder = NULL;

// This is here for speed 
void SetUpTankGame(void)
{
	//int& y = getNumber();


//    cTankFactory::shoeSize = -9;
//
//    // Created yet? 
//    if (!pTankFactory)
//    {   
//        // Create it
//        pTankFactory = new cTankFactory();
//    }

//    cTankFactory::getTankFactory().CreateATank("Regular Tank");

	if (!pTheTankBuilder)
	{
		pTheTankBuilder = new cTankBuilder();
	}





	std::vector<std::string> vecTankTpyes;
	//    pTankFactory->GetTankTypes(vecTankTpyes);
	//    cTankFactory::get_pTankFactory()->GetTankTypes(vecTankTpyes);
	pTheTankBuilder->GetTankTypes(vecTankTpyes);
	std::cout << "The tank factory can create "
		<< vecTankTpyes.size() << " types of tanks:" << std::endl;
	for (std::string tankTypeString : vecTankTpyes)
	{
		std::cout << tankTypeString << std::endl;
	}
	std::cout << std::endl;

	// Create 1 super tank
//    iTank* pTheTank = cTankFactory::get_pTankFactory()->CreateATank("Super Tank");
	iTank* pTheTank = pTheTankBuilder->CreateATank("Super Tank!");
	if (pTheTank)
	{
		::g_vecTheTanks.push_back(pTheTank);
	}

	// Create 10 tanks
	for (unsigned int count = 0; count != 10; count++)
	{
		//        iTank* pTheTank = cTankFactory::get_pTankFactory()->CreateATank("Regular Tank");
		iTank* pTheTank = pTheTankBuilder->CreateATank("Regular Tank with Shield");
		if (pTheTank)
		{
			::g_vecTheTanks.push_back(pTheTank);
		}
	}

	// Also a hover tank
//    iTank* pHoverTank = cTankFactory::get_pTankFactory()->CreateATank("Hover Tank");
	iTank* pHoverTank = pTheTankBuilder->CreateATank("Hover Tank");
	if (pHoverTank)
	{
		::g_vecTheTanks.push_back(pHoverTank);
	}

	return;
}

void TankStepFrame(double timeStep)
{



	return;
}