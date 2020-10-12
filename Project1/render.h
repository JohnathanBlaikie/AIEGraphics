#pragma onceaaaadwd

#include <vector>

#include "glew/GL/glew.h"
#include "glm/glm.hpp"

struct vertex
{
	glm::vec4 pos;
	glm::vec4 col;
	glm::vec2 uv;
	glm::vec4 normal;
};

struct geometry
{
	GLuint vao, vbo, ibo;
	GLuint size;

};

struct shader
{
	GLuint program;
};

struct texture
{
	GLuint handle;
	unsigned width, height, channels;
};

struct light
{
	glm::vec3 direction;
	glm::vec3 color;
};

geometry loadGeometry(const char* filePath);

geometry makeGeometry(vertex* verts, size_t vertCount, 
				     unsigned int* indices, size_t indxCount);

void freeGeometry(geometry& geo);

texture loadTexture(const char* filepath);
texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels);
void freeTexture(texture& tex);

shader loadShader(const char* vertPath, const char* fragPath);
shader makeShader(const char* vertSource, const char* fragSource);

void freeShader(shader& sha);

void draw(const shader& shad, const geometry& geo);

void setUniform(const shader& shad, GLuint location, const glm::mat4& value);
void setUniform(const shader& shad, GLuint location, const texture& tex, int textureSlot);
void setUniform(const shader& shad, GLuint location, float value);
void setUniform(const shader& shad, GLuint location, const glm::vec3 &value);
