#version 330 core

layout (location = 0) in vec2 inPos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * transform * vec4(inPos, 0, 1);
}