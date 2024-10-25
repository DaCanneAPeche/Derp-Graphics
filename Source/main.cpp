#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "dg_renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "dg_logger.hpp"

void run()
{
    dg::Logger::isEnabled = true;

    dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
    dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};

    dg::VulkanToolBox vulkanToolBox;
    dg::Renderer renderer(windowInfo, vulkanToolBox);

    vulkanToolBox.init(appInfo, renderer.window);
    renderer.init();

    while (!renderer.shouldWindowClose())
    {
        renderer.pollEvents();
        renderer.draw();
    }
    renderer.waitIdle();

    renderer.clean();
}

int main(void)
{
    run();
}
