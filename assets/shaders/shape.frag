#version 450

layout(location = 0) in vec2 fragTextureCoordinates;
layout (location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D textureSampler;

layout(push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
} push;

void main()
{
	outColor = texture(textureSampler, fragTextureCoordinates);
}

