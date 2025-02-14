#version 450
#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoordinates;

layout(location = 0) out vec2 fragTextureCoordinates;

layout(push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
} push;

layout(std430, set = 1, binding = 0) uniform UniformBufferObject {
  mat2 screenTransform;
} ubo;

void main()
{
	gl_Position = vec4(ubo.screenTransform * push.transform * position + push.offset, 0.0f, 1.0);
  fragTextureCoordinates = textureCoordinates;
}

