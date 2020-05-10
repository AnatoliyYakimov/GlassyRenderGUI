#version 430 core

in vec2 UV;

out vec3 color;

layout (binding = 1, rgba32f) uniform sampler2D tex;

void main()
{
    color = texture(tex, UV).xyz;
}
