#version 330 core
// Polygon shader for framebuffer ONLY
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 TexCoords;

uniform mat4 transform;

void main() {
	gl_Position = transform * vec4(inPos, 0, 1);
	TexCoords = inTexCoords;
}