#version 450

layout(location = 0) in vec2 fragTextureCoordinates;
layout(location = 0) out vec4 outColor;

// layout(set = 0, binding = 0) uniform sampler2D textureSampler;
layout(set = 0, binding = 0) uniform sampler samp;
layout(set = 0, binding = 1) uniform texture2D textures[];

layout(push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
} push;

void main()
{
	outColor = texture(sampler2D(textures[0], samp), fragTextureCoordinates);
}

