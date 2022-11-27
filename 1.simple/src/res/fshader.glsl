#version 330 core
in vec3 ourColor;
in vec2 ourTexCoord;
out vec4 color;

uniform sampler2D ourTexture;

void main()
{
    color = texture(ourTexture, ourTexCoord) * vec4(ourColor, 1.0f);
}