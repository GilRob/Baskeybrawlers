#version 420

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;

out vec2 texcoord;
out vec3 norm;
out vec3 pos;

void main()
{
	texcoord = in_uv;
	gl_Position = vec4(in_vert, 1.0f);
}