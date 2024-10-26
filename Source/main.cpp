#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "dg_renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"

#include <plog/Log.h>
#include "plog/Init.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Appenders/RollingFileAppender.h"

void run()
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::RollingFileAppender<plog::TxtFormatter> rollingFileAppender("log.txt",
        1000000, 5);
    plog::init(plog::verbose, &consoleAppender).addAppender(&rollingFileAppender);

    dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
    dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};

    dg::VulkanToolBox vulkanToolBox;
    dg::Renderer renderer(windowInfo, vulkanToolBox);

    vulkanToolBox.init(appInfo, renderer.window);
    renderer.init();

    PLOG_INFO << "Init finished";
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
