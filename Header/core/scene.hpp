#pragma once

#include <memory>
#include "core/signal_handler.hpp"
#include "core/inputs.hpp"
#include "vulkan_renderer/asset_manager.hpp"

namespace dg
{
  class Application;

  class Scene
  {
    public:
      virtual void start() {};
      virtual void update() {};
      virtual void lateUpdate() {};
      virtual void end() {};
      virtual std::unordered_map<uint32_t, std::string> getAssets() {return {};};

      Application* app = nullptr; 
      SignalHandler signalHandler;
      std::unique_ptr<AssetManager> assetManager = nullptr;

      void init(Application* application);

      void bindInput(std::vector<Key>& keys, dg::config::Signals signal,
          KeyboardMods necessaryMods = KeyboardMods::none,
          KeyboardMods forbiddenMods = KeyboardMods::none);

      void bindInput(Key key, dg::config::Signals signal,
          KeyboardMods necessaryMods = KeyboardMods::none,
          KeyboardMods forbiddenMods = KeyboardMods::none);

    protected:

    private:
  };
}
