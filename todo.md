# Todo list

- [x] Sprite class
- [x] Detach image sampler from the texture
- [x] Code refactoring
- [x] Main class
- [x] Scene system
- [x] ECS
- [x] Pipeline control
- [x] Multi texturing
- [x] Time control
- [x] Redo scene/app structure
- [x] Redo scene registration

## Technical
- [x] Buffer abstraction specialization with templates
- [x] rewrite the fucking swap chain
- [x] Understand what the hell is a renderpass and maybe make an abstraciton around
- [x] Renderer refactoring
- [ ] Keep the image/buffer descriptor infos in memory in the descriptor writers until the end of the frame
- [ ] Try and separate assets from the asset manager to manage how things are loaded more easily (eg. having a different assets struct by scene but also a global one) NOTE: Maybe simply using a static member ?
- [x] Create descriptor sets already in order instead of organizing them later with std::cref
- [ ] Render graph
- [ ] Template update for descriptor and better ressource mananger
- [ ] Buffer syncronization

### Can wait a looonnnnnng timmmme...
- [x] Descriptor Pool abstraction
- [ ] Spine support for multiple renderer with templates
- [ ] Error handling
- [ ] Better pipeline abstraction

## Features
- [ ] TOML support for config
- [ ] 2D camera
- [x] Input library
- [ ] Audio
- [x] Game loop
- [x] **Entity systems**
- [x] Signal handler able to have multiple callbacks
- [ ] Redo input system
