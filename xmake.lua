add_rules("mode.debug", "mode.release")
add_requires("glm", "glfw", "vulkan-memory-allocator-hpp", "vulkan-hpp", "vulkan-memory-allocator")

target("Derp_Graphics")
    set_kind("binary")
    add_files("Source/*.cpp")
    add_includedirs("Header")

    set_symbols("debug")

    add_packages("glm", "glfw", "vulkan-memory-allocator-hpp", "vulkan-hpp", "vulkan-memory-allocator")
    set_languages("c++23")

    after_build(function (target)
       
        -- Compile shaders
        for _, file in ipairs(os.files("$(projectdir)/assets/shaders/*")) do
            cmd = "glslc " .. file ..  " --target-spv=spv1.6 --target-env=vulkan1.3 -o assets/compiled_shaders/" .. file:match("^.+/(.+)$") .. ".spv"
            print(cmd)
            os.run(cmd)
        end

    end)

