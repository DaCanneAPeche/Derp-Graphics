#include "dg_sprite.hpp"

namespace dg
{
  
  Sprite::Sprite(std::shared_ptr<Texture> _texture)
    : texture(_texture)
  {
    const float model_limit = 0.5f;
    float model_width, model_height;

    if (texture->width < texture->height)
    {
      model_height = model_limit;
      model_width = (texture->width * model_height) / texture->height;
    } else
    {
      model_width = model_limit;
      model_height = (texture->height * model_width) / texture->width;
    }

    std::vector<Vertex> vertices 
    {
      {{-model_width, -model_height}, {0.0f, 0.0f}},
        {{model_width, -model_height}, {1.0f, 0.0f}},
        {{model_width, model_height}, {1.0f, 1.0f}},
        {{-model_width, model_height}, {0.0f, 1.0f}}
    };

    std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    model = std::make_unique<Model>(vertices, indices);
  }

  void Sprite::draw(vk::CommandBuffer& commandBuffer,
      vk::PipelineLayout& pipelineLayout) const
  {
        PushConstant push {
            .transform = transform.getMatrix(),
            .offset = transform.translation};
        commandBuffer.pushConstants(pipelineLayout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0, sizeof(PushConstant), &push);

        model->bind(commandBuffer);
        model->draw(commandBuffer);
  }

  vk::DescriptorImageInfo Sprite::getImageInfo() const
  {
    return vk::DescriptorImageInfo(
        texture->sampler,
        texture->imageView,
        vk::ImageLayout::eShaderReadOnlyOptimal);
  }
}
