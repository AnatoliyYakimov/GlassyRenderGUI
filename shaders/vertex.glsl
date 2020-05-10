#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texturePosition;

out vec2 UV;

void main()
{
    UV = texturePosition;
    gl_Position = vec4(vertexPosition, 1);
}

