#define VMA_IMPLEMENTATION
#include <iostream>
#include "dg_renderer.hpp"
#include "dg_logger.hpp"

void run()
{
    dg::Logger::isEnabled = true;

    dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
    dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};

    dg::Renderer renderer(windowInfo, appInfo);
    while (!renderer.shouldWindowClose())
    {
        renderer.pollEvents();
        renderer.draw();
    }
    renderer.waitIdle();
}

int main(void)
{
    try {
        run();
    } catch(const std::exception &error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
}
