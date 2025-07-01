add_rules("mode.debug", "mode.release")
add_requires("glm", "glfw", "vulkan-memory-allocator-hpp v3.1.0", "vulkan-hpp v1.3.283",
    "vulkan-memory-allocator v3.1.0", "stb", "plog", "entt", "reflect-cpp", "imgui",
    "spirv-cross", "slang")
add_requires("imgui docking", {configs = {glfw = true, vulkan = true}})

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
target("Derp_Graphics")

    set_kind("binary")
    add_files("Source/**.cpp")
    add_includedirs("Header")
    set_languages("c++20")
    set_symbols("debug")

    add_packages("glm", "glfw", "vulkan-memory-allocator-hpp", "vulkan-hpp",
        "vulkan-memory-allocator", "stb", "plog", "entt", "reflect-cpp", "imgui",
        "spirv-cross", "slang")

    after_build(function (target)
       
        -- Compile shaders
        for _, file in ipairs(os.files("$(projectdir)/assets/shaders/glsl/*")) do
            cmd = "glslc " .. file ..  " -o assets/shaders/spirv/" .. file:match("^.+/(.+)$") .. ".spv"
            print(cmd)
            os.run(cmd)
        end

    end)

