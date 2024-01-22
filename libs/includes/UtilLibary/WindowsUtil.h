#include "pch.h"
#include "Camera.h"
#include <glad43/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#pragma once
using namespace std;
class WindowsUtil {
private:
	bool firstMouse = true;
	float lastX = 400, lastY = 300;
public:
	static void framebuffer_size_callback(GLFWwindow*, int _width, int _height)
	{
		glViewport(0, 0, _width, _height);
	}
	/*! @brief Creates a window and its associated context.
 *
 *  This function creates a window and its associated OpenGL or OpenGL ES
 *  context.  Most of the options controlling how the window and its context
 *  should be created are specified with [window hints](@ref window_hints
 * 
 *  @param[in] width The desired width, in screen coordinates, of the window.
 *  This must be greater than zero.
 *  @param[in] height The desired height, in screen coordinates, of the window.
 *  This must be greater than zero.
 *  @param[in] title The initial, UTF-8 encoded window title.
 *  @param[in] monitor The monitor to use for full screen mode, or `NULL` for
 *  windowed mode.
 *  @param[in] share The window whose context to share resources with, or `NULL`
 *  to not share resources.
 */
	GLFWwindow* InitWindows(int _width, int _height, const char* _name, GLFWmonitor* _monitor, GLFWwindow* _share) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow* window = glfwCreateWindow(_width, _height, _name, _monitor, _share);
		if (window == NULL)
		{
			cout << " GLFW cannot init" << endl;
			glfwTerminate();
			return NULL;
		}
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << " GLAD cannot init" << endl;
			return NULL;
		}
		glViewport(0, 0, _width, _height);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		return window;
	}

	GLFWwindow* InitWindowV4(int _width, int _height, const char* _name, GLFWmonitor* _monitor, GLFWwindow* _share) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow* window = glfwCreateWindow(_width, _height, _name, _monitor, _share);
		if (window == NULL)
		{
			cout << " GLFW cannot init" << endl;
			glfwTerminate();
			return NULL;
		}
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << " GLAD cannot init" << endl;
			return NULL;
		}
		glViewport(0, 0, _width, _height);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		return window;
	}

	GLFWwindow* InitWindowV43(int _width, int _height, const char* _name, GLFWmonitor* _monitor, GLFWwindow* _share) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow* window = glfwCreateWindow(_width, _height, _name, _monitor, _share);
		if (window == NULL)
		{
			cout << " GLFW cannot init" << endl;
			glfwTerminate();
			return NULL;
		}
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << " GLAD cannot init" << endl;
			return NULL;
		}
		glViewport(0, 0, _width, _height);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		return window;
	}

	bool processInput(GLFWwindow* window, Camera* camera, float deltaTime)
	{
		const float cameraSpeed = 2.5f * deltaTime; // adjust accordinglys

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) return true;

		if (glfwGetKey(window, GLFW_KEY_N)) camera->ProcessKeyboard(FIX, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			camera->ProcessKeyboard(DOWN, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			camera->ProcessKeyboard(UP, deltaTime);
		}

		return false;
	}

	 void mouse_callback(GLFWwindow* window, double xpos, double ypos, Camera* camera)
	{
		if (firstMouse) // initially set to true
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;

		camera->ProcessMouseMovement(xoffset, yoffset);
	}
};