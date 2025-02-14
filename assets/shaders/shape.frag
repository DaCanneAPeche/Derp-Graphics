#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 fragTextureCoordinates;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
  uint textureId;
} push;

layout(set = 0, binding = 0) uniform sampler samp;
layout(set = 0, binding = 1) uniform texture2D textures[];

layout(std140, set = 1, binding = 0) uniform UniformBufferObject {
  mat2 screenTransform;
} ubo;

void main()
{
	outColor = texture(sampler2D(textures[push.textureId], samp), fragTextureCoordinates);
}

