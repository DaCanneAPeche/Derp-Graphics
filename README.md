# Derp Graphics

Just a game engine in the making using the Vulkan API.
It's a in pause right now while I work on another project.

## Dependencies

You need the vulkan SDK installed in 1.3 (1.2 probably works), with `glslc` installed in order to compile the shaders.
The build chain will install every libraries needed.

## Build

The project uses [xmake](https://xmake.io/) as a build tool, (it's very easy to configure using lua, you should try it).
Run the `xmake` command after installation to build the project (and install dependencies other than the ones listed above) and `xmake run` to run it.

## Features
For now supports things like an Entity Component System (using the entt library), inputs, descriptor binding, imgui, assets (more or less), 
signal/events (need to be remade though)...
Still missing too much to be fully usable and probably not very safe. 3D isn't implemented yet
but a lot of the abstraction needed is already there.

