add_rules("mode.debug", "mode.release")
add_requires("glm", "glfw", "vulkan-memory-allocator-hpp", "vulkan-hpp",
    "vulkan-memory-allocator", "stb", "plog", "entt", "reflect-cpp", "imgui")
add_requires("imgui docking", {configs = {glfw = true, vulkan = true}})

target("Derp_Graphics")

    set_kind("binary")
    add_files("Source/**.cpp")
    add_includedirs("Header", "external")
    set_languages("c++23")
    set_symbols("debug")

    add_packages("glm", "glfw", "vulkan-memory-allocator-hpp", "vulkan-hpp",
        "vulkan-memory-allocator", "stb", "plog", "entt", "reflect-cpp", "imgui")

    after_build(function (target)
       
        -- Compile shaders
        for _, file in ipairs(os.files("$(projectdir)/assets/shaders/*")) do
            cmd = "glslc " .. file ..  " -o assets/compiled_shaders/" .. file:match("^.+/(.+)$") .. ".spv"
            print(cmd)
            os.run(cmd)
        end

    end)

