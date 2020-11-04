#include "context.h"

#include <cstdio>

#include "glew/GL/glew.h"
#include "glfw/glfw3.h"
void APIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message,
	const void* userParam)
{
	printf("\n");
	printf(message);
}

bool context::init(int width, int height, const char* title)
{
	

	int glfwStatus = glfwInit();
	if (glfwStatus == GLFW_FALSE)
	{
		printf("an error occurred with GLFW.");
		return false;
	}

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	int glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
	{
		printf("An error occured with GLEW.");
		return false;
	}


	printf("OpenGL Version: %s\n", (const char*)glGetString(GL_VERSION));
	printf("Renderer: %s\n", (const char*)glGetString(GL_VERSION));
	printf("Vendor: %s\n", (const char*)glGetString(GL_VERSION));
	printf("GLSL: %s\n", (const char*)glGetString(GL_VERSION));

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(errorCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);
	
#endif

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	return false;
}

void context::tick()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}


void context::term()
{
	glfwDestroyWindow(window);
	window = nullptr;

	glfwTerminate();
}

void context::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

bool context::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

float context::time() const
{
	return (float)glfwGetTime();
}

void context::resetTime(float resetValue = 0.0)
{
	//glfw
}