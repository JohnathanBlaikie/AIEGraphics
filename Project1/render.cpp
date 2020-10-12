#include "render.h"
#include "glm/ext.hpp"
#include <stddef.h>
#include "tinyobj/tiny_obj_loader.h"
#include "stb/stb_image.h"

#include <vector>
#include <cassert>
#include <cstddef>
#include <fstream>
geometry loadGeometry(const char* filePath)
{
	//load up data from file
	tinyobj::attrib_t vertexAttributes;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool success = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &error, filePath);

	//double-check
	if (!success)
	{
		fprintf(stderr, error.c_str());
		return { };
	}

	//get mesh data
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;


	//form geo data out of mesh
	size_t offset = 0;
	for (size_t i = 0; i < shapes[0].mesh.num_face_vertices.size(); i++)
	{
		unsigned char faceVertices = shapes[0].mesh.num_face_vertices[i];

		assert(faceVertices == 3 && "Unsupported number of face vertices");


		for (unsigned char j = 0; j < faceVertices; ++j)
		{
			tinyobj::index_t idx = shapes[0].mesh.indices[offset + j];
			
			
			tinyobj::real_t posX = vertexAttributes.vertices[3 * idx.vertex_index + 0];
			tinyobj::real_t posY = vertexAttributes.vertices[3 * idx.vertex_index + 1];
			tinyobj::real_t posZ = vertexAttributes.vertices[3 * idx.vertex_index + 2];
		
			tinyobj::real_t colR = 1.0f;
			tinyobj::real_t colG = 1.0f;
			tinyobj::real_t colB = 1.0f;

			tinyobj::real_t texU = vertexAttributes.texcoords[2 * idx.texcoord_index + 0];
			tinyobj::real_t texV = vertexAttributes.texcoords[2 * idx.texcoord_index + 1];

			tinyobj::real_t norX = vertexAttributes.normals[3 * idx.normal_index + 0];
			tinyobj::real_t norY = vertexAttributes.normals[3 * idx.normal_index + 1];
			tinyobj::real_t norZ = vertexAttributes.normals[3 * idx.normal_index + 2];

			vertices.push_back(
				{ 
					{posX,posY,posZ, 1.0f}, 
					{colR, colG, colB, 1.0f}, 
					{texU, texV },
					{norX, norY,norZ, 0.0f}
				});
			indices.push_back(faceVertices * i + j);

		}
		offset += faceVertices;
	}
	return makeGeometry(vertices.data(), vertices.size(), indices.data(), indices.size());

	//return geometry();
}

geometry makeGeometry(vertex* verts, size_t vertCount,
	unsigned int* indices, size_t indxCount)
{
	// make an instance geometry

	geometry newGeo = {};
	newGeo.size = indxCount;

	// generate buffers and VAO
	glGenBuffers(1, &newGeo.vbo);
	glGenBuffers(1, &newGeo.ibo);
	glGenVertexArrays(1, &newGeo.vao);
	
	//bind the VAO and buffers (order matters!)
	glBindVertexArray(newGeo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);
	
	//populate the buffers (Send the data)
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//auto off = offsetof(vertex pos, vertex col);

	//describe the data contained within the buffers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, col));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));

	//unbind the buffers (VAO then the buffers)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//return the geo

	return newGeo;
}

void freeGeometry(geometry& geo)
{
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);

	geo = {};
}


texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels)
{
	assert(channels > 0 && channels < 5);
	GLenum oglFormat = GL_RED;
	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;
		break;
	case 2:
		oglFormat = GL_RG;
		break;
	case 3:
		oglFormat = GL_RGB;
		break;
	case 4:
		oglFormat = GL_RGBA;
		break;
	}
		//Generate a texture
		texture retVal = { 0,width,height,channels };
		glGenTextures(1, &retVal.handle);
		
		//Bind and buffer texture data
		glBindTexture(GL_TEXTURE_2D, retVal.handle);

		glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglFormat, GL_UNSIGNED_BYTE, pixels);

		//set some filtering settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		//return the texture
		return retVal;
	

}

texture loadTexture(const char* filepath) {
	//create some variables to hold onto some data
	int imageWidth, imageHeight, imageFormat;
	unsigned char* rawPixelData = nullptr;

	//use stb to load up the image
	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(filepath, &imageWidth, &imageHeight, &imageFormat, STBI_default);
	assert(rawPixelData != nullptr && "Image failed to load.");
	
	//pass the image to make texture
	texture newTexture = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);
	assert(newTexture.handle != 0 && "Failed to create texture.");

	//free the image data
	stbi_image_free(rawPixelData);

	//return texture
	return newTexture;

}

void freeTexture(texture& tex)
{
	glDeleteTextures(1, &tex.handle);
	tex = {};
}

bool checkShader(GLuint target, const char* humanReadableName = "")
{
	assert(glIsShader(target) && "target is not a shader -- cannot check");
	GLint shaderCompileStatus = 0;
	glGetShaderiv(target, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (shaderCompileStatus != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar message[1024];
		glGetShaderInfoLog(target, 1024, &logLength, message);
		fprintf(stderr, "\n[ERROR] %s Shader \n %s", humanReadableName, message);

		// return an empty shader if it fails
		return false;
	}
	return true;
}

shader loadShader(const char* vertPath, const char* fragPath)
{
	//extract the text from the vertex file
	std::fstream vertFile(vertPath, std::ios::in);
	assert(vertFile.is_open() && "Failed to open vertex shader file.");
	
	std::string vertSource;
	for (std::string line; std::getline(vertFile, line);)
	{
		vertSource += line + "\n";
	}
	vertFile.close();

	//extract the text from the frag file
	std::fstream fragFile(fragPath, std::ios::in);
	assert(fragFile.is_open() && "Failed to open frag shader file.");
	std::string fragSource;
	for (std::string line; std::getline(fragFile, line);)
	{
		fragSource += line + "\n";
	}
	fragFile.close();


	return makeShader(vertSource.c_str(),fragSource.c_str());
}

shader makeShader(const char* vertSource, const char* fragSource)
{
	//Make a shader program
	shader newShad = {};
	newShad.program = glCreateProgram();

	//Create the shaders
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	//compile the shaders
	glShaderSource(vert, 1, &vertSource, 0);
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);
	glCompileShader(frag);

	//error checking
	GLint successfulComp;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &successfulComp);
	if (successfulComp != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vert, 1024, &log_length, message);
		printf(message);
		//assert(false);
	}
	glGetShaderiv(frag, GL_COMPILE_STATUS, &successfulComp);
	if (successfulComp != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(frag, 1024, &log_length, message);
		printf(message);
		//assert(false);

	}
	
	//attach the shaders
	glAttachShader(newShad.program, vert);
	glAttachShader(newShad.program, frag);

	//link the shader program
	glLinkProgram(newShad.program);

	//delete the shaders
	glDeleteShader(vert);
	glDeleteShader(frag);

	//return the shaders
	return newShad;
}


void freeShader(shader& sha) {
	glDeleteProgram(sha.program);
	sha = {};
}

void draw(const shader& shad, const geometry& geo)
{
	//specify which shader
	glUseProgram(shad.program);

	//specify which VAO
	glBindVertexArray(geo.vao);

	//draw
	glDrawElements(GL_TRIANGLES, geo.size, GL_UNSIGNED_INT, 0);
}

void setUniform(const shader& shad, GLuint location, const glm::mat4& value)
{
	glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));

}

void setUniform(const shader& shad, GLuint location, const texture& tex, int textureSlot)
{
	//set up current texture in a slot
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, tex.handle);

	//pass the slot number to uniform
	glProgramUniform1i(shad.program, location, textureSlot);

}

void setUniform(const shader& shad, GLuint location, float value)
{
	glProgramUniform1f(shad.program, location, value);
}

void setUniform(const shader& shad, GLuint location, const glm::vec3& value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));

}