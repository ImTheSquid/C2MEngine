#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec4 color;

void main()
{
    FragColor = texture(ourTexture, TexCoord) * color;
    if (FragColor.a == 0) discard;
}