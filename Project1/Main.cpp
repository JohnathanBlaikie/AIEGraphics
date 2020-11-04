#include "context.h"
#include "render.h"

#include "glm/ext.hpp"

#include <limits>
int main()
{
	context game;
	game.init(256, 256, "Custom 3D Renderer");

	//verts
	vertex triVerts[] =
	{
		{ { -.5f, -.5f, 0, 1  }, {0.0f,0.0f,1.0f,1.0f}, {0,0}, {0.0f, 0.0f, -1.0f, 0.0f}  },
		{ {  .5f, -.5f, 0, 1  }, {1.0f,0.0f,.0f,1.0f }, {1,0}, {0.0f, 0.0f, -1.0f, 0.0f}   },
		{ {   0.f, .5f, 0,  1 }, {0.0f,1.0f,.0f,1.0f }, {.5,1}, {0.0f, 0.0f, -1.0f, 0.0f}  }
	};

	vertex quadVerts[] =
	{
		{ { -1.f, -1.f, 0, 1 }, {1.0f, 1.0f, 1.0f, 1.0f }, {0, 0}, {0.0f, 0.0f, -1.0f, 0.0f} },
		{ {  1.f, -1.f, 0, 1 }, {1.0f, 1.0f, 1.0f, 1.0f }, {1, 0}, {0.0f, 0.0f, -1.0f, 0.0f} },
		{ { -1.f,  1.f, 0, 1 }, {1.0f, 1.0f, 1.0f, 1.0f }, {0, 1}, {0.0f, 0.0f, -1.0f, 0.0f} },
		{ {  1.f,  1.f, 0, 1 }, {1.0f, 1.0f, 1.0f, 1.0f }, {1, 1}, {0.0f, 0.0f, -1.0f, 0.0f} }

	};

	//indis
	unsigned int triIndices[] = { 2, 0, 1 };
	unsigned int quadIndices[] = { 3, 2, 0, 0, 1, 3 };

	//make geo
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry quad = makeGeometry(quadVerts, 4, quadIndices, 6);
	geometry cube = loadGeometry("res\\cube.obj");

	texture terry = loadTexture("res\\terry.png");

	//source for vertex shader
	const char* basicVertShader =
		"#version 410\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec4 color;\n"
		"layout (location = 2) in vec2 uv;\n"
		"out vec4 vColor;\n"
		"out vec2 vUV;\n"
		"void main(){ gl_Position = position; vColor = position; vUV = uv; }";

	//source for fragment shader
	const char* basicFragShader =
		"#version 430\n"
		"in vec4 vColor;\n"
		"in vec2 vUV;\n"
		"out vec4 outputColor;\n"
		"layout (location = 3) uniform sampler2D mainTexture;\n"
		//"void main(){ vertColor = vColor; }";
		"void main(){  outputColor = texture(mainTexture, vUV); }";

	const char* mvpVertShader =
		"#version 430\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec4 color;\n"
		"layout (location = 0) uniform mat4 proj;\n"
		"layout (location = 1) uniform mat4 view;\n"
		"layout (location = 2) uniform mat4 model;\n"
		"out vec4 vColor;\n"
		"void main(){ gl_Position = proj * view * model * position;\n"
		"vColor = color; }";

	//make the shader
	//shader basicShader = makeShader(basicVertShader, basicFragShader);
	//shader basicShader = makeShader("res\\basic.vert", "res\\basic.frag");
	//shader fileShader = loadShader("res\\mvp.vert", "res\\basic.frag");
	//shader mvpShader = makeShader(mvpVertShader, basicFragShader);
	shader lightShader = loadShader("res\\light.vert", "res\\light.frag");

	light sun =
	{
		{-1,0,0},{1,1,1}

	};

	light sun2 =
	{
		{1,0,0},{1,1,1}
	};


	//set up matrices
	glm::mat4 camProj = glm::perspective(glm::radians(80.0f), 512.f/512.f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(3, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 triModel = glm::identity<glm::mat4>();

	setUniform(lightShader, 0, camProj);
	setUniform(lightShader, 1, camView);
	setUniform(lightShader, 2, triModel);
	setUniform(lightShader, 3, terry, 0);
			   
	setUniform(lightShader, 5, { 0.1f, 0.1f, 0.1f });
	setUniform(lightShader, 6, sun.color);
	setUniform(lightShader, 7, sun.direction);


	while (!game.shouldClose())
	{
		game.tick();

		triModel = glm::rotate(triModel, glm::radians(0.05f), glm::vec3(0, 0.05f, 0));

		game.clear();

		setUniform(lightShader, 2, triModel);
		setUniform(lightShader, 4, game.time());

		draw(lightShader, cube);
	}
	freeGeometry(triangle);
	freeGeometry(quad);
	freeGeometry(cube);

	//freeShader(basicShader);
	//freeShader(mvpShader);

	game.term();

	return 0; 

}