struct Vertex {
  float2 position;
  float2 uv;
}

struct VOut {
  float4 position : SV_Position;
  float2 uv : TEXCOORD;
}

struct PushConstant {
  float2x2 transform;
  float2 offset;
  uint textureIndex;
}
[[vk::push_constant]]
ConstantBuffer<PushConstant, ScalarDataLayout> push;

[[vk::binding(0, 0)]]
SamplerState textureSampler;

[[vk::binding(1, 0)]]
Texture2D textures[1000];

[[vk::binding(0, 1)]]
ConstantBuffer<UniformBufferObject> ubo;

struct UniformBufferObject {
  float2x2 screenTransform;
}

[shader("vertex")]
VOut vertexMain(Vertex input)
{
    VOut output;
    output.position = float4(mul(mul(input.position, push.transform) + push.offset,
      ubo.screenTransform), 0.0f, 1.0f);
    output.uv = input.uv;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VOut input) : SV_Target
{
    let texture = textures[push.textureIndex];
    let color = texture.Sample(textureSampler, input.uv);
    return color;
}
