#version 450

layout(location = 0) in vec2 position;
layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
	vec3 color;
} push;

const vec3 colors[5] = vec3[5](
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		vec3(0.0f, 0.0f, 0.0f) // Should appear as the hightest index is supposed to be 3
		);

void main()
{
	gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
	fragColor = colors[gl_VertexIndex];
}

