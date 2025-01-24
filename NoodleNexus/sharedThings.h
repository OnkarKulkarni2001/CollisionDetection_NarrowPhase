#pragma once

#include "GLCommon.h"

#include "cBasicFlyCamera/cBasicFlyCamera.h"
#include "cLightManager.h"

extern cBasicFlyCamera* g_pFlyCamera;

extern cLightManager* g_pLightManager;
extern unsigned int g_selectedLightIndex;


// GLFW callback function signatures
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
// Mouse entering and leaving the window
void cursor_enter_callback(GLFWwindow* window, int entered);
// Mouse button...
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
// Mouse scroll wheel
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// WE call these (not GLFW)
void handleKeyboardAsync(GLFWwindow* window, float rightDistance, float leftDistance, float frontDistance, float backDistance, float collisionThreshold);
void handleMouseAsync(GLFWwindow* window);

// Can call this from anything that has this header file
void DrawDebugSphere(glm::vec3 position, glm::vec4 RGBA, float scale, GLuint program);

extern bool g_bShowDebugSpheres;

glm::vec3 g_rgb_from_HTML(unsigned int red, unsigned int green, unsigned int blue);

// HACK: Show "targeting LASER"
extern bool g_bShowLASERBeam;