#include "common.h"
#include <stdlib.h>

GLFWwindow* createWindow(int w, int h) {

	// Initialize glfw, which will create and setup an OpenGL context.
	if(!glfwInit()) {
		return NULL;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(w, h, "sr_gui", NULL, NULL);

	if(!window) {
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	return window;
}
