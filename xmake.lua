add_rules("mode.debug", "mode.release")
add_requires("glm", "glfw", "vulkan-memory-allocator", "vulkan-hpp")

target("Derp_Graphics")
    set_kind("binary")
    add_files("Source/*.cpp")
    add_includedirs("Header")

    set_symbols("debug")

    add_packages("glm", "glfw", "vulkan-memory-allocator", "vulkan-hpp")
    set_languages("c++23")

    -- after_build(function (target)
        
        -- for _, file in ipairs(os.files("$(projectdir)/shaders/*")) do
        --     os.run("glslc " .. file .. " -o compiled_shaders/" .. file:match("^.+/(.+)$") .. ".spv")
        -- end

        -- os.ln("$(projectdir)/compiled_shaders", "$(buildir)/$(host)/$(arch)/debug/compiled_shaders")
    -- end)

