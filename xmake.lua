add_rules("mode.debug", "mode.release")
add_requires("glm", "glfw", "vulkan-memory-allocator-hpp", "vulkan-hpp",
    "vulkan-memory-allocator", "stb", "plog", "entt", "reflect-cpp", "imgui",
    "spirv-cross")
add_requires("imgui docking", {configs = {glfw = true, vulkan = true}})

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
target("Derp_Graphics")

    set_kind("binary")
    add_files("Source/**.cpp")
    add_includedirs("Header", "external")
    set_languages("c++20")
    set_symbols("debug")

    add_packages("glm", "glfw", "vulkan-memory-allocator-hpp", "vulkan-hpp",
        "vulkan-memory-allocator", "stb", "plog", "entt", "reflect-cpp", "imgui",
        "spirv-cross")

    after_build(function (target)
       
        -- Compile shaders
        for _, file in ipairs(os.files("$(projectdir)/assets/shaders/*")) do
            cmd = "glslc " .. file ..  " -o assets/compiled_shaders/" .. file:match("^.+/(.+)$") .. ".spv"
            print(cmd)
            os.run(cmd)
        end

    end)

