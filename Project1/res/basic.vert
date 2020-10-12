#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;

void main()
{
	gl_Position = position;
	vColor = color;
	vUV = uv;
}