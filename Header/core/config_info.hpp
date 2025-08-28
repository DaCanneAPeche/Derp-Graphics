#pragma once

#include <string>
#include <array>

#include "rfl/toml.hpp"
#include "rfl.hpp"

namespace dg
{
  struct ConfigInfo
  {
    struct
    {
      std::string name;
      std::array<uint8_t, 3> version;
    } application;

    struct
    {
      std::string name;
      std::array<uint16_t, 2> size; 
    } window;

    static ConfigInfo fromTOML(const std::string& filePath)
    {
      rfl::Result<dg::ConfigInfo> config = rfl::toml::load<dg::ConfigInfo>("assets/derp-config.toml");
      return config.value();
    }
  };
}
