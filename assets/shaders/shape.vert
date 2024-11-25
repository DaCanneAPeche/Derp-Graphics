#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoordinates;

layout(location = 0) out vec2 fragTextureCoordinates;

layout(push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
} push;

void main()
{
	gl_Position = vec4(push.transform * (position) + push.offset, 0.0f, 1.0);
  fragTextureCoordinates = textureCoordinates;
}

